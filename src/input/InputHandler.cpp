#include <iostream>
#include <iterator>
#include <cstring>
#include <unistd.h>
#include "input/InputHandler.h"
#include "util/strutil.h"
#include "global.h"

#define SEQ_LEFT_ARROW "\033[D"
#define SEQ_RIGHT_ARROW "\033[C"
#define SEQ_UP_ARROW "\033[A"
#define SEQ_DOWN_ARROW "\033[B"
#define SEQ_DELETE "\033[3~"

#define KEY_ENTER '\r'
#define KEY_BACKSPACE '\177'

#define MIN_CSI_SEQ_LEN 3

namespace bashkir
{

const std::vector<std::string> CSI_seqs = {
    SEQ_LEFT_ARROW,
    SEQ_RIGHT_ARROW,
    SEQ_UP_ARROW,
    SEQ_DOWN_ARROW,
    SEQ_DELETE
};

InputHandler::InputHandler(std::shared_ptr<BaseIO> nc_io, std::shared_ptr<std::vector<std::string>> history)
    : io(std::move(nc_io)), hist(std::move(history))
{
    this->log_level = 2;
}

std::string InputHandler::waitInput()
{
    memset(&this->buffer, 0, sizeof(this->buffer));
    this->index = this->iend = 0;
    this->hist_ind = this->hist->size();
    bool new_line = false;
    do
    {
        memset(tmp_buf, 0, sizeof(tmp_buf));
        read(STDIN_FILENO, &tmp_buf, sizeof(tmp_buf));
        std::size_t rlen = strlen(tmp_buf);
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
                            if (this->log_level == 2) log::to->Info(csi);
                            this->pressCSIsequence(csi);
                            i += csi.length();
                            break;
                        }
                    }
                }
            }
            if (!found_csi)
            {
                if (this->log_level == 2) log::to->Info(this->tmp_buf[i]);
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
        if (index > 0)
        {
            this->io->write('\b');
            --index;
        }
    }
    else if (csi_seq == SEQ_RIGHT_ARROW)
    {
        if (index < iend)
        {
            this->io->write(SEQ_RIGHT_ARROW);
            ++index;
        }
    }
    else if (csi_seq == SEQ_UP_ARROW)
    {
        if (hist_ind > 0)
        {
            --hist_ind;
            index = iend = this->setHistoryItem();
        }
    }
    else if (csi_seq == SEQ_DOWN_ARROW)
    {
        if (hist_ind < this->hist->size() - 1)
        {
            ++hist_ind;
            index = iend = this->setHistoryItem();
        }
    }
}

void InputHandler::pressSimpleKey(char ch)
{
    switch (ch)
    {
    case KEY_ENTER:
        this->io->write("\r\n");
        break;
    case KEY_BACKSPACE:
        if (index > 0)
        {
            std::size_t subs_len = iend - index;
            char *last_part = util::substr(this->buffer, index, subs_len);
            this->io->write('\b');
            this->io->write(last_part);
            this->io->write(" \b");
            this->buffer[iend] = '\0';
            --index;
            --iend;
            this->io->write(std::string(subs_len, '\b'));
            for (std::size_t i = 0; i <= subs_len; ++i)
            {
                this->buffer[index + i] = this->buffer[index + i + 1];
            }
        }
        break;
    default:
        std::size_t subs_len = iend - index;
        char *last_part = util::substr(this->buffer, index, subs_len);
        this->io->write(ch);
        this->io->write(last_part);
        ++index;
        ++iend;
        this->io->write(std::string(subs_len, '\b'));
        for (std::size_t i = subs_len + 1; i != 0; --i)
        {
            this->buffer[index + i - 1] = this->buffer[index + i - 2];
        }
        this->buffer[index - 1] = ch;
        break;
    }
}

std::size_t InputHandler::setHistoryItem()
{
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
    std::size_t new_index = hist_item.length();
    return new_index;
}

} // namespace bashkir
