# Error-message cases

Each file in `cases/` is a translation unit that must fail to compile, together with directives naming what its
diagnostic has to say. `run.py` compiles every case and grades the compiler output against those directives.

## Directives

| Directive | Meaning |
| --- | --- |
| `expect: fail` | the case must not compile |
| `expect-match: <text>` | the diagnostic must contain `<text>` |
| `forbid-match: <text>` | the diagnostic must not contain `<text>` |
| `expect-max-lines: <n>` | the diagnostic must be at most `n` lines on gcc |
| `expect-max-lines-clang: <n>` | the same bound for clang |
| `grades: compiler` | the case grades the compiler's own words, not a library sentence |

## Deleted overloads

Some refusals are expressed by deleting the overload rather than by a `static_assert` in its body. A body-fired
assertion resolves the overload, so a `requires`-expression reports the operation as available and generic code with
a SFINAE fallback hard-errors from inside the library instead of taking its fallback. Deletion costs the remedy
sentence, so a case grading a deleted overload grades instead that the diagnostic is short and names both operand
types.

The bound differs by compiler: for a deleted overload gcc prints the declaration and stops, while clang lists every
declined candidate. That is why those cases carry both `expect-max-lines` and a larger `expect-max-lines-clang`.

## Running

```
python3 test/errorMessages/run.py --cc g++-13 --include include
```

`--mutate` wrecks each library diagnostic string in turn and requires every case that grades a library sentence to
fail, which catches a case that only ever matched the compiler's echo of its own source line. `--emit-doc` and
`--check-doc` write and verify the captured pages under `docs/diagnostics/`.
