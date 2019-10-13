// #include <iterator>
// #include <string.h>
// #include <unistd.h>
// #include <stack>
// #include <map>
// #include <experimental/filesystem>
// #include <algorithm>
#include "input/InputHandler.h"
#include "input/interface/CsiSequences.h"
#include "util/strutil.h"
#include "util/pathutil.h"
#include "global.h"

namespace bashkir
{

namespace fs = std::experimental::filesystem;

const uint8_t MIN_CSI_SEQ_LEN = 3;

const std::vector<std::string> CSI_seqs = {
    csi::BS_UP_ARROW,
    csi::BS_DOWN_ARROW,
    csi::BS_RIGHT_ARROW,
    csi::BS_LEFT_ARROW,
    csi::BS_SHIFT_UP_ARROW,
    csi::BS_SHIFT_DOWN_ARROW,
    csi::BS_SHIFT_RIGHT_ARROW,
    csi::BS_SHIFT_LEFT_ARROW,
    csi::BS_DELETE
};

InputHandler::InputHandler(std::shared_ptr<std::vector<std::string>> history)
    : keymap(*this), hist(std::move(history))
{
    this->input.push_back(Line());
}

PreParsedInput InputHandler::waitInput()
{
    this->flushState();
    this->writePrefix();
    this->runInputLoop();
    const PreParsedInput ret = compressInputData(this->blocks.getFullList(), this->input);
    assert(std::accumulate(
            ret.blocks.begin(),
            ret.blocks.end(),
            0,
            [](int sum, const CompressedBlockData &block) { return sum + (block.is_start ? block.uid : -static_cast<int>(block.uid)); }
        ) == 0);
    return ret;
}

void InputHandler::clearInput()
{
    if (this->input.size() > 1)
    {
        this->input.erase(this->input.begin() + 1, this->input.end());
    }
    memset(this->input[0].data, 0, sizeof(this->input[0].data));
    this->input[0].real_length = 0;
}

void InputHandler::flushState()
{
    this->cur = {0, 0};
    this->hist_ind = this->hist->size();
    this->end = false;
    this->untouched = false;
    this->mode = Mode::SINGLELINE;
    this->blocks = AllBlocksData();
    this->clearInput();
}

void InputHandler::writePrefix()
{
    std::string cPath = fs::current_path().c_str();
    util::fullToHomeRel(cPath);
    this->input[0].prefix = "paradox> " + cPath + " $ ";
    io.write(this->input[0].prefix);
}

void InputHandler::runInputLoop()
{
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
                auto csi = this->lookForCSISequenceInPos(i);
                if (csi)
                {
                    found_csi = true;
                    if (log::Lev3()) log::to.Info(*csi);
                    this->pressCSIsequence(*csi);
                    i += csi->length();
                }
            }
            if (!found_csi)
            {
                char ch = this->tmp_buf[i];
                if (log::Lev3()) log::to.Info(ch);
                this->pressSimpleKey(ch);
                if (this->end)
                {
                    break;
                }
                ++i;
            }
        }
    } while (!this->end);
}

void InputHandler::rebuildBlocksData(const Pos &from_pos)
{
    this->blocks.eraseAfterPos(from_pos);
    for (size_t pos = from_pos.pos; pos < this->input[from_pos.line].real_length + 1; ++pos)
    {
        detectBlocks(Pos(from_pos.line, pos));
    }
    for (size_t line_n = from_pos.line + 1; line_n < this->input.size(); ++line_n)
    {
        for (size_t pos = 0; pos < this->input[line_n].real_length + 1; ++pos)
        {
            detectBlocks(Pos(line_n, pos));
        }
    }
}

void InputHandler::detectBlocks(const Pos &inpos)
{
    auto fopn = global::blocksMeta.searchStartBeforePos(this->input[inpos.line].data, inpos.pos);
    auto fcls = global::blocksMeta.searchEndBeforePos(this->input[inpos.line].data, inpos.pos);
    if (!fopn && !fcls)
    {
        return;
    }
    if (this->blocks.open.empty() && fopn)
    {
        size_t seq_start_pos = inpos.pos - fopn->start_seq.length();
        if (!this->isPosEscaped(seq_start_pos))
        {
            const Pos start_pos = Pos(inpos.line, seq_start_pos);
            this->blocks.open.push(OpenBlock(*fopn, start_pos, false));
            const OpenBlock &ref = this->blocks.open.top();
            // *This is crutial to pass into the `addOpen` method the same reference when a block opens and closes
            this->blocks.addOpen(start_pos, ref); // *
        }
    }
    else if (!this->blocks.open.empty())// When there are already opened blocks here
    {
        const OpenBlock &last = this->blocks.open.top();
        // Check if the user print closing of last-opened block
        if (fcls && last.block == *fcls && last.escaped == this->isPosEscaped(inpos.pos - fcls->end_seq.length()))
        {
            this->blocks.open.pop();
            this->blocks.addClose(Pos(inpos.line, inpos.pos - fcls->end_seq.length()), last); // *
        }
        else if (fopn) // Check restrictions for opening a nested block
        {
            size_t seq_start_pos = inpos.pos - fopn->start_seq.length();
            bool esc = this->isPosEscaped(seq_start_pos);
            auto r = last.block.rules; // Following the defined rules
            if (r.esc == esc && (r.all || std::find(r.allowed.begin(), r.allowed.end(), *fopn) != r.allowed.end()))
            {
                const Pos start_pos = Pos(inpos.line, seq_start_pos);
                this->blocks.open.push(OpenBlock(*fopn, start_pos, esc));
                const OpenBlock &ref = this->blocks.open.top();
                this->blocks.addOpen(start_pos, ref); // *
            }
        }
    }
}

