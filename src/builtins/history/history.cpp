// #include <algorithm>
// #include <iomanip>
#include "builtins/history/history.h"
#include "util/convutil.h"
#include "global.h"

namespace bashkir::builtins
{

History::History(std::shared_ptr<std::vector<std::string>> history)
    : hist(std::move(history)) {}

int History::exec(const Command &cmd)
{
    std::size_t i = 1;
    std::string size_str = std::to_string(this->hist->size());
    int index_cap = util::size_t2int(size_str.length());
    std::for_each(this->hist->begin(), this->hist->end(), [this, &i, &index_cap](std::string &command) {
        io.formatStr("  %" + std::to_string(index_cap) + "d  " + command, i);
        i++;
    });
    return 0;
}

} // namespace bashkir::builtins
