### --- Project Initialization and Configuration --- ###
cmake_minimum_required(VERSION 4.2)   # Require CMake 4.2 or later
project(arkoi_language VERSION 0.1.0)

# Use C++23 and enforce it
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to enable/disable testing:
option(BUILD_TESTING "Enable testing" ON)
# Option to enable/disable executable:
option(BUILD_EXECUTABLE "Create executable" ON)
# Option to enable/disable installation:
option(MAKE_INSTALLABLE "Make the library installable" ON)
# Option to control static vs shared library
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Add some flags to ensure consistency
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic -Wswitch -fsanitize=address,undefined,leak")

# Find pretty diagnostics on the system
find_package(pretty_diagnostics CONFIG REQUIRED)

### --- CMake Modules for Installation and Fetching --- ###
include(CMakePackageConfigHelpers) # Helper for creating config files for package managers
include(GNUInstallDirs)            # Define standard install directories (lib, include, etc.)
include(FetchContent)              # For downloading and using external dependencies


### --- Define Library Target --- ##

# Create the main library from source file
add_library(${PROJECT_NAME}
        src/arkoi_language/front/token.cpp
        src/arkoi_language/front/scanner.cpp
        src/arkoi_language/front/parser.cpp
        src/arkoi_language/ast/ast_printer.cpp
        src/arkoi_language/sem/name_resolver.tpp
        src/arkoi_language/sem/name_resolver.cpp
        src/arkoi_language/sem/symbol_table.tpp
        src/arkoi_language/sem/symbol.cpp
        src/arkoi_language/sem/type_resolver.cpp
        src/arkoi_language/sem/type.cpp
        src/arkoi_language/il/instruction.cpp
        src/arkoi_language/il/generator.cpp
        src/arkoi_language/il/il_printer.cpp
        src/arkoi_language/il/cfg_printer.cpp
        src/arkoi_language/il/dataflow.tpp
        src/arkoi_language/il/analyses.cpp
        src/arkoi_language/il/operand.cpp
        src/arkoi_language/il/cfg.cpp
        src/arkoi_language/opt/pass.cpp
        src/arkoi_language/opt/pass.tpp
        src/arkoi_language/opt/constant_folding.cpp
        src/arkoi_language/opt/constant_propagation.cpp
        src/arkoi_language/opt/dead_code_elimination.cpp
        src/arkoi_language/opt/simplify_cfg.cpp
        src/arkoi_language/x86_64/assembly.cpp
        src/arkoi_language/x86_64/register_allocation.cpp
        src/arkoi_language/x86_64/mapper.cpp
        src/arkoi_language/x86_64/operand.cpp
        src/arkoi_language/x86_64/generator.cpp
        src/arkoi_language/utils/interference_graph.tpp
        src/arkoi_language/utils/ordered_set.tpp
        src/arkoi_language/utils/driver.cpp
        src/arkoi_language/utils/utils.cpp
        src/arkoi_language/utils/size.cpp
)
# Create an alias for the library
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC pretty_diagnostics::pretty_diagnostics)

# Setup include directories for consumers and installers
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> # For build tree
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)      # For install tree

# Define public headers
set(PUBLIC_HEADERS
        include/arkoi_language/ast/ast_printer.hpp
        include/arkoi_language/ast/nodes.hpp
        include/arkoi_language/ast/visitor.hpp
        include/arkoi_language/front/parser.hpp
        include/arkoi_language/front/scanner.hpp
        include/arkoi_language/front/token.hpp
        include/arkoi_language/il/analyses.hpp
        include/arkoi_language/il/cfg.hpp
        include/arkoi_language/il/cfg_printer.hpp
        include/arkoi_language/il/dataflow.hpp
        include/arkoi_language/il/generator.hpp
        include/arkoi_language/il/il_printer.hpp
        include/arkoi_language/il/instruction.hpp
        include/arkoi_language/il/operand.hpp
        include/arkoi_language/il/visitor.hpp
        include/arkoi_language/opt/constant_folding.hpp
        include/arkoi_language/opt/constant_propagation.hpp
        include/arkoi_language/opt/dead_code_elimination.hpp
        include/arkoi_language/opt/pass.hpp
        include/arkoi_language/opt/simplify_cfg.hpp
        include/arkoi_language/sem/name_resolver.hpp
        include/arkoi_language/sem/symbol.hpp
        include/arkoi_language/sem/symbol_table.hpp
        include/arkoi_language/sem/type.hpp
        include/arkoi_language/sem/type_resolver.hpp
        include/arkoi_language/utils/driver.hpp
        include/arkoi_language/utils/interference_graph.hpp
        include/arkoi_language/utils/ordered_set.hpp
        include/arkoi_language/utils/size.hpp
        include/arkoi_language/utils/utils.hpp
        include/arkoi_language/x86_64/assembly.hpp
        include/arkoi_language/x86_64/generator.hpp
        include/arkoi_language/x86_64/mapper.hpp
        include/arkoi_language/x86_64/operand.hpp
        include/arkoi_language/x86_64/register_allocation.hpp)

# Set the properties of the resulting library
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        PUBLIC_HEADER "${PUBLIC_HEADERS}")


### --- Define Executable Target --- ###

if(BUILD_EXECUTABLE)
    add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}")
    add_compile_definitions(PROJECT_VERSION="${CMAKE_PROJECT_VERSION}")

    # Create the main executable from the main source file
    add_executable(${PROJECT_NAME}_app src/main.cpp)
    # Link the library with the application
    target_link_libraries(${PROJECT_NAME}_app PRIVATE ${PROJECT_NAME} pretty_diagnostics::pretty_diagnostics)
    # Includes the headers to the application
    target_include_directories(${PROJECT_NAME}_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()


### --- Install Rules --- ###

if(MAKE_INSTALLABLE)
    # Install the library and its headers
    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

    # Install the target export file (for find_package)
    install(EXPORT ${PROJECT_NAME}Targets
            FILE ${PROJECT_NAME}Targets.cmake
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

    # Generate version file for package configuration
    write_basic_package_version_file(
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY AnyNewerVersion)

    # Creates a config file for package configuration
    configure_package_config_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

    # Install the generated config and config version file
    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
endif ()


### --- Unit Testing Setup --- ###

if(BUILD_TESTING)
    # Enable testing framework
    enable_testing()
    # Adds the test directory to the build
    add_subdirectory(tests)
endif()