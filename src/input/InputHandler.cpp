// #include <iterator>
// #include <string.h>
// #include <unistd.h>
#include "input/InputHandler.h"
#include "util/strutil.h"
#include "global.h"

namespace bashkir
{

const char *const SEQ_LEFT_ARROW = "\033[D";
const char *const SEQ_RIGHT_ARROW = "\033[C";
const char *const SEQ_UP_ARROW = "\033[A";
const char *const SEQ_DOWN_ARROW = "\033[B";
const char *const SEQ_DELETE = "\033[3~";

const char BS_KEY_ENTER = '\r';
const char BS_KEY_BACKSPACE = '\177';
const char BS_KEY_CTRL_C = '\3';

const uint8_t MIN_CSI_SEQ_LEN = 3;

const std::vector<std::string> CSI_seqs = {
    SEQ_LEFT_ARROW,
    SEQ_RIGHT_ARROW,
    SEQ_UP_ARROW,
    SEQ_DOWN_ARROW,
    SEQ_DELETE
};

InputHandler::InputHandler(std::shared_ptr<std::vector<std::string>> history)
    : hist(std::move(history)) {}

std::string InputHandler::waitInput()
{
    memset(&this->buffer, 0, sizeof(this->buffer));
    this->index = this->iend = 0;
    this->hist_ind = this->hist->size();
    bool new_line = false;
    do
    {
        memset(this->tmp_buf, 0, sizeof(this->tmp_buf));
        read(STDIN_FILENO, &this->tmp_buf, sizeof(this->tmp_buf));
        std::size_t rlen = strlen(this->tmp_buf);
        for (std::size_t i = 0; i < rlen;)
        {
            bool found_csi = false;
            if (i + MIN_CSI_SEQ_LEN <= rlen && this->tmp_buf[i] == '\033' && this->tmp_buf[i + 1] == '[')
            {
                for (const std::string &csi : CSI_seqs)
                {
                    if (i + csi.length() <= rlen)
                    {
                        bool eq = true;
                        for (std::size_t j = 2; j < csi.length(); j++)
                        {
                            if (this->tmp_buf[i + j] != csi[j])
                            {
                                eq = false;
                                break;
                            }
                        }
                        if (eq)
                        {
                            found_csi = true;
                            if (log::Lev3()) log::to->Info(csi);
                            this->pressCSIsequence(csi);
                            i += csi.length();
                            break;
                        }
                    }
                }
            }
            if (!found_csi)
            {
                if (log::Lev3()) log::to->Info(this->tmp_buf[i]);
                this->pressSimpleKey(this->tmp_buf[i]);
                if (tmp_buf[i] == '\r')
                {
                    new_line = true;
                }
                ++i;
            }
        }
    } while (!new_line);
    return std::string(this->buffer);
}

void InputHandler::pressCSIsequence(std::string csi_seq)
{
    if (csi_seq == SEQ_LEFT_ARROW)
    {
        if (this->index > 0)
        {
            io.write('\b');
            --(this->index);
        }
    }
    else if (csi_seq == SEQ_RIGHT_ARROW)
    {
        if (this->index < this->iend)
        {
            io.write(SEQ_RIGHT_ARROW);
            ++(this->index);
        }
    }
    else if (csi_seq == SEQ_UP_ARROW)
    {
        if (this->hist_ind > 0)
        {
            --(this->hist_ind);
            this->index = this->iend = this->setHistoryItem();
        }
    }
    else if (csi_seq == SEQ_DOWN_ARROW)
    {
        if (this->hist_ind < this->hist->size() - 1)
        {
            ++(this->hist_ind);
            this->index = this->iend = this->setHistoryItem();
        }
    }
    else if (csi_seq == SEQ_DELETE)
    {
        if (this->index < this->iend)
        {
            std::size_t subs_len = this->iend - this->index - 1;
            char *last_part = util::substr(this->buffer, this->index + 1, subs_len);
            io.write(last_part);
            io.write(' ');
            this->buffer[this->iend] = '\0';
            --(this->iend);
            io.write(std::string(subs_len + 1, '\b'));
            for (std::size_t i = 0; i <= subs_len; ++i)
            {
                this->buffer[this->index + i] = this->buffer[this->index + i + 1];
            }
        }
    }
}

void InputHandler::pressSimpleKey(char ch)
{
    switch (ch)
    {
    case BS_KEY_ENTER:
        io.write("\r\n");
        break;
    case BS_KEY_BACKSPACE:
        if (this->index > 0)
        {
            std::size_t subs_len = this->iend - this->index;
            char *last_part = util::substr(this->buffer, this->index, subs_len);
            io.write('\b');
            io.write(last_part);
            io.write(" \b");
            this->buffer[this->iend] = '\0';
            --(this->index);
            --(this->iend);
            io.write(std::string(subs_len, '\b'));
            for (std::size_t i = 0; i <= subs_len; ++i)
            {
                this->buffer[this->index + i] = this->buffer[this->index + i + 1];
            }
        }
        break;
    case BS_KEY_CTRL_C:
        this->writeChars("^C");
        break;
    default:
        this->writeChars(std::string(1, ch));
        break;
    }
}

void InputHandler::writeChars(const std::string &chars)
{
    std::size_t subs_len = this->iend - this->index;
    char *last_part = util::substr(this->buffer, this->index, subs_len);
    io.write(chars);
    io.write(last_part);
    this->index += chars.length();
    this->iend += chars.length();
    io.write(std::string(subs_len, '\b'));
    for (std::size_t i = subs_len + 1; i != 0; --i)
    {
        this->buffer[this->index + i - 1] = this->buffer[this->index + i - 2];
    }
    for (std::size_t i = 0; i < chars.length(); ++i)
    {
        this->buffer[this->index - chars.length() + i] = chars[i];
    }
    assert(this->iend == strlen(this->buffer));
}

std::size_t InputHandler::setHistoryItem()
{
    std::string hist_item = (*(this->hist))[this->hist_ind];
    io.write(std::string(this->index, '\b'));
    io.write(hist_item);
    std::size_t max_space = this->iend > hist_item.length() ? this->iend : hist_item.length();
    if (max_space == this->iend)
    {
        std::size_t free_space_len = this->iend - hist_item.length();
        io.write(std::string(free_space_len, ' '));
        io.write(std::string(free_space_len, '\b'));
    }
    for (std::size_t i = 0; i < max_space; ++i)
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
    std::size_t new_index = hist_item.length();
    return new_index;
}

} // namespace bashkir
