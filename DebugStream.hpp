#ifndef DEBUGSTREAM_HPP
#define DEBUGSTREAM_HPP

#include <iostream>
#ifdef DEBUG_LOG_ENABLED
#  define DEBUG_LOG std::cerr

#else

#include "global.h"

class log_disabled_output {};
MAY_BE_UNUSED static log_disabled_output log_disabled_output_instance;

template<typename T>
inline log_disabled_output& operator << (log_disabled_output& any, MAY_BE_UNUSED T const& thing) { return any; }

// std::endl simple, quick and dirty
inline log_disabled_output& operator << (log_disabled_output& any, std::ostream&(*)(std::ostream&)) { return any; }

#  define DEBUG_LOG log_disabled_output_instance
#endif



#endif // DEBUGSTREAM_HPP
