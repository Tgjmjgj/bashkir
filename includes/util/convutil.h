#include <string.h>
#include <string>
#include <vector>

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

} // namespace bashkir::util