void InputHandler::setPos(size_t p) noexcept
{
    this->cur.pos = p;
    this->mem = p;
}

bool InputHandler::isPosEscaped(size_t pos) const
{
    bool escaped = false;
    int tmp_pos = static_cast<int>(pos) - 1;
    while (tmp_pos >= 0 && this->input[this->cur.line].data[tmp_pos] == '\\')
    {
        escaped = !escaped;
        --tmp_pos;
    }
    return escaped;
}

bool InputHandler::isPosEscaped(size_t pos, size_t line) const
{
    bool escaped = false;
    size_t tmp_pos = pos - 1;
    while (tmp_pos >= 0 && this->input[line].data[tmp_pos] == '\\')
    {
        escaped = !escaped;
        --tmp_pos;
    }
    return escaped;
}

bool InputHandler::isCurPosEscaped() const
{
    return this->isPosEscaped(this->cur.pos);
}

std::optional<std::string> InputHandler::lookForCSISequenceInPos(size_t pos)
{
    size_t rlen = strlen(this->tmp_buf);
    if (pos >= rlen)
    {
        return std::nullopt;
    }
    for (const std::string &csi : CSI_seqs)
    {
        if (pos + csi.length() <= rlen)
        {
            bool eq = true;
            for (std::size_t j = 2; j < csi.length(); j++)
            {
                if (this->tmp_buf[pos + j] != csi[j])
                {
                    eq = false;
                    break;
                }
            }
            if (eq)
            {
                return csi;
            }
        }
    }
    return std::nullopt;
}

void InputHandler::pressCSIsequence(std::string csi_seq)
{
    if (auto handler = this->keymap[csi_seq]; handler != std::nullopt)
    {
        (*handler)();
    }
    // if (csi_seq == SEQ_LEFT_ARROW)
    // {
    //     this->moveCursorLeft();
    // }
    // else if (csi_seq == SEQ_RIGHT_ARROW)
    // {
    //     this->moveCursorRight();
    // }
    // else if (csi_seq == SEQ_UP_ARROW || csi_seq == SEQ_SHIFT_UP_ARROW)
    // {
    //     // cool formula
    //     if (!(((this->mode == Mode::SINGLELINE && !this->untouched) || this->untouched) ^ csi_seq == SEQ_UP_ARROW))
    //     {
    //         if (this->hist_ind > 0)
    //         {
    //             this->hist_ind -= 1;
    //             this->setHistoryItem();
    //         }
    //     }
    //     else
    //     {
    //         this->moveCursorUp();
    //     }
    // }
    // else if (csi_seq == SEQ_DOWN_ARROW || csi_seq == SEQ_SHIFT_DOWN_ARROW)
    // {
    //     if (!(((this->mode == Mode::SINGLELINE && !this->untouched) || this->untouched) ^ csi_seq == SEQ_DOWN_ARROW))
    //     {
    //         if (this->hist_ind < this->hist->size() - 1)
    //         {
    //             this->hist_ind += 1;
    //             this->setHistoryItem();
    //         }
    //     }
    //     else
    //     {
    //         this->moveCursorDown();
    //     }
    // }
    // else if (csi_seq == SEQ_DELETE)
    // {
    //     this->removeFromRight();
    // }
    // else if (csi_seq == SEQ_SHIFT_UP_ARROW)
    // {
    //     io.write("\033[33");
    //     io.write("\033[?33");
    // }
}

void InputHandler::pressSimpleKey(char ch)
{
    auto handler = this->keymap[ch];
    if (ch == csi::BS_KEY_ENTER)
    {
        this->rebuildBlocksData(Pos(0, 0));
        if (handler != std::nullopt && (this->isCurPosEscaped() || !this->blocks.open.empty()))
        {
            (*handler)();
        }
        else
        {
            io.write("\r\n");
            this->untouched = false;
            this->end = true;
        }

    }
    else if (handler != std::nullopt)
    {
        (*handler)();
    }
    else switch (ch)
    {
    case csi::BS_KEY_CTRL_C:
        this->writeChars("^C");
        break;
    default:
        this->writeChars(std::string(1, ch));
        break;
    }
}

void InputHandler::writeChars(const std::string &chars)
{
    Line &line = this->input[this->cur.line];
    const std::size_t subs_len = line.real_length - this->cur.pos;
    std::string last_part = util::substr(std::string(line.data), this->cur.pos, subs_len);
    io.write(chars);
    io.write(last_part);
    io.write(std::string(subs_len, '\b'));
    for (std::size_t i = 0; i < subs_len; ++i)
    {
        line.data[line.real_length + chars.length() - i - 1] = line.data[line.real_length - i - 1];
    }
    for (std::size_t i = 0; i < chars.length(); ++i)
    {
        line.data[this->cur.pos + i] = chars[i];
    }
    this->setPos(this->cur.pos + chars.length());
    this->untouched = false;
    line.real_length += chars.length();
    assert(line.real_length == strlen(line.data));
}

} // namespace bashkir
