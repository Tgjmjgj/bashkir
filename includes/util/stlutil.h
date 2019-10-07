#pragma once
#include "deps.h"
// #include <algorithm>
#include "global.h"

namespace bashkir::util
{

template <class T, class Predicate>
inline std::vector<T> filter(const std::vector<T> &main, Predicate pred)
{
    std::vector<T> res;
    std::copy_if(main.begin(), main.end(), std::back_inserter(res), pred);
    return res;
}

} // namespace bashkir::util
