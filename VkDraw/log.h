#include <iostream>
#include <format>

#define LOG(fmt, ...) std::cout << std::format(fmt, __VA_ARGS__) << std::endl