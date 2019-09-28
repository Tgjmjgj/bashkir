// SOURCE: https://stackoverflow.com/questions/5419356/redirect-stdout-stderr-to-a-string
// AUTHOR: Sir Digby Chicken Caesar
// BSD LICENSE
#ifdef _MSC_VER
#include <io.h>
#define popen _popen 
#define pclose _pclose
#define stat _stat 
#define dup _dup
#define dup2 _dup2
#define fileno _fileno
#define close _close
#define pipe _pipe
#define read _read
#define eof _eof
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <chrono>
#include <thread>
#include "io/StdCapture.h"
#include "util/strutil.h"

namespace bashkir
{

int StdCapture::m_pipe[2];
int StdCapture::m_oldStdOut;
int StdCapture::m_oldStdErr;
bool StdCapture::m_capturing;
std::mutex StdCapture::m_mutex;
std::string StdCapture::m_captured;

void StdCapture::BeginCapture()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_capturing)
    {
        return;
    }
    SecurePipe(m_pipe);
    m_oldStdOut = SecureDup(STDOUT_FILENO);
    m_oldStdErr = SecureDup(STDERR_FILENO);
    SecureDup2(m_pipe[WRITE], STDOUT_FILENO);
    SecureDup2(m_pipe[WRITE], STDERR_FILENO);
    m_capturing = true;
#ifndef _MSC_VER
    SecureClose(m_pipe[WRITE]);
#endif
}

bool StdCapture::IsCapturing()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_capturing;
}

bool StdCapture::EndCapture()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_capturing)
    {
        return false;
    }
    m_captured.clear();
    SecureDup2(m_oldStdOut, STDOUT_FILENO);
    SecureDup2(m_oldStdErr, STDERR_FILENO);

    const size_t bufSize = 1025;
    char buf[bufSize];
    int bytesRead = 0;
    bool fd_blocked(false);
    do
    {
        bytesRead = 0;
        fd_blocked = false;
#ifdef _MSC_VER
        if (!eof(m_pipe[READ]))
        {
            bytesRead = read(m_pipe[READ], buf, bufSize - 1);
        }
#else
        bytesRead = read(m_pipe[READ], buf, bufSize - 1);
#endif
        if (bytesRead > 0)
        {
            buf[bytesRead] = 0;
            m_captured += buf;
        }
        else if (bytesRead < 0)
        {
            fd_blocked = (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR);
            if (fd_blocked)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
    while (fd_blocked || bytesRead == (bufSize - 1));

    SecureClose(m_oldStdOut);
    SecureClose(m_oldStdErr);
    SecureClose(m_pipe[READ]);
#ifdef _MSC_VER
    SecureClose(m_pipe[WRITE]);
#endif
    m_capturing = false;
}

std::string StdCapture::GetCapture()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    util::remove_eol(m_captured);
    return m_captured;
}

int StdCapture::SecureDup(int src)
{
    int ret = -1;
    bool fd_blocked = false;
    do
    {
            ret = dup(src);
            fd_blocked = (errno == EINTR ||  errno == EBUSY);
            if (fd_blocked)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
    } while (ret < 0);
    return ret;
}

void StdCapture::SecureDup2(int src, int dest)
{
    int ret = -1;
    bool fd_blocked = false;
    do
    {
        ret = dup2(src,dest);
        fd_blocked = (errno == EINTR ||  errno == EBUSY);
        if (fd_blocked)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } while (ret < 0);
}

void StdCapture::SecurePipe(int * pipes)
{
    int ret = -1;
    bool fd_blocked = false;
    do
    {
#ifdef _MSC_VER
        ret = pipe(pipes, 65536, O_BINARY);
#else
        ret = pipe(pipes) == -1;
#endif
        fd_blocked = (errno == EINTR ||  errno == EBUSY);
        if (fd_blocked)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } while (ret < 0);
}

void StdCapture::SecureClose(int & fd)
{
    int ret = -1;
    bool fd_blocked = false;
    do
    {
        ret = close(fd);
        fd_blocked = (errno == EINTR);
        if (fd_blocked)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    } while (ret < 0);
    fd = -1;
}

} // namespace bashkir
