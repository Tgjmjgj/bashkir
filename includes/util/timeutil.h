#pragma once
#include "deps.h"
// #include <string>
// #include <chrono>
#include "global.h"
namespace bashkir::util
{

enum BenchDest : short int
{
    LOG,
    OUTPUT
};

template<typename Action>
inline bool benchmark(Action fn, BenchDest dest = BenchDest::LOG)
{
    auto time_start = std::chrono::system_clock::now();
    fn();
    auto time_end = std::chrono::system_clock::now();
    auto time_diff = std::chrono::duration<double, std::milli>(time_end - time_start).count();
    std::string str = "Duration: " + std::to_string(time_diff) + " ms";
    switch (dest)
    {
    case BenchDest::LOG:
        log::to.Info(str);
        break;
    case BenchDest::OUTPUT:
        io.writeStr(str);
        break;
    default:
        break;
    }
}

} // namespace bashkir::util
