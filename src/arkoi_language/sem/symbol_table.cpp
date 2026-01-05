#include "arkoi_language/sem/symbol_table.hpp"

using namespace pretty_diagnostics;
using namespace arkoi::sem;
using namespace arkoi;

IdentifierAlreadyTaken::IdentifierAlreadyTaken(const front::Token& first, const front::Token& second) :
    SemanticError(
        Report::Builder()
       .severity(Severity::Error)
       .message("The identifier '" + first.span().substr() + "' is already taken")
       .code("E3001")
       .label("This is the first definition", first.span())
       .label("And this is the redefinition", second.span())
       .build()
    ) { }

IdentifierNotFound::IdentifierNotFound(const front::Token& name) :
    SemanticError(
        Report::Builder()
       .severity(Severity::Error)
       .message("The identifier '" + name.span().substr() +  "' was not found")
       .code("E3002")
       .label("There was no definition for this identifier", name.span())
       .build()
    ) { }
