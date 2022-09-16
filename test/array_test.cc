#include "gtest/gtest.h"

#ifndef TEST_CASE_DIR
#error "Missing TEST_CASE_DIR"
#endif

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
