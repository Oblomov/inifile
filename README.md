# inifile

A small C++ project to parse `.ini` files. Supports git-style ‘named’
sections `[stuff "name"]` with keys accessible through
`stuff.name.key` and fallbacks to `stuff.*.key` (i.e. define a
`[stuff "*"]` to provide fallbacks common to all `stuff` subsections).

