#pragma once

// include the test framework
#if ENABLE_GTEST

    #include "gtest/gtest.h"

#elif ENABLE_CATCH2TEST

    # include <catch2/catch_test_macros.hpp>
    # include <catch2/reporters/catch_reporter_event_listener.hpp>
    # include <catch2/reporters/catch_reporter_registrars.hpp>

    // use gtest macros in Catch2
    // #define TEST(A, B) TEST_CASE(#B, "[all]["#A"]")
    // #define EXPECT_EQ(A, B) REQUIRE((A) == (B))
    // #define EXPECT_NE(A, B) REQUIRE((A) != (B))
    // #define BENCHMARK(X) BENCHMARK(#X)

#endif
