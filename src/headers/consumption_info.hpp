#include <string.h>
#include "file_system.hpp"
#if (defined (_WIN32) || defined (_WIN64))
#include "windows.h"
#include "psapi.h"
#endif

int parseLine(char* line)
{
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

string EditorMemoryUsage()
{
    #if (defined (LINUX) || defined (__linux__))
    FILE* file = fopen("/proc/self/status", "r");
    int result = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL)
    {
        if (strncmp(line, "VmRSS:", 6) == 0)
        {
            result = parseLine(line);
            break;
        }
    }
    fclose(file);
    return byteConverter(result * 1024);
    #endif

    #if (defined (_WIN32) || defined (_WIN64))
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
    return byteConverter(physMemUsedByMe);
    #endif
}