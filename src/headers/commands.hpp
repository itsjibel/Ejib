#include "UI.hpp"

class CommandLine : public FileSystem, public EditorUI
{
    public:
        void ResetAllEditFileData()
        {
            currentColumn=0;
            currentLine=0;
            startLineForDisplayPage=0;
            startColumnForDisplayPage=0;
            mainText.clear();
            vector<char> emptyVector;
            mainText.push_back(emptyVector);
        }

        void DisplayHelpFile(string helpFileName)
        {
            string line;
            ifstream HelpFile("../helps/" + helpFileName);

            if (HelpFile.is_open())
            {
                while(getline (HelpFile, line))
                {
                    string SwitchName, explanation;

                    for (char ch : line)
                        if (ch != ']')
                            SwitchName += ch;
                        else {
                            SwitchName += ch;
                            break;
                        }

                    bool startGivingExplain = false;

                    for (char ch : line)
                        if (startGivingExplain)
                            explanation += ch;
                        else if (ch == ']')
                            startGivingExplain=true;

                    ColorPrint('\t' + SwitchName, 6);
                    ColorPrint(explanation + '\n', 5);
                }
                HelpFile.close();
            } else
                ColorPrint("\tUnable to open \'" + helpFileName + "' file!\n", 4);
        }

        void CommandsRouter()
        {
            string cmd;
            #if (defined (_WIN32) || defined (_WIN64))
            ColorPrint("cmd: ", 2)
            #endif
            #if (defined (LINUX) || defined (__linux__))
            ColorPrint("\e[1mcmd: \e[0m", 2);
            #endif

            getline (cin, cmd);
            
            if (cmd == "file -N" || cmd == "file --new") {
                currentMode = "edit";
                ResetAllEditFileData();
                ClearTerminalScreen();
                displayLocationInfo();
                printTabs();
                displayPageOfText(mainText, -1, -1, currentLine, currentColumn);
                gotoxy (0, 2);
            } else if (cmd == "file -O" || cmd == "file --open") {
                if (fileSystem("open", mainText) == true) {
                    currentMode = "edit";
                    displayLocationInfo();
                    printTabs();
                    displayPageOfText(mainText, -1, -1, currentLine, currentColumn);
                } else
                    ClearTerminalScreen();
            }

            else if (cmd == "clear")
                ClearTerminalScreen();
                
            else if (cmd == "exit")
                exit (0);

            else if (cmd == "help")
                DisplayHelpFile("help.txt");

            else if (cmd == "help -F" ||
                     cmd == "help --file")
                DisplayHelpFile("file_help.txt");
            
            else if (cmd == "help -V" ||
                     cmd == "help --visual")
                DisplayHelpFile("visual_mode_help.txt");

            else if (cmd == "help -S" ||
                     cmd == "help --shortcuts")
                DisplayHelpFile("shortcuts_help.txt");

            else if (cmd == "") {}

            else
                ColorPrint("[Command Error]: command not found\n", 4);
        }
};