#include <filesystem>
#include <fstream>

#include "gtest/gtest.h"

#include "../../snapshot/snapshot.hpp"

#include "arkoi_language/ast/ast_printer.hpp"
#include "arkoi_language/front/parser.hpp"
#include "arkoi_language/front/scanner.hpp"

static const std::string SCANNER_FILES = TEST_PATH "/arkoi_language/snapshot/scanner/";

TEST(Snapshot, Scanner) {
    for (const auto& entry : std::filesystem::directory_iterator(SCANNER_FILES)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".ark") continue;

        const auto file_name = entry.path().filename().stem().string();
        const auto file_path = entry.path().string();

        const auto source = std::make_shared<pretty_diagnostics::FileSource>(file_path);
        source->set_working_path(TEST_PATH);

        auto diagnostics = arkoi::utils::Diagnostics();
        auto scanner = arkoi::front::Scanner(source, diagnostics);

        std::stringstream output;
        for (const auto& token : scanner.tokenize()) {
            output << token << "\n";
        }

        EXPECT_FALSE(diagnostics.has_errors());

        const auto snapshot_path = entry.path().parent_path() / (file_name + ".snapshot");
        EXPECT_SNAPSHOT_EQ(file_name, snapshot_path.string(), output.str());
    }
}

static const std::string PARSER_FILES = TEST_PATH "/arkoi_language/snapshot/parser/";

TEST(Snapshot, Parser) {
    for (const auto& entry : std::filesystem::directory_iterator(PARSER_FILES)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".ark") continue;

        const auto file_name = entry.path().filename().stem().string();
        const auto file_path = entry.path().string();

        const auto source = std::make_shared<pretty_diagnostics::FileSource>(file_path);
        source->set_working_path(TEST_PATH);

        auto diagnostics = arkoi::utils::Diagnostics();
        auto scanner = arkoi::front::Scanner(source, diagnostics);
        auto parser = arkoi::front::Parser(source, scanner.tokenize(), diagnostics);
        auto program = parser.parse_program();

        EXPECT_FALSE(diagnostics.has_errors());

        auto ast_printer = arkoi::ast::ASTPrinter::print(program);

        const auto snapshot_path = entry.path().parent_path() / (file_name + ".snapshot");
        EXPECT_SNAPSHOT_EQ(file_name, snapshot_path.string(), ast_printer.str());
    }
}
