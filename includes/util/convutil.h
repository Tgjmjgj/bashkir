#include <string.h>
#include <string>
#include <vector>

namespace bashkir::util
{
    inline char* const* NullTerminatedCStrArr(std::vector<std::string> strs)
    {
        char** retArr = new char*[strs.size() + 1];
        for (int i = 0; i < strs.size(); ++i)
        {
            retArr[i] = new char[strs[i].length() + 1];
            strcpy(retArr[i], strs[i].c_str());
        }
        retArr[strs.size()] = NULL;
        return retArr;
    }
}
