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
Capture verbatim diagnostics for the docs (so a shown error can never drift from the compiler):
  run.py --emit-doc --cc g++ --compiler-label "GCC 13" --include ../../include --out-dir ../../docs/diagnostics
"""
import argparse, json, os, re, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent

def parse_directives(text):
    d = {"expect_fail": False, "expect_match": [], "expect_match_gcc": [], "expect_match_msvc": [],
         "forbid_match": [], "forbid_match_gcc": [], "forbid_match_msvc": [], "flags": [], "flags_msvc": [],
         "max_lines": None, "grades_compiler": False}
    for line in text.splitlines():
        m = re.search(r'//\s*expect:\s*(\w+)', line)
        if m:
            d["expect_fail"] = (m.group(1).lower() == "fail")
        # Compiler-specific readable-token expectations. The exact diagnostic differs by compiler: GCC/clang
        # surface a thrown std::format_error's string in the message, while MSVC reports only C7595 ("call to
        # immediate function is not a constant expression") for the same consteval rejection. `expect-match:`
        # applies to every compiler; `expect-match-gcc:` / `expect-match-msvc:` apply only to that compiler.
        m = re.search(r'//\s*expect-match-gcc:\s*(.+?)\s*$', line)
        if m:
            d["expect_match_gcc"].append(m.group(1))
        m = re.search(r'//\s*expect-match-msvc:\s*(.+?)\s*$', line)
        if m:
            d["expect_match_msvc"].append(m.group(1))
        if re.search(r'//\s*grades:\s*compiler\s*$', line):
            d["grades_compiler"] = True
            continue
        m = re.search(r'//\s*expect-max-lines:\s*(\d+)\s*$', line)
        if m:
            d["max_lines"] = int(m.group(1))
            continue
        m = re.search(r'//\s*expect-match:\s*(.+?)\s*$', line)
        if m:
            d["expect_match"].append(m.group(1))
        # Compiler-specific anti-soup guards, symmetric to expect-match-gcc/-msvc. A token that is genuinely
        # absent on GCC/clang but that MSVC legitimately spells (e.g. the `units::cos(AngleUnit)` candidate
        # signature carries `dimension_t<`/`conversion_factor<...>` in an overload-resolution note) is forbidden
        # only where it is truly soup. `forbid-match:` applies to every compiler; `forbid-match-gcc:` /
        # `forbid-match-msvc:` apply only to that compiler. Checked before the generic form so the `-gcc`/`-msvc`
        # suffix is not swallowed by the generic pattern.
        m = re.search(r'//\s*forbid-match-gcc:\s*(.+?)\s*$', line)
        if m:
            d["forbid_match_gcc"].append(m.group(1))
        m = re.search(r'//\s*forbid-match-msvc:\s*(.+?)\s*$', line)
        if m:
            d["forbid_match_msvc"].append(m.group(1))
        m = re.search(r'//\s*forbid-match:\s*(.+?)\s*$', line)
        if m:
            d["forbid_match"].append(m.group(1))
        m = re.search(r'//\s*flags:\s*(.+?)\s*$', line)
        if m:
            d["flags"].extend(m.group(1).split())
        m = re.search(r'//\s*flags-msvc:\s*(.+?)\s*$', line)
        if m:
            d["flags_msvc"].extend(m.group(1).split())
    return d

def is_msvc(cc):
    return Path(cc).stem.lower() == "cl"

def compile_cmd(cc, std, include, path, extra=None, extra_msvc=None):
    # Syntax-only compile that PRODUCES the diagnostic but no object file, across compilers. A case may add
    # per-case compiler flags (e.g. warning flags to surface a diagnostic) via a // flags: / // flags-msvc: directive.
    if is_msvc(cc):
        # /Zs = syntax check only; /EHsc for standard C++; /permissive- for conformance; /diagnostics:classic
        # keeps the message form stable. /std:c++latest when std is c++23 (older MSVC lacks /std:c++23).
        stdflag = "/std:c++latest" if std in ("c++23", "c++2b") else f"/std:{std}"
        return [cc, "/nologo", "/Zs", "/EHsc", "/permissive-", stdflag, f"/I{include}", *(extra_msvc or []), str(path)]
    return [cc, f"-std={std}", "-I", include, "-fsyntax-only", *(extra or []), str(path)]

def normalize(diag):
    # Make token matching COMPILER-INDEPENDENT: the exact diagnostic text differs by compiler (MSVC prefixes
    # class/struct/enum and writes 'units::length::meters<double>' with different spacing than g++/clang). Strip the
    # elaborated-type keywords and collapse all whitespace so an expect-match like 'meters<double>' or the bare
    # 'meters' matches on g++, clang, AND MSVC alike.
    d = re.sub(r'\b(class|struct|enum|typename)\s+', '', diag)
    d = re.sub(r'\s+', ' ', d)
    return d

def run_case(path, cc, std, include):
    # Read the case and the compiler's diagnostics as UTF-8: cases (and the library's diagnostics) contain
    # non-ASCII (°, −, →, — in comments and message strings), which the platform-default codec (cp1252 on
    # Windows CI) cannot decode. `errors="replace"` keeps a stray byte from aborting the whole run.
    text = path.read_text(encoding="utf-8", errors="replace")
    d = parse_directives(text)
    cmd = compile_cmd(cc, std, include, path, extra=d["flags"], extra_msvc=d["flags_msvc"])
    t0 = time.perf_counter()
    proc = subprocess.run(cmd, capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=180)
    dt = time.perf_counter() - t0
    out = (proc.stderr or "") + (proc.stdout or "")
    compiled = (proc.returncode == 0)

    # SELF-GRADING GUARD. A compiler echoes the offending source line, comments included. When that line is the
    # CASE's own, a phrase parked in its prose satisfies its own expect-match and the case passes with the library's
    # message destroyed. The echo of a LIBRARY line is different -- for a constexpr `throw` the echoed expression IS
    # the message the user reads -- so only the case's own echoed lines are removed from the graded text.
    own_lines = {ln.strip() for ln in text.splitlines() if ln.strip()}
    graded = "\n".join(l for l in out.splitlines()
                       if not (re.match(r'\s*\d+\s*\|', l) and l.split('|', 1)[1].strip() in own_lines))
    norm = normalize(graded)

    problems = []
    if d["expect_fail"] and compiled:
        problems.append("expected compile FAILURE but it compiled")
    if not d["expect_fail"] and not compiled:
        problems.append("expected compile SUCCESS but it FAILED")
    # Match against the whitespace/keyword-normalized diagnostic so tokens are compiler-portable. The
    # generic expect-match applies to every compiler; the per-compiler sets add tokens only for the compiler
    # in use (a diagnostic that a compiler simply does not emit — e.g. MSVC not surfacing a thrown
    # std::format_error string — is asserted against that compiler's own form instead).
    expected = list(d["expect_match"])
    expected += d["expect_match_msvc"] if is_msvc(cc) else d["expect_match_gcc"]
    for sub in expected:
        if normalize(sub) not in norm:
            problems.append(f"missing readable token: {sub!r}")
    forbidden = list(d["forbid_match"])
    forbidden += d["forbid_match_msvc"] if is_msvc(cc) else d["forbid_match_gcc"]
    for sub in forbidden:
        if normalize(sub) in norm:
            problems.append(f"contains forbidden soup: {sub!r}")
    if d["max_lines"] is not None:
        n = len([l for l in out.splitlines() if l.strip()])
        if n > d["max_lines"]:
            problems.append(f"diagnostic is {n} lines, over the {d['max_lines']}-line bound: a readable rejection "
                            f"names the problem without a wall of declined overloads")

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

def do_check_doc(args):
    """Re-emit the captured diagnostics and diff them against the committed pages."""
    import tempfile, filecmp
    out_dir = Path(args.out_dir)
    tmp = Path(tempfile.mkdtemp(prefix="units_docdiff_"))
    emit_args = argparse.Namespace(**{**vars(args), "out_dir": str(tmp), "emit_doc": True})
    do_emit_doc(emit_args)
    drift, missing = [], []
    for doc_id, case_name in DOC_CASES.items():
        slug = args.compiler_slug or "gcc13"
        name = f"error_{doc_id}.{slug}.md"
        fresh, committed = tmp / name, out_dir / name
        if not committed.exists():
            missing.append(name); continue
        if not fresh.exists():
            continue
        if fresh.read_text(encoding="utf-8") != committed.read_text(encoding="utf-8"):
            drift.append(name)
    if missing:
        print("MISSING committed pages:", ", ".join(missing))
    if drift:
        print(f"{len(drift)} committed page(s) no longer match what the compiler emits:")
        for n in drift:
            print(f"  [DRIFT] {n}")
        print(f"  regenerate with: python3 test/errorMessages/run.py --emit-doc --compiler-slug {args.compiler_slug or 'gcc13'} "
              f"--compiler-label \"$({args.cc} --version | head -1)\" --include include")
        return 1
    if missing:
        return 1
    print(f"all {len(DOC_CASES)} captured diagnostics match the committed pages")
    return 0

def compiler_banner(cc):
    """The compiler's own version line, so a report never claims a compiler it did not run."""
    try:
        out = subprocess.run([cc, "--version"], capture_output=True, text=True, timeout=30).stdout
        return out.splitlines()[0].strip() if out else cc
    except Exception:
        return cc

