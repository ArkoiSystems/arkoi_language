#include <fstream>
#include <string>

#include "gtest/gtest.h"

#include "utils/driver.hpp"
#include "utils/utils.hpp"

using namespace arkoi;

static const std::string PROGRAM_FILES = TEST_PATH "/e2e/programs/";

TEST(EndToEnd, AllPrograms) {
    for (const auto &entry: std::filesystem::directory_iterator(PROGRAM_FILES)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".ark") continue;

        const auto file_path = entry.path().string();
        const auto base_path = get_base_path(file_path);

        const auto source = std::make_shared<pretty_diagnostics::FileSource>(file_path);

        const auto asm_path = base_path + ".s";
        { // Compile the source to assembly
            std::ofstream asm_ostream(asm_path);

            const int32_t compiler_exit = driver::compile(source, nullptr, nullptr, &asm_ostream);
            if (compiler_exit != 0) std::remove(asm_path.c_str());

            ASSERT_EQ(0, compiler_exit);
        }

        const auto obj_path = base_path + ".o";
        { // Assemble the compiled source
            std::ofstream obj_ostream(obj_path);

            const auto assemble_exit = driver::assemble(asm_path, obj_ostream);
            if (assemble_exit != 0) std::remove(obj_path.c_str());

            ASSERT_EQ(0, assemble_exit);
        }

        const auto bin_path = base_path + ".out";
        { // Link the assembled source
            std::ofstream bin_ostream(bin_path);

            const auto link_exit = driver::link({obj_path}, bin_ostream);
            if (link_exit != 0) std::remove(bin_path.c_str());

            ASSERT_EQ(0, link_exit);
        }

        // Run the resulting binary
        EXPECT_EQ(0, driver::run_binary(bin_path));

        // Clean up the temporary file artifacts
        std::remove(asm_path.c_str());
        std::remove(obj_path.c_str());
        std::remove(bin_path.c_str());
    }
}