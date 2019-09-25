#include "deps.h"
// #include <string.h>
// #include <string>
// #include <vector>

namespace bashkir::util
{

inline char *const *createExecArgs(const std::string &program, const std::vector<std::string> &args)
{
    char const **ret_arr = new char const *[args.size() + 2];
    for (std::size_t i = 0; i < args.size(); ++i)
    {
        ret_arr[1 + i] = args[i].c_str();
    }
    ret_arr[0] = program.c_str();
    ret_arr[args.size() + 1] = NULL;
    auto pcpc = const_cast<char *const *>(ret_arr);
    return pcpc;
}

inline std::size_t int2size_t(int val) {
    return (val < 0) ? __SIZE_MAX__ : static_cast<std::size_t>(static_cast<unsigned int>(val));
}

inline std::size_t li2size_t(long int val) {
    return (val < 0) ? __SIZE_MAX__ : static_cast<std::size_t>(static_cast<unsigned int>(val));
}

inline int size_t2int(std::size_t val) {
    return (val <= __INT_MAX__) ? static_cast<int>(static_cast<ssize_t>(val)) : -1;
}

inline uint c2ui(char ch)
{
    return static_cast<uint>(static_cast<u_char>(ch));
}

inline uint i2ui(int i)
{
    return static_cast<uint>(i);
}

} // namespace bashkir::util
