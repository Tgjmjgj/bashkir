#include <iostream>
#include <iterator>
#include <cstring>
#include <unistd.h>
#include "input/InputHandler.h"

namespace bashkir
{
InputHandler::InputHandler(std::shared_ptr<std::vector<std::string>> history)
    : hist(std::move(history)) {}

std::string InputHandler::waitInput()
{
    const size_t buf_size = 10;
    char buf[buf_size];
    do
    {
        memset(buf, 0, sizeof(buf));
        read(STDIN_FILENO, &buf, sizeof(buf));
        std::cout << buf;
        if (strcmp(buf, "[D") == 0)
        {
            std::cout << '\b';
        }
        else if (strcmp(buf, "^D") == 0)
        {
            std::cout << "\b \b";
        }
    } while (buf[0] != '\n');

    return "ls -a";
}

} // namespace bashkir
