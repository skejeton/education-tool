#include "open_dir.hpp"
#include <cstdlib>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif

void catedu::sys::open(const char *path)
{
#if defined(_WIN32)
    ShellExecuteA(NULL, "explore", path, NULL, NULL, SW_SHOWNORMAL);
#elif defined(__APPLE__)
    system(("open " + std::string(path)).c_str());
#elif defined(__linux__)
    system(("xdg-open " + std::string(path)).c_str());
#endif
}
