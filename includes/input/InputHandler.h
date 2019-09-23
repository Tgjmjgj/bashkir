#pragma once
#include <memory>
#include <vector>
#include <string>

#define INPUT_BUFFER_SIZE 1024
#define TMP_BUFFER_SIZE 10

namespace bashkir
{
    
class InputHandler
{
private:
    const std::shared_ptr<std::vector<std::string>> hist;
    char buffer[INPUT_BUFFER_SIZE];
    char tmp_buf[TMP_BUFFER_SIZE];
    
    std::size_t index;
    std::size_t iend;
    std::size_t hist_ind;

public:
    InputHandler(std::shared_ptr<std::vector<std::string>> history);
    std::string waitInput();
private:
    void pressCSIsequence(std::string csi_seq);
    void pressSimpleKey(char ch);
    void writeChars(const std::string &chars);
    std::size_t setHistoryItem();
};

} // namespace bashkir
