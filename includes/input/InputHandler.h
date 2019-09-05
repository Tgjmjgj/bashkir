#pragma once
#include <memory>
#include <vector>
#include <string>
#include "io/interface/BaseIO.h"

#define INPUT_BUFFER_SIZE 1024

namespace bashkir
{
    
class InputHandler
{
private:
    const std::shared_ptr<BaseIO> io;
    const std::shared_ptr<std::vector<std::string>> hist;
    char buffer[INPUT_BUFFER_SIZE];
public:
    InputHandler(std::shared_ptr<BaseIO> nc_io, std::shared_ptr<std::vector<std::string>> history);
    std::string waitInput();
private:
    std::size_t setHistoryItem(std::size_t hist_ind, std::size_t index, std::size_t iend);
};

} // namespace bashkir