def do_run(args):
    cases = sorted((HERE / "cases").glob("*.cpp"))
    # Each case is an independent compiler invocation with no shared state, so the cases run concurrently across
    # a thread pool (the work is dominated by the compiler subprocess, which releases the GIL). Results are
    # gathered back into the original sorted order so the report and pass/fail are deterministic regardless of
    # completion order. --jobs 1 forces the serial path.
    jobs = args.jobs if args.jobs and args.jobs > 0 else (os.cpu_count() or 1)
    if jobs == 1:
        results = [run_case(c, args.cc, args.std, args.include) for c in cases]
    else:
        from concurrent.futures import ThreadPoolExecutor
        with ThreadPoolExecutor(max_workers=jobs) as pool:
            results = list(pool.map(lambda c: run_case(c, args.cc, args.std, args.include), cases))
    total_time = round(sum(r["seconds"] for r in results), 3)
    passed = sum(1 for r in results if r["ok"])
    report = {"label": args.label, "cc": args.cc, "std": args.std,
              "total_seconds": total_time, "passed": passed, "count": len(results),
              "results": results}
    print(f"\n=== error-message harness [{args.label}] :: {compiler_banner(args.cc)} {args.std} ===")
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

# The subset of cases whose verbatim diagnostics the documentation quotes. Keyed by the id the docs
# reference; value is the case filename. These are the "mistakes units is designed to reject" (E1-E6)
# plus the positive include-ordering case (E7).
DOC_CASES = {
    "incompatible-add":    "readable_add_incompatible.cpp",
    "wrong-result-type":   "readable_wrong_result_type.cpp",
    "narrowing-to-int":    "readable_narrowing_to_int.cpp",
    "scalar-plus-unit":    "readable_scalar_plus_unit.cpp",
    "trig-needs-angle":    "readable_trig_needs_angle.cpp",
    "compare-dimensions":  "readable_compare_across_dimensions.cpp",
    # The affine and decibel diagnostics, whose messages the README quotes verbatim. Registered here so the quoted
    # text is regenerated from the compiler rather than hand-typed, and cannot drift as the diagnostics change.
    "add-scalar-to-affine":    "add_scalar_to_affine.cpp",
    "sub-scalar-from-affine":  "sub_scalar_from_affine.cpp",
    "scale-decibel-level":     "scale_decibel_level.cpp",
    "add-scalar-to-decibel":   "add_scalar_to_decibel.cpp",
    "add-two-decibel-levels":  "decibel_level_plus_level.cpp",
    "transcendental-of-decibel": "log_of_decibel_gain.cpp",
    "add-different-dimensions": "add_different_dimensions.cpp",
    "multiply-in-place":       "multiply_in_place_by_quantity.cpp",
}

