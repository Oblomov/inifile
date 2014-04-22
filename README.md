# inifile

`inifile` provides a C++ interface for parsing (reading) and dumping
(writing) `.ini` files.

Read/write support is designed to offer a high degree of round-trip
robustness, modulo white-space cleanup. The code tries to remember the
order of the sections and of their values, and preserve comments (as
well as empty lines), under the assumption that comments refer to the
following section or key.

The library also supports git-style ‘named’ sections `[stuff "name"]`,
with keys accessible through `stuff.name.key` and fallbacks to
`stuff.*.key` (i.e. define a `[stuff "*"]` to provide fallbacks common
to all `stuff` subsections).

