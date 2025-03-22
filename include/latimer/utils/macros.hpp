#include <assert.h>

#define UNUSED __attribute__((unused))
#define NO_RETURN __attribute__((noreturn))

#define UNREACHABLE_CODE assert(false);