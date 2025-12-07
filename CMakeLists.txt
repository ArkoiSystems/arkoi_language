cmake_minimum_required(VERSION 3.29.6)

project(arkoi_language VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wswitch -fsanitize=address,undefined,leak")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

# =====================
# Library
# =====================

include_directories(include)

add_library(${PROJECT_NAME}_lib
    src/front/token.cpp
    src/front/scanner.cpp
    src/front/parser.cpp
    src/ast/ast_printer.cpp
    src/sem/name_resolver.tpp
    src/sem/name_resolver.cpp
    src/sem/symbol_table.tpp
    src/sem/symbol.cpp
    src/sem/type_resolver.cpp
    src/sem/type.cpp
    src/il/instruction.cpp
    src/il/generator.cpp
    src/il/il_printer.cpp
    src/il/cfg_printer.cpp
    src/il/dataflow.tpp
    src/il/analyses.cpp
    src/il/operand.cpp
    src/il/cfg.cpp
    src/opt/pass.cpp
    src/opt/pass.tpp
    src/opt/constant_folding.cpp
    src/opt/constant_propagation.cpp
    src/opt/dead_code_elimination.cpp
    src/opt/simplify_cfg.cpp
    src/x86_64/assembly.cpp
    src/x86_64/register_allocation.cpp
    src/x86_64/mapper.cpp
    src/x86_64/operand.cpp
    src/x86_64/generator.cpp
    src/utils/interference_graph.tpp
    src/utils/ordered_set.tpp
    src/utils/driver.cpp
    src/utils/utils.cpp
    src/utils/size.cpp
)

target_link_libraries(${PROJECT_NAME}_lib)

# =====================
# Executable
# =====================

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")
add_compile_definitions(PROJECT_VERSION="${CMAKE_PROJECT_VERSION}")

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

# =====================
# Tests
# =====================

# Enable testing framework
enable_testing()
# Adds the test directory to the build
add_subdirectory(test)