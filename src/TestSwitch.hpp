// Implement failed

// usage:
// #define TEST_NAME
// #define HEADER
#define CONCAT_HELPER(A, B) A##B
#define CONCAT(A, B) CONCAT_HELPER(A, B)
#define STRINGIFY_HELPER(X) #X
#define STRINGIFY(X) STRINGIFY_HELPER(X)

#if CONCAT(TEST_NAME, _TEST_BUTTON)
#include STRINGIFY(HEADER)
// https://stackoverflow.com/questions/47557125/dynamically-prefix-macro-names-with-a-variadic-macro
// Right. You need a "pre-pre-processor". – WhatsUp from StackOverflow
// My question is similar to up, I want dynamic define macro which constructs from defined macro.
#define CONCAT(TEST_NAME, _TEST)(FUNC) FUNC
#else
#define CONCAT(TEST_NAME, _TEST)(FUNC)
#endif

#undef STRINGIFY
#undef STRINGIFY_HELPER
#undef CONCAT
#undef CONCAT_HELPER
// #undef TEST_NAME
// #undef HEADER
