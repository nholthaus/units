#!/usr/bin/env bash
# Run the CI gates locally with the SAME compiler + flags CI uses, so local is never more permissive than CI.
# Mirrors .github/workflows/gcc-13.yaml. Resource-frugal: reuses warm build dirs (ninja rebuilds only what changed)
# and makes the slow (~130s) diagnostic-message harness opt-in.
#
# Usage:
#   scripts/ci_local.sh            fast gates: build + ctest + gen_reference --check + drift check + UBSan
#   scripts/ci_local.sh --full     also run the errorMessages diagnostic harness (~130s; run when diagnostics,
#                                   operators, or serialization change)
set -uo pipefail
cd "$(dirname "$0")/.."
CC=g++-13
FULL=0; [ "${1:-}" = "--full" ] && FULL=1
fail=0
step() { echo; echo "========== $* =========="; }

step "1. Configure (once) + warm build (g++-13, Release)"
[ -f build-ci/build.ninja ] || cmake -B build-ci -G Ninja -DCMAKE_CXX_COMPILER=$CC > /tmp/ci_cfg.log 2>&1 || { echo "CONFIGURE FAILED"; cat /tmp/ci_cfg.log; exit 1; }
cmake --build build-ci --config Release --parallel 2>&1 | tail -2 || { echo "BUILD FAILED"; fail=1; }

step "2. ctest"
( cd build-ci && ctest --output-on-failure --parallel 4 2>&1 | tail -4 ) || fail=1

step "3. gen_reference --check"
python3 docs/reference/gen_reference.py --check 2>&1 | tail -2 || fail=1

step "4. builtin_dimensions drift check"
python3 scripts/check_builtin_dimensions.py 2>&1 | tail -2 || fail=1

step "5. UBSan warm build + ctest (Debug)"
[ -f build-ci-ubsan/build.ninja ] || cmake -B build-ci-ubsan -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=$CC \
  -DCMAKE_CXX_FLAGS="-fsanitize=undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=undefined" > /tmp/ci_ubsan_cfg.log 2>&1 || { echo "UBSAN CONFIGURE FAILED"; cat /tmp/ci_ubsan_cfg.log; exit 1; }
cmake --build build-ci-ubsan --parallel 2>&1 | tail -2 && ( cd build-ci-ubsan && ctest --output-on-failure --parallel 4 2>&1 | tail -3 ) || fail=1

if [ $FULL -eq 1 ]; then
	step "6. errorMessages diagnostic harness (g++-13, c++23) [--full]"
	python3 test/errorMessages/run.py --cc $CC --std c++23 --include include --jobs 4 2>&1 | tail -2 || fail=1
else
	echo; echo "(skipped the ~130s errorMessages harness; pass --full when diagnostics/operators/serialization changed)"
fi

echo; echo "================================================"
[ $fail -eq 0 ] && echo "CI GATES PASS LOCALLY${FULL:+ (full)}" || echo "*** CI GATES FAILED LOCALLY (fix before push) ***"
exit $fail
