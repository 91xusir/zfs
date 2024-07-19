// iob_func.cpp

#include <stdio.h>

// Define __iob_func to use __acrt_iob_func
extern "C" FILE* __cdecl __iob_func(unsigned i) {
    // This assumes __acrt_iob_func is defined somewhere in the project or library
    return __acrt_iob_func(i);
}
extern "C" FILE* __cdecl _iob(unsigned i) {
    // This assumes __acrt_iob_func is defined somewhere in the project or library
    return __acrt_iob_func(i);
}
