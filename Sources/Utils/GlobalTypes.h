#pragma once

#include <stdint.h>

// Signed Integral Types

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;

#ifdef ENVIRONMENT64
using int64 = int64_t;
#endif


// Unsigned Integral types

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;

#ifdef ENVIRONMENT64
using uint64 = uint64_t;
#endif

using byte = uint8;
using uint = uint32;


// Character types
using char8 = char;
using char16 = char16_t;
using char32 = char32_t;

using wchar = wchar_t;

