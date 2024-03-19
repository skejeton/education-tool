#ifdef _WIN32
#include "console.hpp"
#include <stdio.h>
#include <windows.h>

static void last_error_fatal()
{
    LPSTR message = nullptr;

    DWORD format_flags = 0;
    format_flags |= FORMAT_MESSAGE_ALLOCATE_BUFFER;
    format_flags |= FORMAT_MESSAGE_FROM_SYSTEM;
    format_flags |= FORMAT_MESSAGE_IGNORE_INSERTS;

    DWORD last_error = GetLastError();

    DWORD language = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

    DWORD size = FormatMessageA(format_flags, NULL, last_error, language,
                                (LPSTR)&message, 0, NULL);
    (void)size;

    FatalAppExitA(0, message);

    //   No need to free message as the app already exited.
}

// NOTE: Always creates a new console on windows.
void console_create_or_bind_existing()
{
    /*
    if (!AllocConsole())
    {
        last_error_fatal();
    }

    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    */
}
#else
void console_create_or_bind_existing()
{
    // Mac and Linux will make use of the default console
}
#endif
