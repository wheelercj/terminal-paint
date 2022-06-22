#include "common.h"
#ifdef _WIN32
#include <windows.h>
#endif

namespace ynot
{

    void sleep_(unsigned milliseconds)
    {
#ifdef _WIN32
        Sleep(milliseconds);
#else
        usleep(milliseconds * 1000);
#endif
    }

}
