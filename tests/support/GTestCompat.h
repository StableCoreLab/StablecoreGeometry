#pragma once

#include <sstream>
#include <stdexcept>

#ifdef assert
#undef assert
#endif

#define assert(condition)                                                                          \
    do                                                                                             \
    {                                                                                              \
        if (!(condition))                                                                          \
        {                                                                                          \
            std::ostringstream geometryAssertStream;                                               \
            geometryAssertStream << "assert(" << #condition << ") failed at " << __FILE__ << ':' \
                                 << __LINE__;                                                      \
            throw std::runtime_error(geometryAssertStream.str());                                  \
        }                                                                                          \
    } while (false)
