// #include <iterator>
// #include <string.h>
// #include <unistd.h>
// #include <stack>
// #include <map>
// #include <experimental/filesystem>
#include "input/InputHandler.h"
#include "util/strutil.h"
#include "util/pathutil.h"
#include "global.h"

namespace bashkir
{

namespace fs = std::experimental::filesystem;

const std::string SEQ_LEFT_ARROW = "\033[D";
const std::string SEQ_RIGHT_ARROW = "\033[C";
const std::string SEQ_UP_ARROW = "\033[A";
const std::string SEQ_DOWN_ARROW = "\033[B";
const std::string SEQ_DELETE = "\033[3~";

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

const std::map<std::string, std::string> blocks = {
    { "(", ")" },
    { "{", "}" },
    { "'", "'" },
    { "`", "`" },
    { "\"", "\"" }
};

const std::string new_line_prefix = "> ";

Line::Line()
{
    memset(this->data, 0, max_line_length);
}

Line::Line(const std::string &init) : Line()
{
    if (init.length() >= max_line_length)
    {
        throw std::length_error("Max input line length exceeded");
    }
    strcpy(this->data, init.c_str());
    this->real_length = init.length();
}

InputHandler::InputHandler(std::shared_ptr<std::vector<std::string>> history)
    : hist(std::move(history)) {}

void InputHandler::writePrefix()
{
    std::string cPath = fs::current_path().c_str();
    util::fullToHomeRel(cPath);
    this->first_line_prefix = "paradox> " + cPath + " $ ";
    io.write(first_line_prefix);
}

std::string InputHandler::waitInput()
{
    this->cur_pos = {0, 0};
    this->input.push_back(Line());
    this->hist_ind = this->hist->size();
    this->escaped_next = false;
    this->end = false;
    this->opened_blocks = std::stack<std::string>();
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
                            this->escaped_next = false;
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
                this->escaped_next = (this->tmp_buf[i] == '\\');
                ++i;
            }
        }
    } while (!this->end);
    std::string ret = util::join(this->input, "\n");
    this->input.clear();
    return ret;
}

void InputHandler::pressCSIsequence(std::string csi_seq)
{
    if (csi_seq == SEQ_LEFT_ARROW)
    {
        if (!(this->cur_pos.line == 0 && this->cur_pos.pos == 0))
        {
            if (this->cur_pos.pos > 0)
            {
                io.write('\b');
                this->cur_pos.pos--;
            }
            else
            {
                // need to move to the end of prev line
                this->cur_pos.line--;
                this->cur_pos.pos = this->input[this->cur_pos.line].real_length;
                io.write(SEQ_UP_ARROW);
                size_t shift_num = this->cur_pos.pos + 
                    (this->cur_pos.line == 0 ? this->first_line_prefix.length() - new_line_prefix.length() : 0);
                for (size_t i = 0; i < shift_num; ++i)
                {
                    io.write(SEQ_RIGHT_ARROW);
                }
            }
        }
    }
    else if (csi_seq == SEQ_RIGHT_ARROW)
    {
        if (!(this->cur_pos.line == this->input.size() - 1 && this->cur_pos.pos == this->input.back().real_length))
        {
            if (this->cur_pos.pos < this->input[this->cur_pos.line].real_length)
            {
                io.write(SEQ_RIGHT_ARROW);
                this->cur_pos.pos++;
            }
            else
            {
                // move to the begin (after prefix) of the next line
                this->cur_pos.line++;
                this->cur_pos.pos = 0;
                io.write(SEQ_DOWN_ARROW);
                size_t shift_num = this->input[this->cur_pos.line - 1].real_length +
                    (this->cur_pos.line == 1 ? this->first_line_prefix.length() - new_line_prefix.length() : 0);
                for (size_t i = 0; i < shift_num; ++i)
                {
                    io.write(SEQ_LEFT_ARROW);
                }
            }
        }
    }
    else if (csi_seq == SEQ_UP_ARROW)
    {
        if (this->hist_ind > 0)
        {
            this->hist_ind -= 1;
            this->setHistoryItem();
        }
    }
    else if (csi_seq == SEQ_DOWN_ARROW)
    {
        if (this->hist_ind < this->hist->size() - 1)
        {
            this->hist_ind += 1;
            this->setHistoryItem();
        }
    }
    else if (csi_seq == SEQ_DELETE)
    {
        // Delete key can't remove new line, but backspace can
        Line &line = this->input[this->cur_pos.line];
        if (this->cur_pos.pos < line.real_length)
        {
            std::size_t subs_len = line.real_length - this->cur_pos.pos - 1;
            std::string last_part = util::substr(std::string(line.data), this->cur_pos.pos + 1, subs_len);
            io.write(last_part);
            io.write(' ');
            line.data[line.real_length] = '\0';
            line.real_length -= 1;
            io.write(std::string(subs_len + 1, '\b'));
            for (std::size_t i = 0; i <= subs_len; ++i)
            {
                line.data[this->cur_pos.pos + i] = line.data[this->cur_pos.pos + i + 1];
            }
        }
    }
}

