#include <iostream>
#include <iterator>
#include <cstring>
#include <unistd.h>
#include "input/InputHandler.h"
#include "util/strutil.h"

#define KEY_LEFT_ARROW      "\033[D"
#define KEY_RIGHT_ARROW     "\033[C"
#define KEY_UP_ARROW        "\033[A"
#define KEY_DOWN_ARROW      "\033[B"
#define KEY_ENTER           "\r"
#define KEY_BACKSPACE       "\177"

namespace bashkir
{
InputHandler::InputHandler(std::shared_ptr<BaseIO> nc_io, std::shared_ptr<std::vector<std::string>> history)
    : io(std::move(nc_io)), hist(std::move(history)) {}

std::string InputHandler::waitInput()
{
    memset(&this->buffer, 0, sizeof(this->buffer));
    const std::size_t tmp_buf_size = 5;
    char tmp_buf[tmp_buf_size];
    std::size_t index = 0, iend = 0;
    std::size_t hist_ind = this->hist->size();
    do
    {
        memset(tmp_buf, 0, sizeof(tmp_buf));
        read(STDIN_FILENO, &tmp_buf, sizeof(tmp_buf));
        if (util::is_empty(tmp_buf))
        {
            continue;
        }
        if (strcmp(tmp_buf, KEY_ENTER) == 0)
        {
            this->io->write("\r\n");
        }
        else if (strcmp(tmp_buf, KEY_LEFT_ARROW) == 0)
        {
            if (index > 0)
            {
                this->io->write('\b');
                --index;
            }
        }
        else if (strcmp(tmp_buf, KEY_RIGHT_ARROW) == 0)
        {
            if (index < iend)
            {
                this->io->write(KEY_RIGHT_ARROW);
                ++index;
            }
        }
        else if (strcmp(tmp_buf, KEY_UP_ARROW) == 0)
        {
            if (hist_ind > 1)
            {
                --hist_ind;
                std::string hist_item = (*(this->hist))[hist_ind];
                this->io->write(std::string(index, '\b'));
                this->io->write(hist_item);
                if (iend > hist_item.length())
                {
                    std::size_t free_space_len = iend - hist_item.length();
                    this->io->write(std::string(free_space_len, ' '));
                    this->io->write(std::string(free_space_len, '\b'));
                }
                for (std::size_t i = 0; i < iend; ++i)
                {
                    if (i < hist_item.length())
                    {
                        this->buffer[i] = hist_item[i];
                    }
                    else
                    {
                        this->buffer[i] = '\0';
                    }
                }
                index = iend = hist_item.length();
            }
        }
        else if (strcmp(tmp_buf, KEY_DOWN_ARROW) == 0)
        {
            if (hist_ind < this->hist->size() - 1)
            {
                ++hist_ind;
                std::string hist_item = (*(this->hist))[hist_ind];
                this->io->write(std::string(index, '\b'));
                this->io->write(hist_item);
                if (iend > hist_item.length())
                {
                    std::size_t free_space_len = iend - hist_item.length();
                    this->io->write(std::string(free_space_len, ' '));
                    this->io->write(std::string(free_space_len, '\b'));
                }
                for (std::size_t i = 0; i < iend; ++i)
                {
                    if (i < hist_item.length())
                    {
                        this->buffer[i] = hist_item[i];
                    }
                    else
                    {
                        this->buffer[i] = '\0';
                    }
                }
                index = iend = hist_item.length();
            }
        }
        else if (strcmp(tmp_buf, KEY_BACKSPACE) == 0)
        {
            if (index > 0)
            {
                std::size_t subs_len = iend - index; 
                char* last_part = util::substr(this->buffer, index, subs_len);
                this->io->write('\b');
                this->io->write(last_part);
                this->io->write(" \b");
                this->buffer[iend] = '\0';
                --index;
                --iend;
                for (std::size_t i = 0; i < subs_len; ++i)
                {
                    this->io->write('\b');
                    this->buffer[index + i] = this->buffer[index + i + 1];
                }
                this->buffer[iend] = '\0';
            }
        }
        else
        {
            std::size_t subs_len = iend - index; 
            char* last_part = util::substr(this->buffer, index, subs_len);
            this->io->write(tmp_buf);
            this->io->write(last_part);
            ++index;
            ++iend;
            for (std::size_t i = subs_len; i != 0; --i)
            {
                this->io->write('\b');
                this->buffer[index + i] = this->buffer[index + i - 1];
            }
            this->buffer[index] = this->buffer[index - 1];
            this->buffer[index - 1] = tmp_buf[0];
        }
    } while (tmp_buf[0] != '\r');
    return std::string(this->buffer);
}

} // namespace bashkir
