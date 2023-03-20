/* <<User guide to starting editor mods>>
 * A summary of what this library does:
 * This command prompt contains all the main functions to start a work with the editor.
 * This library can give the user enough information to work with the editor,
 * it can create and open a new file for the user using the file system library,
 * and also control the configurations.
 */
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

        void CommandsRouter()
        {
            /// Giving commands and running the related function
            string cmd;
            #if (defined (_WIN32) || defined (_WIN64))
            ColorPrint("cmd: ", GREEN);
            #endif
            #if (defined (LINUX) || defined (__linux__))
            ColorPrint("\e[1mcmd: \e[0m", GREEN);
            #endif
            getline (cin, cmd);
            
            if (cmd == "file -N" || cmd == "file --new")
            {
                currentMode = "edit";
                ResetAllEditFileData();
                ClearTerminalScreen();
                displayLocationInfo();
                printTabs();
                displayPageOfText(mainText, -1, -1);
                gotoxy (0, 2);
            } else if (cmd == "file -O" || cmd == "file --open") {
                if (fileSystem("open", mainText) == true) {
                    currentMode = "edit";
                    displayLocationInfo();
                    printTabs();
                    displayPageOfText(mainText, -1, -1);
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
                ColorPrint("[Command Error]: command not found\n", RED);
        }
};