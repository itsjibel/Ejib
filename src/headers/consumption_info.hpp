#include <string.h>
#include "file_system.hpp"

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
}

void DisplayMemoryUsage(string &mode, int &terminalColumn)
{
    while (1)
    {
        if (mode == "edit")
        {
            string MemoryUsageRawString = EditorMemoryUsage();
            string MemoryUseage;

            for (char ch : MemoryUsageRawString)
                if (ch == '.')
                    break;
                else
                    MemoryUseage += ch;

            MemoryUseage = MemoryUseage +\
                           MemoryUsageRawString[MemoryUsageRawString.size() - 2] +\
                           MemoryUsageRawString[MemoryUsageRawString.size() - 1];

            /*
            ShowConsoleCursor(false);
            gotoxy(terminalColumn - MemoryUseage.size() - 9, 0);
            ColorPrint("| MEM: " + MemoryUseage + " |", 3);
            */
           
            Sleep(100);
        } else
            Sleep(1);
    }
}