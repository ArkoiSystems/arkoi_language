from pygments.token import Comment, Keyword, Name, Number, Operator, String, Text
from pygments.lexer import RegexLexer

class ArkoiLexer(RegexLexer):
    name      = "Arkoi"
    aliases   = ["arkoi"]
    filenames = ["*.ark", "*.arkoi"]
    mimetypes = ["text/x-arkoi"]

    tokens = {
        "root": [
            (r"\s+", Text),
            (r"#.*?$", Comment.Single),
            (r'"([^"\\]|\\.)*"', String.Double),
            (r"'([^'\\]|\\.)'", String.Char),
            (r"\b(true|false)\b", Keyword.Constant),
            (r"\b(fun|if|else|while|return)\b", Keyword),
            (r"@[a-zA-Z_][a-zA-Z0-9_]*", Name.Class),
            (r"\b(?:s|u)(?:8|16|32|64|size)\b|\bf(?:32|64)\b|\bbool\b", Name.Builtin),
            (r"\b[0-9]+\.[0-9]+\b", Number.Float),
            (r"\b[0-9]+\b", Number.Integer),
            (r"==|!=|<=|>=|&&|\|\||[+\-*/<>:=,(){}\[\]]", Operator),
            (r"[A-Za-z_][A-Za-z0-9_]*", Name),
            (r".", Text),
        ]
    }
