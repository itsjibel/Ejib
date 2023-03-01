#include "text_interface.hpp"
class CommandLine : public File, public EditorUI
{
    private:
        bool _firstError=true;

    public:
        void clearEditFile()
        {
            columnSelected=0;
            lineSelected=0;
            startPrintLine=0;
            startPrintColumn=0;
            input.clear();
            input.push_back(emptyVector);
        }

        void showHelpFile(string helpFileName)
        {
            string line;
            ifstream myfile("../helps/" + helpFileName);

            if (myfile.is_open()) {
                while(getline (myfile, line))
                {
                    string key, explanation;

                    for (char ch : line)
                        if (ch != ']')
                            key += ch;
                        else {
                            key += ch;
                            break;
                        }

                    bool startGivingExplain = false;

                    for (char ch : line)
                        if (startGivingExplain)
                            explanation += ch;
                        else if (ch == ']')
                            startGivingExplain=true;
                    ColourizePrint('\t' + key, 6);
                    ColourizePrint(explanation + '\n', 5);
                }
                myfile.close();
            } else {
                ColourizePrint("\tUnable to open \'" + helpFileName + "' file!\n", 4);
            }
        }

        void COMMAND_LINE()
        {
            string cmd;
            #if (defined (_WIN32) || defined (_WIN64))
            ColourizePrint("cmd: ", 2)
            #endif
            #if (defined (LINUX) || defined (__linux__))
            ColourizePrint("\e[1mcmd: \e[0m", 2);
            #endif

            getline (cin, cmd);
            
            if (cmd == "file -N" || cmd == "file --new") {
                mode = "edit";
                clearEditFile();
                clearTerminal();
                printInfo();
                printTabs();
                printText(input, -1, -1, lineSelected, columnSelected);
                gotoxy (0, 2);
                _firstError = true;
            } else if (cmd == "file -O" || cmd == "file --open") {
                if (fileSystem("open", input) == true) {
                    mode = "edit";
                    printInfo();
                    printTabs();
                    printText(input, -1, -1, lineSelected, columnSelected);
                } else {
                    clearTerminal();
                }
                _firstError = true;
            } else if (cmd == "clear") {
                clearTerminal();
                _firstError = true;
            } else if (cmd == "exit") {
                exit (0);
            } else if (cmd == "help") {
                showHelpFile("help.txt");
                _firstError = true;
            } else if (cmd == "help -F" || cmd == "help --file") {
                showHelpFile("file_help.txt");
            } else if (cmd == "help -V" || cmd == "help --visual") {
                showHelpFile("visual_mode_help.txt");
            } else if (cmd == "help -S" || cmd == "help --shortcuts") {
                showHelpFile("shortcuts_help.txt");
            } else if (cmd == "") {} else {
                if (_firstError)
                {
                    ColourizePrint("[Command Error]: command not found (type \'help\' for help)\n", 4);
                    _firstError = false;
                } else {
                    ColourizePrint("[Command Error]: command not found\n", 4);
                }
            }
        }
};