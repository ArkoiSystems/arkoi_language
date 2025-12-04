// End-to-end test using the reusable driver API
#include <fstream>
#include <string>

#include "gtest/gtest.h"

#include "utils/driver.hpp"
#include "utils/utils.hpp"

using namespace arkoi;

#ifndef ROOT_PATH
#error "ROOT_PATH must be defined by CMake for tests"
#endif

TEST(E2E, HelloWorldCompilesAndRuns) {
    const auto hello_path = std::string(ROOT_PATH) + "/example/hello_world/hello_world.ark";

    const auto base_path = get_base_path(hello_path);
    const auto source = read_file(hello_path);

    const auto asm_path = base_path + ".s";
    { // Compile the source to assembly
        std::ofstream asm_ostream(asm_path);
        ASSERT_EQ(0, driver::compile(source, nullptr, nullptr, &asm_ostream));
    }

    const auto obj_path = base_path + ".o";
    { // Assemble the compiled source
        std::ofstream obj_ostream(obj_path);
        ASSERT_EQ(0, driver::assemble(asm_path, obj_ostream));
    }

    const auto bin_path = base_path + ".out";
    { // Link the assembled source
        std::ofstream bin_ostream(bin_path);
        ASSERT_EQ(0, driver::link({obj_path}, bin_ostream));
    }

    // Run the resulting binary
    EXPECT_EQ(0, driver::run_binary(bin_path));

    // Clean up the temporary file artifacts
    std::remove(asm_path.c_str());
    std::remove(obj_path.c_str());
    std::remove(bin_path.c_str());
}