#pragma once
#include <iostream>
#define LOG_HELPER(FILE, LINE, DATA) std::cout << FILE << LINE << #DATA ": " << DATA << std::endl;
#define LOG_DETAIL(DATA) LOG_HELPER(__FILE__ ": ", __LINE__" ", DATA)

#define LOG(DATA) LOG_HELPER("", "", DATA)

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual)                           \
    do {                                                                   \
        ++test_count;                                                      \
        if (equality) {                                                    \
            ++test_pass;                                                   \
        } else {                                                           \
            std::cerr << __FILE__ <<" : " << __LINE__ << ": "<< "expect: " \
            << expect << " actual: "<< actual << std::endl;                \
            main_ret = 1;                                                  \
        }                                                                  \
    } while(0)

#define EXPECT_EQ(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual)

#define FOR_EACH(container, operation) 									   \
    do {                                                                   \
        for (auto& e : container) {										   \
        	operation(e);												   \
        }                                                                  \
    } while(0)

