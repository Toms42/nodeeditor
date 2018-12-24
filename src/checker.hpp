#pragma once

#include <exception>
#include <sstream>

#define GET_INFO()                                                             \
  std::stringstream error_stream_info;                                         \
  error_stream_info << __FILE__ << ':' << __LINE__ << " in " << __FUNCTION__;  \
  throw std::runtime_error{error_stream_info.str()};
