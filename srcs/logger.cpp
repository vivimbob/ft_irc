#include "../includes/logger.hpp"
#include <string>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <iomanip>

namespace logger
{
static const char*
    GetPrefix(int level)
{
    switch (level)
    {
        case LogLevel_Verbose:
            return "\033[34m[VERBOSE]";
        case LogLevel_Debug:
            return "\033[36m[DEBUG]";
        case LogLevel_Info:
            return "\033[37m[INFO]";
        case LogLevel_Warning:
            return "\033[33m[WARNING]";
        case LogLevel_Error:
            return "\033[35m[ERROR]";
        case LogLevel_Fatal:
            return "\033[31m[FATAL]";
        default:
            return "\033[0m";
    }
}

static void
    print_time(void)
{
    std::time_t	time = std::time(NULL);
    char        buffer[100];

    if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time)))
        std::cout << buffer;
}

static void
   Printfp(int level, const char* prefix, const char* format, va_list& va)
{
    std::cout << "[";
    print_time();
    std::cout << "] ";
    std::cout << std::setw(14) << std::left << GetPrefix(level);
    std::cout << " [";
    std::cout << std::setw(22) << std::left << prefix;
    std::cout << "] ";
    std::vfprintf(stdout, format, va);
    std::fflush(stdout);
    std::cout << GetPrefix(-1);
    std::cout << std::endl;
}

static void
    Printf(int level, const char* format, va_list& va)
{
    Printfp(level, "", format, va);
}

#define VBEGIN  va_list va; va_start(va, format)
#define VEND    va_end(va)

void    V(const char* format, ...) { VBEGIN; Printf(LogLevel_Verbose, format, va); VEND; }
void    D(const char* format, ...) { VBEGIN; Printf(LogLevel_Debug, format, va); VEND; }
void    I(const char* format, ...) { VBEGIN; Printf(LogLevel_Info, format, va); VEND; }
void    W(const char* format, ...) { VBEGIN; Printf(LogLevel_Warning, format, va); VEND; }
void    E(const char* format, ...) { VBEGIN; Printf(LogLevel_Error, format, va); VEND; }
void    F(const char* format, ...) { VBEGIN; Printf(LogLevel_Fatal, format, va); VEND; }

void    Vp(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Verbose, prefix, format, va); VEND; }
void    Dp(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Debug, prefix, format, va); VEND; }
void    Ip(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Info, prefix, format, va); VEND; }
void    Wp(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Warning, prefix, format, va); VEND; }
void    Ep(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Error, prefix, format, va); VEND; }
void    Fp(const char* prefix, const char* format, ...) { VBEGIN; Printfp(LogLevel_Fatal, prefix, format, va); VEND; }
};  //  namespace namespace