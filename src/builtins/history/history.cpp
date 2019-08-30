#include <algorithm>
#include <iostream>
#include <iomanip>
#include "builtins/history/history.h"
#include "util/convutil.h"

namespace bashkir::builtins
{

History::History(std::shared_ptr<BaseIO> nc_io, std::shared_ptr<std::vector<std::string>> history)
    : io(std::move(nc_io)), hist(std::move(history)) {}

int History::exec(const Command &cmd)
{
    std::size_t i = 1;
    std::string size_str = std::to_string(this->hist->size());
    int index_cap = util::size_t2int(size_str.length());
    std::for_each(this->hist->begin(), this->hist->end(), [this, &i, &index_cap](std::string &command) {
        this->io->formatStr("  %" + std::to_string(index_cap) + "d  " + command, i);
        i++;
    });
    return 0;
}

} // namespace bashkir::builtins
