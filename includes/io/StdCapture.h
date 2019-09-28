#pragma once
#include <string>
#include <mutex>

namespace bashkir
{

class StdCapture
{
private:
    enum PIPES { READ, WRITE };

    static int m_pipe[2];
    static int m_oldStdOut;
    static int m_oldStdErr;
    static bool m_capturing;
    static std::mutex m_mutex;
    static std::string m_captured;

public:
    static void BeginCapture();
    static bool IsCapturing();
    static bool EndCapture();
    static std::string GetCapture();
private:
    static int SecureDup(int src);
    static void SecureDup2(int src, int dest);
    static void SecurePipe(int *pipes);
    static void SecureClose(int &fd);
};

}
