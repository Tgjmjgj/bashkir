#include <string.h>
#include <string>
#include <vector>

namespace util
{
    inline char** StdStrVecToCStrArr(std::vector<std::string> strs)
    {
        char** retArr = new char*[strs.size()];
        for (int i = 0; i < strs.size(); ++i)
        {
            retArr[i] = new char[strs[i].length() + 1];
            strcpy(retArr[i], strs[i].c_str());
        }
        return retArr;
    }
}
