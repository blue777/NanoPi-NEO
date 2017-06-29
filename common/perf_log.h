
#include <iostream>
#include <string>
#include <chrono>

class PerfLog
{
public:
    PerfLog( const std::string &str )
    {
        m_str   = str;
        m_start = std::chrono::system_clock::now();
    }

    ~PerfLog()
    {
        m_end = std::chrono::system_clock::now();
        
        double elapsed = std::chrono::duration_cast<std::chrono::microseconds>(m_end-m_start).count();

        std::cout << ' ' << (int)elapsed / 1000 << "." << ((int)elapsed / 100) % 10 << " [msec] @ " << m_str << std::endl;
    }

protected:
    std::string     m_str;
    std::chrono::system_clock::time_point   m_start;
    std::chrono::system_clock::time_point   m_end;
};
