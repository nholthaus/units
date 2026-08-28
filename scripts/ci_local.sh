#!/usr/bin/env bash
# Run the CI gates locally with the SAME compiler + flags CI uses, so local is never more permissive than CI.
# Mirrors .github/workflows/gcc-13.yaml. Resource-frugal: reuses warm build dirs (ninja rebuilds only what changed)
# and makes the slow (~130s) diagnostic-message harness opt-in.
#
# Usage:
#   scripts/ci_local.sh            fast gates: build + ctest + gen_reference --check + drift check + UBSan
#   scripts/ci_local.sh --full     also run the errorMessages diagnostic harness (~130s; run when diagnostics,
#                                   operators, or serialization change)
#
# This covers the gcc-13 and clang legs only. MSVC differs on overload resolution, concept evaluation, and template
# instantiation order (and has no __int128), so anything touching operators, constraints, or traits must also pass
# scripts/ci_local_msvc.cmd (and `ci_local_msvc.cmd harness` when diagnostics change).
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

step "6. Doxygen docs (warnings are errors, as on a PR)"
# The docs CI leg sets DOXYGEN_WARN_AS_ERROR=FAIL_ON_WARNINGS for pull requests, and it reads it at CONFIGURE time
# (the value is substituted into the generated Doxyfile), so it must be set on the cmake call, not the build. A
# markdown edit under docs/ can fail this leg while every compiler leg stays green -- that is how a mangled
# docs/explain page reached CI once.
if command -v doxygen > /dev/null; then
	[ -f build-docs/CMakeCache.txt ] || DOXYGEN_WARN_AS_ERROR=FAIL_ON_WARNINGS cmake -B build-docs -DUNITS_BUILD_DOCS=ON \
		-DUNITS_BUILD_TESTS=OFF -DUNITS_BUILD_EXAMPLES=OFF > /tmp/ci_docs_cfg.log 2>&1 || { echo "DOCS CONFIGURE FAILED"; cat /tmp/ci_docs_cfg.log; fail=1; }
	cmake --build build-docs --target doc > /tmp/ci_docs.log 2>&1 && echo "doxygen clean" || { echo "DOXYGEN FAILED:"; grep -E ": (error|warning):" /tmp/ci_docs.log | head -20; fail=1; }
else
	echo "(doxygen not installed; the docs CI leg cannot be mirrored here)"
fi

if [ $FULL -eq 1 ]; then
	step "7. errorMessages diagnostic harness (g++-13, c++23) [--full]"
	python3 test/errorMessages/run.py --cc $CC --std c++23 --include include --jobs 4 2>&1 | tail -2 || fail=1
else
	echo; echo "(skipped the ~130s errorMessages harness; pass --full when diagnostics/operators/serialization changed)"
fi

echo; echo "================================================"
[ $FULL -eq 1 ] && scope=" (full)" || scope=" (fast; errorMessages harness NOT run)"
[ $fail -eq 0 ] && echo "CI GATES PASS LOCALLY${scope}" || echo "*** CI GATES FAILED LOCALLY (fix before push) ***"
exit $fail
