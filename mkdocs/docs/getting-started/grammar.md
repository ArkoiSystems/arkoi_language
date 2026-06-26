# Grammar

This page describes the structural grammar of Arkoi. It shows the core forms used by the language and the way those forms fit together.

## Program structure

```ebnf
program        = { function_definition } ;
function_definition = "fun" identifier "(" [ parameter_list ] ")" return_type ":" block ;
parameter_list = parameter { "," parameter } ;
parameter      = identifier type_annotation ;
type_annotation = "@" type_name ;
block          = { statement } ;
```

## Statements

```ebnf
statement = variable_declaration
          | assignment
          | if_statement
          | while_statement
          | return_statement
          | expression ;
```

## Expressions

```ebnf
expression = logical_or ;
logical_or  = logical_and { "||" logical_and } ;
logical_and = equality { "&&" equality } ;
equality    = comparison { ("==" | "!=") comparison } ;
comparison  = term { ("<" | "<=" | ">" | ">=") term } ;
term        = factor { ("+" | "-") factor } ;
factor      = unary { ("*" | "/") unary } ;
unary       = primary ;
primary     = literal | identifier | function_call | "(" expression ")" ;
```

## Notes

- A function definition is the top-level unit of compilation.
- Blocks group statements under functions and control-flow constructs.
- Expressions build from calls, literals, identifiers, and operators.