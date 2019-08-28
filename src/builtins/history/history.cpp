#include <algorithm>
#include <iostream>
#include <iomanip>
#include "builtins/history/history.h"
#include "util/convutil.h"

namespace bashkir::builtins
{

History::History(std::shared_ptr<std::vector<std::string>> hist)
    : history(std::move(hist)) {}

int History::exec(const Command &cmd)
{
    std::size_t i = 1;
    std::string size_str = std::to_string(this->history->size());
    int index_cap = util::size_t2int(size_str.length());
    std::for_each(this->history->begin(), this->history->end(), [this, &i, &index_cap](std::string &command) {
        std::cout << "  " << std::setw(index_cap) << i << "  " << command << std::endl;
        i++;
    });
    return 0;
}

} // namespace bashkir::builtins
