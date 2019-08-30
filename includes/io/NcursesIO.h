#pragma once
#include <string>
#include <memory>
#include "io/interface/BaseIO.h"
#include "wrappers/NCurses.h"

namespace bashkir
{

class NcursesIO : public BaseIO
{
private:
    std::shared_ptr<NCurses> nc;
public:
    NcursesIO(std::shared_ptr<NCurses> ncurses);
    ~NcursesIO() {};
    void write(const std::string &str) const;
    void writeStr(const std::string &str) const;
    void formatStr(const std::string &str, ...) const;
    void error(const std::string &err_msg) const;
    std::string readStr() const;
    char readChar() const;
};

} // namespace bashkir