def trim_diagnostic(out, abs_path, case_name, max_lines=8):
    # Keep the diagnostic readable in a doc: take from the first "error:" line through a modest window,
    # dropping the compiler's "In file included from" preamble and note-spam beyond the window. Verbatim
    # otherwise -- this is the compiler's real text, never paraphrased. The only rewrite is replacing the
    # absolute temp path of the case file with its bare name, so a snippet is portable across checkouts.
    out = out.replace(str(abs_path), case_name)
    out = re.sub(r'(include/units/\w+\.h):\d+(:\d+)?', r'\1:LINE', out)
    normalized, in_library = [], False
    for line in out.splitlines():
        if re.search(r'include/units/\w+\.h:LINE', line):
            in_library = True
        elif case_name in line:
            in_library = False
        if in_library:
            line = re.sub(r'^\s*\d+\s*\|', ' LINE |', line)
        normalized.append(line)
    out = "\n".join(normalized)
    lines = out.splitlines()
    # NOTE the pattern: there is no `\b` after the colon. A word boundary there would sit between two non-word
    # characters and never match, so `start` would fall back to 0 and every generated page would carry the
    # compiler's preamble instead of its diagnostic.
    start = next((i for i, l in enumerate(lines) if re.search(r'\berror:|error C\d', l)), 0)
    # A library sentence fires from a template BODY, so the compiler reports the user's own location ABOVE the
    # error line ("required from here" / "the template being instantiated"). Keep that line: a page showing only
    # the library's line does not tell a reader where in their code the mistake is.
    for i in range(max(0, start - 4), start):
        if case_name in lines[i] and re.search(r'required from here|being instantiated|In instantiation', lines[i]):
            start = i
            break
    kept = lines[start:start + max_lines]
    return "\n".join(kept).rstrip()

