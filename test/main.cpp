#include <gtest/gtest.h>

#include "snapshot/snapshot.hpp"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    for (int index = 0; index < argc; ++index) {
        if (std::string(argv[index]) == "--update-snapshots") {
            UPDATE_SNAPSHOTS = true;
        }
    }

    return RUN_ALL_TESTS();
}
