#!/usr/bin/env bash
# ODR-AUDIT reproduction runner.
#
# Each repro is a set {NAME_shared.h, NAME_a.cpp, NAME_b.cpp, NAME_main.cpp} where a.cpp
# includes the dimension header that registers the named class and b.cpp does not, and
# all three .cpp share ONE weak/inline entity whose result type is rewrap_to_named_t<...>.
# We compile the three objects once, then LINK them in BOTH orders (a-before-b and
# b-before-a). The ODR violation makes the linker keep ONE weak definition per order, so
# a divergent repro prints two DIFFERENT lines -> "DIVERGES". A stable one prints the same
# line both orders -> "STABLE".
#
# Special repros:
#   s04 (std::formatter): a per-TU COMPILE SPLIT, not a link-order flip -- handled separately.
#   s06 (sizeof probe):   a single-TU probe -- handled separately.
set -u
INC="-I /e/workspace/units/include"
STD="-std=c++23"
CXX="${CXX:-g++}"
PASS=0; FAIL=0

run_linkflip() {
  local name="$1"
  [ -f "${name}_a.cpp" ] || { echo "skip ${name}: no sources"; return; }
  $CXX $STD $INC -c "${name}_a.cpp" -o "/tmp/${name}_a.o" 2>/tmp/${name}.err || { echo "${name}: a.cpp FAILED to compile"; cat /tmp/${name}.err; FAIL=$((FAIL+1)); return; }
  $CXX $STD $INC -c "${name}_b.cpp" -o "/tmp/${name}_b.o" 2>>/tmp/${name}.err || { echo "${name}: b.cpp FAILED to compile"; cat /tmp/${name}.err; FAIL=$((FAIL+1)); return; }
  $CXX $STD $INC -c "${name}_main.cpp" -o "/tmp/${name}_main.o" 2>>/tmp/${name}.err || { echo "${name}: main.cpp FAILED"; FAIL=$((FAIL+1)); return; }
  $CXX "/tmp/${name}_a.o" "/tmp/${name}_b.o" "/tmp/${name}_main.o" -o "/tmp/${name}_ab" 2>>/tmp/${name}.err
  $CXX "/tmp/${name}_b.o" "/tmp/${name}_a.o" "/tmp/${name}_main.o" -o "/tmp/${name}_ba" 2>>/tmp/${name}.err
  local ab ba
  ab="$("/tmp/${name}_ab")"
  ba="$("/tmp/${name}_ba")"
  if [ "$ab" != "$ba" ]; then
    echo "==> ${name}: DIVERGES  [a b]='${ab}'  [b a]='${ba}'"
    PASS=$((PASS+1))
  else
    echo "    ${name}: STABLE    (both orders='${ab}')"
  fi
  rm -f "/tmp/${name}_a.o" "/tmp/${name}_b.o" "/tmp/${name}_main.o" "/tmp/${name}_ab" "/tmp/${name}_ba" "/tmp/${name}.err"
}

echo "COMPILER: $($CXX --version | head -1)"
echo "--- link-order-flip repros (a divergent one prints two different lines) ---"
# discover every NAME_shared.h that has an a.cpp/b.cpp/main.cpp set
for shared in *_shared.h; do
  name="${shared%_shared.h}"
  [ -f "${name}_a.cpp" ] && run_linkflip "$name"
done

echo "--- s04 (std::formatter): per-TU COMPILE SPLIT ---"
if [ -f s04_named.cpp ]; then
  if $CXX $STD $INC -c s04_named.cpp -o /tmp/s04n.o 2>/dev/null; then n=OK; else n=FAIL; fi
  if $CXX $STD $INC -c s04_plain.cpp -o /tmp/s04p.o 2>/dev/null; then p=COMPILES; else p=COMPILE-ERROR; fi
  rm -f /tmp/s04n.o /tmp/s04p.o
  if [ "$n" = OK ] && [ "$p" = COMPILE-ERROR ]; then
    echo "==> s04: DIVERGES (named TU compiles=$n ; plain TU=$p) -- per-TU compile split"
    PASS=$((PASS+1))
  else
    echo "    s04: UNEXPECTED (named=$n plain=$p)"
  fi
fi

echo "--- s06 (sizeof/alignof/trivial-copy probe) ---"
if [ -f s06_probe.cpp ]; then
  $CXX $STD $INC s06_probe.cpp -o /tmp/s06 2>/dev/null && /tmp/s06 | sed 's/^/    /'
  rm -f /tmp/s06
fi

echo "--- corr_dispatch (overload dispatch -> NUMERIC value flip; non-standard header names) ---"
if [ -f corr_dispatch_a.cpp ]; then
  $CXX $STD $INC -c corr_dispatch_a.cpp -o /tmp/cd_a.o 2>/dev/null
  $CXX $STD $INC -c corr_dispatch_b.cpp -o /tmp/cd_b.o 2>/dev/null
  $CXX $STD $INC -c corr_dispatch_main.cpp -o /tmp/cd_m.o 2>/dev/null
  $CXX /tmp/cd_m.o /tmp/cd_a.o /tmp/cd_b.o -o /tmp/cd_ab 2>/dev/null
  $CXX /tmp/cd_m.o /tmp/cd_b.o /tmp/cd_a.o -o /tmp/cd_ba 2>/dev/null
  ab="$(/tmp/cd_ab)"; ba="$(/tmp/cd_ba)"
  if [ "$ab" != "$ba" ]; then echo "==> corr_dispatch: DIVERGES  [a b]='$ab'  [b a]='$ba'"; PASS=$((PASS+1));
  else echo "    corr_dispatch: STABLE (both='$ab')"; fi
  rm -f /tmp/cd_a.o /tmp/cd_b.o /tmp/cd_m.o /tmp/cd_ab /tmp/cd_ba
fi

echo "--- summary: ${PASS} divergent repro(s) reproduced ---"
