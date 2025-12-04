#include <filesystem>
#include <fstream>

#include "gtest/gtest.h"

#include "front/scanner.hpp"
#include "utils/utils.hpp"
#include "snapshot.hpp"

static const std::string SCANNER_FILES = TEST_PATH "/snapshot/scanner/";

TEST(Snapshot, Scanner) {
    for (const auto &entry: std::filesystem::directory_iterator(SCANNER_FILES)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".ark") continue;

        const auto file_name = entry.path().filename().stem().string();
        const auto snapshot_file = SCANNER_FILES + file_name + ".snapshot";

        SnapshotTester tester(snapshot_file);

        const auto source = read_file(entry.path().string());
        auto scanner = arkoi::front::Scanner(source);

        std::stringstream output;
        for (const auto &token: scanner.tokenize()) {
            output << token << "\n";
        }

        ASSERT_TRUE(tester.compare(output.str()));
    }
}