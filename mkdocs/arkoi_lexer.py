import re

from pygments.lexer import RegexLexer, words
from pygments.token import (
    Comment,
    Keyword,
    Name,
    Number,
    Operator,
    Punctuation,
    String,
    Text,
)


KEYWORDS = (
    "and",
    "as",
    "break",
    "const",
    "continue",
    "data",
    "elif",
    "else",
    "enum",
    "export",
    "extends",
    "fail",
    "failure",
    "for",
    "fun",
    "handle",
    "if",
    "implements",
    "import",
    "in",
    "interface",
    "loop",
    "module",
    "mut",
    "named",
    "not",
    "or",
    "own",
    "pass",
    "pub",
    "resource",
    "return",
    "type",
    "union",
    "unsafe",
    "while",
    "yield",
)

CONSTANTS = ("false", "none", "null", "true")

ARKOI_SCALAR_TYPES = (
    "bool",
    "char",
    "f32",
    "f64",
    "s8",
    "s16",
    "s32",
    "s64",
    "ssize",
    "string",
    "string_view",
    "u8",
    "u16",
    "u32",
    "u64",
    "usize",
)

C_SCALAR_TYPES = (
    "bool",
    "char",
    "double",
    "float",
    "int",
    "long",
    "long_long",
    "ptrdiff",
    "short",
    "signed_char",
    "size",
    "uint",
    "unsigned_char",
    "unsigned_int",
    "unsigned_long",
    "unsigned_long_long",
    "unsigned_short",
    "void",
)

TARGET_DEFINED_TYPES = (
    "target_defined_boolean_integer_type",
    "target_defined_integer_type",
    "target_defined_signed_integer_type",
    "target_defined_unsigned_integer_type",
)

# Keep longer operators first so that, for example, `<<!=` is not split into
# `<<` and `!=`.
OPERATORS = (
    "<<!=",
    ">>!=",
    "+!=",
    "-!=",
    "*!=",
    "/!=",
    "%!=",
    "+%=",
    "-%=",
    "*%=",
    "<<!",
    ">>!",
    "<<=",
    ">>=",
    "...",
    "+!",
    "-!",
    "*!",
    "/!",
    "%!",
    "+%",
    "-%",
    "*%",
    "+=",
    "-=",
    "*=",
    "/=",
    "%=",
    "&=",
    "|=",
    "^=",
    "==",
    "!=",
    "<=",
    ">=",
    "&&",
    "||",
    "|>",
    "<<",
    ">>",
    "?.",
    "??",
    "..",
    "+",
    "-",
    "*",
    "/",
    "%",
    "&",
    "|",
    "^",
    "~",
    "<",
    ">",
    "=",
    "!",
    "?",
    "@",
)
OPERATOR_RE = "|".join(re.escape(operator) for operator in OPERATORS)

DECIMAL_DIGITS = r"[0-9](?:_?[0-9])*"
HEX_DIGITS = r"[0-9a-fA-F](?:_?[0-9a-fA-F])*"
BINARY_DIGITS = r"[01](?:_?[01])*"
OCTAL_DIGITS = r"[0-7](?:_?[0-7])*"
HEX_SIGNIFICAND = (
    rf"(?:{HEX_DIGITS}(?:\.(?:{HEX_DIGITS})?)?|\.(?:{HEX_DIGITS}))"
)

FLOAT_RE = (
    rf"(?<![\w.])(?:"
    rf"0[xX]{HEX_SIGNIFICAND}[pP][+-]?{DECIMAL_DIGITS}"
    rf"|{DECIMAL_DIGITS}\.{DECIMAL_DIGITS}(?:[eE][+-]?{DECIMAL_DIGITS})?"
    rf"|{DECIMAL_DIGITS}[eE][+-]?{DECIMAL_DIGITS}"
    rf")(?![\w.])"
)


class ArkoiLexer(RegexLexer):
    name = "Arkoi"
    aliases = ["arkoi"]
    filenames = ["*.ark", "*.arkoi"]
    mimetypes = ["text/x-arkoi"]

    tokens = {
        "root": [
            (r"\s+", Text),
            (r"#.*?$", Comment.Single),
            (r'"(?:[^"\\\n]|\\.)*"', String.Double),
            (
                r"'(?:[^'\\\n]|\\(?:x[0-9a-fA-F]{2}|u\{[0-9a-fA-F]+\}|.))'",
                String.Char,
            ),
            (words(CONSTANTS, prefix=r"\b", suffix=r"\b"), Keyword.Constant),
            (words(KEYWORDS, prefix=r"\b", suffix=r"\b"), Keyword),
            (
                words(TARGET_DEFINED_TYPES, prefix=r"\b", suffix=r"\b"),
                Name.Builtin.Pseudo,
            ),
            (r"\bc\.u?int(?:8|16|32|64)_t\b", Name.Builtin),
            (
                words(C_SCALAR_TYPES, prefix=r"\bc\.", suffix=r"\b"),
                Name.Builtin,
            ),
            (
                words(ARKOI_SCALAR_TYPES, prefix=r"\b", suffix=r"\b"),
                Name.Builtin,
            ),
            (r"\bSelf\b", Name.Builtin.Pseudo),
            (FLOAT_RE, Number.Float),
            (
                rf"(?<!\w)0[xX]{HEX_DIGITS}(?!\w)",
                Number.Hex,
            ),
            (
                rf"(?<!\w)0[bB]{BINARY_DIGITS}(?!\w)",
                Number.Bin,
            ),
            (
                rf"(?<!\w)0[oO]{OCTAL_DIGITS}(?!\w)",
                Number.Oct,
            ),
            (
                rf"(?<![\w.]){DECIMAL_DIGITS}(?![\w.])",
                Number.Integer,
            ),
            (OPERATOR_RE, Operator),
            (r"[()[\]{},:;.\\]", Punctuation),
            (r"(?<![A-Za-z0-9_])_(?![A-Za-z0-9_])", Name.Builtin.Pseudo),
            (r"[A-Za-z_][A-Za-z0-9_]*", Name),
            (r".", Text),
        ]
    }
