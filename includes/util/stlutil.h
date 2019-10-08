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

template <class T1, class T2, class Action>
inline std::vector<T2> map(const std::vector<T1> &main, Action act)
{
    std::vector<T2> res;
    std::transform(main.begin(), main.end(), std::back_inserter(res), act);
    return res;
}

} // namespace bashkir::util
