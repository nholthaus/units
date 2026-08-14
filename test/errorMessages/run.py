#!/usr/bin/env python3
"""
Template error-message harness for nholthaus/units.

For each case in cases/*.cpp it:
  - compiles with -fsyntax-only and records exit code + wall time,
  - honours in-file directives:
      // expect: fail            (default: pass)  -- whether the compile SHOULD fail
      // expect-match: <substr>  the error/output MUST contain <substr>   (readability assertion)
      // forbid-match: <substr>  the error/output must NOT contain <substr> (anti-soup assertion)
  - records a trimmed error excerpt.

Readability is graded by the expect-match/forbid-match directives: they assert the diagnostic
names the friendly `strong` types (meters_, hertz_, ...) and never the raw conversion_factor soup.

Usage:
  run.py --cc g++ --std c++23 --include ../../include [--json out.json] [--label baseline]
Compare two runs:
  run.py --compare baseline.json proto.json
"""
import argparse, json, re, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent

def parse_directives(text):
    d = {"expect_fail": False, "expect_match": [], "forbid_match": []}
    for line in text.splitlines():
        m = re.search(r'//\s*expect:\s*(\w+)', line)
        if m:
            d["expect_fail"] = (m.group(1).lower() == "fail")
        m = re.search(r'//\s*expect-match:\s*(.+?)\s*$', line)
        if m:
            d["expect_match"].append(m.group(1))
        m = re.search(r'//\s*forbid-match:\s*(.+?)\s*$', line)
        if m:
            d["forbid_match"].append(m.group(1))
    return d

def is_msvc(cc):
    return Path(cc).stem.lower() == "cl"

def compile_cmd(cc, std, include, path):
    # Syntax-only compile that PRODUCES the diagnostic but no object file, across compilers.
    if is_msvc(cc):
        # /Zs = syntax check only; /EHsc for standard C++; /permissive- for conformance; /diagnostics:classic
        # keeps the message form stable. /std:c++latest when std is c++23 (older MSVC lacks /std:c++23).
        stdflag = "/std:c++latest" if std in ("c++23", "c++2b") else f"/std:{std}"
        return [cc, "/nologo", "/Zs", "/EHsc", "/permissive-", stdflag, f"/I{include}", str(path)]
    return [cc, f"-std={std}", "-I", include, "-fsyntax-only", str(path)]

def normalize(diag):
    # Make token matching COMPILER-INDEPENDENT: the exact diagnostic text differs by compiler (MSVC prefixes
    # class/struct/enum and writes 'units::length::meters<double>' with different spacing than g++/clang). Strip the
    # elaborated-type keywords and collapse all whitespace so an expect-match like 'meters<double>' or the bare
    # 'meters' matches on g++, clang, AND MSVC alike.
    d = re.sub(r'\b(class|struct|enum|typename)\s+', '', diag)
    d = re.sub(r'\s+', ' ', d)
    return d

def run_case(path, cc, std, include):
    text = path.read_text()
    d = parse_directives(text)
    cmd = compile_cmd(cc, std, include, path)
    t0 = time.perf_counter()
    proc = subprocess.run(cmd, capture_output=True, text=True, timeout=180)
    dt = time.perf_counter() - t0
    out = proc.stderr + proc.stdout
    compiled = (proc.returncode == 0)
    norm = normalize(out)

    problems = []
    if d["expect_fail"] and compiled:
        problems.append("expected compile FAILURE but it compiled")
    if not d["expect_fail"] and not compiled:
        problems.append("expected compile SUCCESS but it FAILED")
    # Match against the whitespace/keyword-normalized diagnostic so tokens are compiler-portable.
    for sub in d["expect_match"]:
        if normalize(sub) not in norm:
            problems.append(f"missing readable token: {sub!r}")
    for sub in d["forbid_match"]:
        if normalize(sub) in norm:
            problems.append(f"contains forbidden soup: {sub!r}")

    # first error line, for the report
    err_excerpt = ""
    for line in out.splitlines():
        if "error:" in line:
            err_excerpt = line.strip()
            break

    return {
        "case": path.name,
        "compiled": compiled,
        "expect_fail": d["expect_fail"],
        "seconds": round(dt, 3),
        "ok": len(problems) == 0,
        "problems": problems,
        "error_excerpt": err_excerpt[:300],
    }

def do_run(args):
    cases = sorted((HERE / "cases").glob("*.cpp"))
    results = [run_case(c, args.cc, args.std, args.include) for c in cases]
    total_time = round(sum(r["seconds"] for r in results), 3)
    passed = sum(1 for r in results if r["ok"])
    report = {"label": args.label, "cc": args.cc, "std": args.std,
              "total_seconds": total_time, "passed": passed, "count": len(results),
              "results": results}
    print(f"\n=== error-message harness [{args.label}] :: {args.cc} {args.std} ===")
    for r in results:
        mark = "PASS" if r["ok"] else "FAIL"
        print(f"  [{mark}] {r['case']:<32} {r['seconds']:>6.3f}s  "
              f"{'(compiled)' if r['compiled'] else '(rejected)'}")
        for p in r["problems"]:
            print(f"          - {p}")
        if not r["ok"] and r["error_excerpt"]:
            print(f"          err: {r['error_excerpt']}")
    print(f"  ---- {passed}/{len(results)} cases OK, total {total_time}s ----")
    if args.json:
        Path(args.json).write_text(json.dumps(report, indent=2))
        print(f"  wrote {args.json}")
    return 0 if passed == len(results) else 1

def do_compare(a_path, b_path):
    a = json.loads(Path(a_path).read_text())
    b = json.loads(Path(b_path).read_text())
    ab = {r["case"]: r for r in a["results"]}
    bb = {r["case"]: r for r in b["results"]}
    print(f"\n=== compare  A=[{a['label']}]  vs  B=[{b['label']}] ===")
    print(f"  total time: A={a['total_seconds']}s  B={b['total_seconds']}s  "
          f"(Δ {round(b['total_seconds']-a['total_seconds'],3):+}s)")
    for case in sorted(set(ab) | set(bb)):
        ra, rb = ab.get(case), bb.get(case)
        if not ra or not rb:
            print(f"  {case}: only in {'A' if ra else 'B'}"); continue
        dt = round(rb["seconds"] - ra["seconds"], 3)
        flags = []
        if ra["ok"] != rb["ok"]:
            flags.append(f"OK {ra['ok']}->{rb['ok']}")
        if ra["compiled"] != rb["compiled"]:
            flags.append(f"compiled {ra['compiled']}->{rb['compiled']}")
        note = ("  <<< " + ", ".join(flags)) if flags else ""
        print(f"  {case:<32} A={ra['seconds']:>6.3f}s B={rb['seconds']:>6.3f}s  Δ{dt:+}s{note}")
    return 0

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cc", default="g++")
    ap.add_argument("--std", default="c++23")
    ap.add_argument("--include", default=str(HERE.parent.parent / "include"))
    ap.add_argument("--json")
    ap.add_argument("--label", default="run")
    ap.add_argument("--compare", nargs=2, metavar=("A.json", "B.json"))
    args = ap.parse_args()
    if args.compare:
        sys.exit(do_compare(*args.compare))
    sys.exit(do_run(args))

if __name__ == "__main__":
    main()
