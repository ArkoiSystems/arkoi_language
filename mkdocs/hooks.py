from pygments.lexers import _mapping

from arkoi_lexer import ArkoiLexer

_mapping.LEXERS[ArkoiLexer.__name__] = (
    "arkoi_lexer",
    ArkoiLexer.name,
    tuple(ArkoiLexer.aliases),
    tuple(ArkoiLexer.filenames),
    tuple(ArkoiLexer.mimetypes),
)