void InputHandler::pressSimpleKey(char ch)
{
    switch (ch)
    {
    case BS_KEY_ENTER:
        if (this->escaped_next || !this->opened_blocks.empty())
        {
            this->addNewInputLine();
        }
        else
        {
            io.write("\r\n");
            this->end = true;
        }
        break;
    case BS_KEY_BACKSPACE:
        if (!(this->cur_pos.line == 0 && this->cur_pos.pos == 0))
        {
            if (this->cur_pos.pos > 0)
            {
                Line &line = this->input[this->cur_pos.line];
                std::size_t subs_len = line.real_length - this->cur_pos.pos;
                std::string last_part = util::substr(std::string(line.data), this->cur_pos.pos, subs_len);
                io.write('\b');
                io.write(last_part);
                io.write(" \b");
                io.write(std::string(subs_len, '\b'));
                line.real_length -= 1;
                this->cur_pos.pos -= 1;
                for (std::size_t i = 0; i <= subs_len; ++i)
                {
                    line.data[this->cur_pos.pos + i] = line.data[this->cur_pos.pos + i + 1];
                }
            }
            else
            {
                this->removeInputLine();
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
    Line &line = this->input[this->cur_pos.line];
    std::size_t subs_len = line.real_length - this->cur_pos.pos;
    std::string last_part = util::substr(std::string(line.data), this->cur_pos.pos, subs_len);
    io.write(chars);
    io.write(last_part);
    this->cur_pos.pos += chars.length();
    line.real_length += chars.length();
    io.write(std::string(subs_len, '\b'));
    for (std::size_t i = subs_len + 1; i != 0; --i)
    {
        line.data[this->cur_pos.pos + i - 1] = line.data[this->cur_pos.pos + i - 2];
    }
    for (std::size_t i = 0; i < chars.length(); ++i)
    {
        line.data[this->cur_pos.pos - chars.length() + i] = chars[i];
    }
    assert(line.real_length == strlen(line.data));
}

void InputHandler::setHistoryItem()
{
    Line &line = this->input[this->cur_pos.line];
    std::string hist_item = (*(this->hist))[this->hist_ind];
    io.write(std::string(this->cur_pos.pos, '\b'));
    io.write(hist_item);
    std::size_t max_space = line.real_length > hist_item.length() ? line.real_length : hist_item.length();
    if (max_space == line.real_length)
    {
        std::size_t free_space_len = line.real_length - hist_item.length();
        io.write(std::string(free_space_len, ' '));
        io.write(std::string(free_space_len, '\b'));
    }
    for (std::size_t i = 0; i < max_space; ++i)
    {
        if (i < hist_item.length())
        {
            line.data[i] = hist_item[i];
        }
        else
        {
            line.data[i] = '\0';
        }
    }
    this->cur_pos.pos = this->input[this->cur_pos.line].real_length = hist_item.length();
}

void InputHandler::addNewInputLine()
{
    // Update internal input buffer
    this->input.push_back(Line());
    for (size_t line = this->input.size() - 1; line != this->cur_pos.line + 1; --line)
    {
        this->input[line] = this->input[line - 1];
    }
    if (this->cur_pos.pos != this->input[this->cur_pos.line].real_length)
    {
        if (this->cur_pos.pos == 0)
        {
            this->input[this->cur_pos.line + 1] = this->input[this->cur_pos.line];
            this->input[this->cur_pos.line] = Line();
        }
        else
        {
            auto parts = util::splitInHalf(std::string(this->input[this->cur_pos.line].data), this->cur_pos.pos);
            this->input[this->cur_pos.line] = Line(std::get<0>(parts));
            this->input[this->cur_pos.line + 1] = Line(std::get<1>(parts));
        }
    }
    
    // Update presentation on the screen
    io.write(std::string(this->input[this->cur_pos.line + 1].real_length, ' ') + '\n');
    for (size_t line = this->cur_pos.line + 1; line < this->input.size(); ++line)
    {
        io.write(new_line_prefix);
        io.write(this->input[line].data);
        if (line != this->input.size() - 1)
        {
            int delta_length = this->input[line + 1].real_length - this->input[line].real_length;
            if (delta_length > 0)
            {
                io.write(std::string(delta_length, ' '));
            }
        }
        if (line != this->input.size() - 1)
        {
            io.write('\n');
        }
    }
    for (size_t i = 0; i < this->input[this->input.size() - 1].real_length; ++i)
    {
        io.write(SEQ_LEFT_ARROW);
    }
    for (size_t i = 0; i < this->input.size() - this->cur_pos.line - 2; ++i)
    {
        io.write(SEQ_UP_ARROW);
    }
    this->cur_pos.line++;
    this->cur_pos.pos = 0;
}

void InputHandler::removeInputLine()
{
    if (this->cur_pos.line == 0 || this->input.size() == 1)
    {
        return;
    }
    // Update internal input buffer
    Line &prev = this->input[this->cur_pos.line - 1];
    Line &cur = this->input[this->cur_pos.line];
    size_t orig_prev_len = prev.real_length;
    for (size_t i = 0; i < cur.real_length; ++i)
    {
        prev.data[prev.real_length + i] = cur.data[i];
    }
    prev.real_length += cur.real_length;
    for (size_t line = this->cur_pos.line; line < this->input.size() - 1; ++line)
    {
        this->input[line] = this->input[line + 1];
    }
    this->input.pop_back();

    // Update presentation on the screen
    io.write(SEQ_UP_ARROW);
    int delta_length = (this->cur_pos.line == 1 ? this->first_line_prefix.length() - new_line_prefix.length() : 0) - this->cur_pos.pos;
    for (size_t i = 0; i < delta_length; ++i)
    {  
        if (delta_length > 0)
        {
            io.write(SEQ_RIGHT_ARROW);
        }
        else
        {
            io.write(SEQ_LEFT_ARROW);
        }
    }
    io.write(this->input[this->cur_pos.line - 1].data);
    io.write('\n');
    for (size_t line = this->cur_pos.line; line < this->input.size(); ++line)
    {
        io.write(new_line_prefix);
        io.write(this->input[line].data);
        delta_length = (line == this->cur_pos.line ? orig_prev_len : this->input[line - 1].real_length) - this->input[line].real_length;
        if (delta_length > 0)
        {
            io.write(std::string(delta_length, ' '));
        }
        io.write('\n');
    }
    io.write(std::string(new_line_prefix.length(), ' '));
    io.write(std::string(this->input.back().real_length, ' '));
    delta_length =
        (this->cur_pos.line == 1 ? this->first_line_prefix.length() - new_line_prefix.length() : 0) +
        orig_prev_len - this->input[this->input.size() - 1].real_length;
    for (size_t i = 0; i < std::abs(delta_length); ++i)
    {
        if (delta_length > 0)
        {
            io.write(SEQ_RIGHT_ARROW);
        }
        else
        {
            io.write(SEQ_LEFT_ARROW);
        }
    }
    for (size_t i = 0; i < this->input.size() - this->cur_pos.line + 1; ++i)
    {
        io.write(SEQ_UP_ARROW);
    }
    this->cur_pos.line--;
    this->cur_pos.pos = orig_prev_len;
}

} // namespace bashkir
