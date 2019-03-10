#pragma once
#include <iostream>
#define LOG_HELPER(FILE, LINE, DATA) std::cout << FILE << LINE << #DATA ": " << DATA << std::endl;
#define LOG_DETAIL(DATA) LOG_HELPER(__FILE__ ": ", __LINE__" ", DATA)

#define LOG(DATA) LOG_HELPER("", "", DATA)
