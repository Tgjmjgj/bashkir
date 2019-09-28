#pragma once
#include "deps.h"
// #include <iostream>
// #include <sstream>
// #include <string>

namespace bashkir
{

class ScopedOutputRedirect
{
private:
    std::stringstream ss;
    std::streambuf *old;
public:
    ScopedOutputRedirect();
    ~ScopedOutputRedirect();
    std::string CollectOutput();
};

} // namespace bashkir