def do_emit_doc(args):
    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    label = args.compiler_label or args.cc
    for doc_id, case_name in DOC_CASES.items():
        path = HERE / "cases" / case_name
        proc = subprocess.run(compile_cmd(args.cc, args.std, args.include, path),
                              capture_output=True, text=True, encoding="utf-8", errors="replace", timeout=180)
        diag = trim_diagnostic((proc.stderr or "") + (proc.stdout or ""), path, case_name)
        snippet = (f"<!-- generated by test/errorMessages/run.py --emit-doc; do not edit by hand. -->\n"
                   f"<!-- case: {case_name}   compiler: {label} -->\n"
                   f"```text\n{diag}\n```\n")
        dest = out_dir / f"error_{doc_id}.{args.compiler_slug or 'diag'}.md"
        dest.write_text(snippet)
        print(f"  wrote {dest}  ({len(diag.splitlines())} lines from {label})")
    return 0

def do_mutate(args):
    """Wreck the library's diagnostic prose in a copy of the headers; every case that grades a library sentence must fail."""
    import shutil, tempfile
    src = Path(args.include)
    tmp = Path(tempfile.mkdtemp(prefix="units_mutate_"))
    shutil.copytree(src, tmp / "include")
    wrecked = 0
    for header in (tmp / "include").rglob("*.h"):
        text = header.read_text(encoding="utf-8", errors="replace")
        new, n = re.subn(r'"units(::\w+)?: [^"]*"', '"WRECKED_DIAGNOSTIC"', text)
        if n:
            header.write_text(new, encoding="utf-8")
            wrecked += n
    print(f"wrecked {wrecked} diagnostic strings in {tmp}/include")

    # A case grades a LIBRARY sentence only if one of its phrases sits inside a diagnostic STRING LITERAL. Matching
    # the headers' whole text would count `meters<` or `operator+` -- which any diagnostic names -- and classify
    # almost every case as a grader.
    library_prose = "\n".join(
        "\n".join(re.findall(r'"units(?:::\w+)?: [^"]*"', h.read_text(encoding="utf-8", errors="replace")))
        for h in src.rglob("*.h"))
    graders, survivors = [], []
    for path in sorted((HERE / "cases").glob("*.cpp")):
        d = parse_directives(path.read_text(encoding="utf-8", errors="replace"))
        phrases = d["expect_match"] + d["expect_match_gcc"] + d["expect_match_msvc"]
        if d["grades_compiler"]:
            continue
        if not any(len(p) >= 12 and p in library_prose for p in phrases):
            continue
        graders.append(path.name)
        r = run_case(path, args.cc, args.std, str(tmp / "include"))
        if r["ok"]:
            survivors.append(path.name)
    print(f"{len(graders)} cases grade a library sentence; {len(survivors)} survived the wreck")
    if survivors:
        for name in survivors:
            print(f"  [SURVIVED] {name} -- passes with the library's message destroyed, so it grades nothing of ours")
        return 1
    print("every case that grades a library sentence fails when that sentence is destroyed")
    return 0

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cc", default="g++")
    ap.add_argument("--std", default="c++23")
    ap.add_argument("--include", default=str(HERE.parent.parent / "include"))
    ap.add_argument("--jobs", type=int, default=0,
                    help="number of cases to compile concurrently (0 = one per CPU; 1 = serial)")
    ap.add_argument("--json")
    ap.add_argument("--label", default="run")
    ap.add_argument("--compare", nargs=2, metavar=("A.json", "B.json"))
    ap.add_argument("--emit-doc", action="store_true",
                    help="capture verbatim diagnostics for the DOC_CASES into --out-dir")
    ap.add_argument("--out-dir", default=str(HERE.parent.parent / "docs" / "diagnostics"))
    ap.add_argument("--compiler-label", help="human label shown in the captured snippet, e.g. 'GCC 13'")
    ap.add_argument("--compiler-slug", help="filename slug for the compiler, e.g. 'gcc13'")
    ap.add_argument("--check-doc", action="store_true",
                    help="re-emit the DOC_CASES diagnostics and diff them against the committed pages; fails on drift")
    ap.add_argument("--mutate", action="store_true",
                    help="wreck every library diagnostic string in a COPY of the headers and require that every case "
                         "grading a library sentence FAILS; proves the suite is not self-grading")
    args = ap.parse_args()
    if args.compare:
        sys.exit(do_compare(*args.compare))
    if args.check_doc:
        sys.exit(do_check_doc(args))
    if args.mutate:
        sys.exit(do_mutate(args))
    if args.emit_doc:
        sys.exit(do_emit_doc(args))
    sys.exit(do_run(args))

if __name__ == "__main__":
    main()
