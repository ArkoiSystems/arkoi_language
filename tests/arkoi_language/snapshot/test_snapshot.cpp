#include <filesystem>
#include <fstream>

#include "gtest/gtest.h"

#include "../../snapshot/snapshot.hpp"

#include "arkoi_language/front/scanner.hpp"
#include "arkoi_language/front/parser.hpp"
#include "arkoi_language/ast/ast_printer.hpp"

static const std::string SCANNER_FILES = TEST_PATH "/arkoi_language/snapshot/scanner/";

TEST(Snapshot, Scanner) {
    for (const auto& entry : std::filesystem::directory_iterator(SCANNER_FILES)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".ark") continue;

        const auto file_name = entry.path().filename().stem().string();
        const auto file_path = entry.path().string();

        const auto source = std::make_shared<pretty_diagnostics::FileSource>(file_path);
        auto scanner = arkoi::front::Scanner(source);

        std::stringstream output;
        for (const auto& token : scanner.tokenize()) {
            output << token << "\n";
        }

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
        auto scanner = arkoi::front::Scanner(source);
        auto parser = arkoi::front::Parser(scanner.tokenize());
        auto program = parser.parse_program();

        auto ast_printer = arkoi::ast::ASTPrinter::print(program);

        const auto snapshot_path = entry.path().parent_path() / (file_name + ".snapshot");
        EXPECT_SNAPSHOT_EQ(file_name, snapshot_path.string(), ast_printer.str());
    }
}
