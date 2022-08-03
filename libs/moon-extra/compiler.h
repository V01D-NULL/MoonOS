#ifndef COMPILER_UTIL
#define COMPILER_UTIL

#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a##b
#define UNIQUE_NAME(base) CONCAT(base, __COUNTER__) // Generate unique variable names (ex: foo0)
#define ARRAY_SIZE(array) sizeof(array) / sizeof(array[0])

#endif // COMPILER_UTIL