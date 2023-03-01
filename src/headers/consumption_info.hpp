#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tools.hpp"

int parseLine(char* line){
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char* p = line;
    while (*p <'0' || *p > '9') p++;
    line[i-3] = '\0';
    i = atoi(p);
    return i;
}

string MEM_USAGE()
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

void display_MEM_USAGE(string &mode, int &terminalColumn, int &terminalLine)
{
    while (1)
    {
        if (mode == "edit")
        {
            string raw_MEM_USAGE_String = MEM_USAGE();
            string MemUseage;

            for (char ch : raw_MEM_USAGE_String)
                if (ch == '.')
                    break;
                else
                    MemUseage += ch;

            MemUseage = MemUseage +\
                        raw_MEM_USAGE_String[raw_MEM_USAGE_String.size() - 2] +\
                        raw_MEM_USAGE_String[raw_MEM_USAGE_String.size() - 1];

            ShowConsoleCursor(false);
            gotoxy(terminalColumn - MemUseage.size() - 9, 0);
            ColourizePrint("| MEM: " + MemUseage + " |", 3);
            Sleep(100);
        } else
            Sleep(1);
    }
}