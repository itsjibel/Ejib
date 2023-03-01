#include <cmath>
#include "consumption_info.hpp"
#include "file.hpp"

vector<char> emptyVector;
vector<vector<char>> input;
string mode="command";
int TerminalColumn=0,      TerminalLine=0,
    TerminalColumnTemp=-1, TerminalLineTemp=-1,
    columnSelected=0,      lineSelected=0,
    startPrintLine=0,      startPrintColumn=0;

class CommandLine : public File
{
    private:
        bool _firstError=true;

    public:
        void printText(const vector<vector<char>> &text, const int &selectedCharacterStart,
                       const int &selectedCharacterEnd,  const int &line, const int column);
        void printTabs();
        void printInfo();

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

void CommandLine::printTabs() {
    gotoxy (0, 0);
    cout<<"  ";
    #if (defined (_WIN32) || defined (_WIN64))
    ColourizePrint(' ', 91);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    ColourizePrint(' ', 44);
    #endif
    ColourizePrint(" \e[1m" + fileName + "\e[0m ", 7);
    #if (defined (_WIN32) || defined (_WIN64))
    ColourizePrint(' ', 91);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    ColourizePrint(' ', 44);
    #endif

    for (int i=18 + fileName.size(); i<TerminalColumn; i++)
        cout<<' ';

    string bar;
    for (int i=0; i<TerminalColumn; i++) bar+=" ";
    gotoxy(0, 1);
    #if (defined (_WIN32) || defined (_WIN64))
    ColourizePrint(bar, 97);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    ColourizePrint(bar, 44);
    #endif
}

void CommandLine::printText(const vector<vector<char>> &text, const int &selectedCharacterStart,
                            const int &selectedCharacterEnd,  const int &line, const int column)
{
    int VisableLines=0, numberLines[10000] = {0};
    string blankView, lines[10000];
    int range = startPrintLine + TerminalLine - 2 <= text.size() ? startPrintLine + TerminalLine - 2 : text.size();

    ShowConsoleCursor(false);
    if (!(text.size() == 1 && text.at(0).size() == 0))
        for (int i=startPrintLine; i<range; i++)
        {
            numberLines[i - startPrintLine] = i + 1;
            int countOfCharacters=0;
            for (int j=startPrintColumn; j<text.at(i).size(); j++)
            {
                if (countOfCharacters > TerminalColumn - 1) 
                {
                    VisableLines++;
                    break;
                }
                lines[VisableLines] += text.at(i).at(j);
                VisableLines = j == text.at(i).size() - 1 ? VisableLines + 1 : VisableLines;
                countOfCharacters++;
            }
            VisableLines = countOfCharacters == 0 ? VisableLines + 1 : VisableLines;
        }
        
    if ((text.at(0).size() > 0 && VisableLines == 0))
        VisableLines++;

    int biggestNumberLine=0, indexBiggestNumberLine=0;

    for (int i=0; i<TerminalLine - 2; i++)
        if (numberLines[i] > biggestNumberLine)
        {
            biggestNumberLine = numberLines[i];
            indexBiggestNumberLine = i;
        }

    int numberDigits_Of_LargestLineNumber = floor(log10(biggestNumberLine) + 1);
    gotoxy(0, 2);
    NumberOfOpenScopes=0;
    for (int j=0; j<TerminalLine - 4; j++)
        if (j<VisableLines)
        {
            for (int l=lines[j].size() % TerminalColumn; l<TerminalColumn; l++)
                if (!(lines[j].size() % TerminalColumn == 0 && lines[j].size() > 0))
                    lines[j]+=' ';

            while (numberDigits_Of_LargestLineNumber + lines[j].size() + 1 > TerminalColumn)
                lines[j].pop_back();

            if (numberDigits_Of_LargestLineNumber - floor(log10(numberLines[j]) + 1) != 0)
                for (int i=0; i<=numberDigits_Of_LargestLineNumber - floor(log10(numberLines[j]) + 1); i++)
                {
                    gotoxy (i, j + 2);
                    cout<<" ";
                }

            gotoxy (numberDigits_Of_LargestLineNumber - floor(log10(numberLines[j]) + 1), j + 2);

            #if (defined (_WIN32) || defined (_WIN64))
            if (j + startPrintLine == line)
                ColourizePrint(numberLines[j], 15);
            else
                ColourizePrint(numberLines[j], 8);
            #endif
            #if (defined (LINUX) || defined (__linux__))
            if (j + startPrintLine == line)
                cout<<BOLD(FWHT("\e[1m" + to_string(numberLines[j]) + "\e[0m"));
            else {
                ColourizePrint("\e[1m" + to_string(numberLines[j]) + "\e[0m", 90);
            }
            #endif

            #if (defined (_WIN32) || defined (_WIN64))
            ColourizePrint(' ', 91);
            #endif
            #if (defined (LINUX) || defined (__linux__))
            ColourizePrint(' ', 44);
            #endif
            colourizeText(lines[j], selectedCharacterStart, selectedCharacterEnd, line, j + startPrintLine, column);
        } else {
            blankView += "~";
            for (int i=0; i<TerminalColumn - 1; i++) blankView += " ";
        }
    ColourizePrint(blankView, 6);
    ShowConsoleCursor(true);
    if (text.at(0).size() == 0 && text.size() == 1)
        gotoxy (0, 2);
    else
        gotoxy (columnSelected - startPrintColumn + numberDigits_Of_LargestLineNumber + 1,
                lineSelected - startPrintLine + 2);
}

void CommandLine::printInfo()
{
    ShowConsoleCursor(false);
    string modeView;
    
    for (int i=0; i<TerminalColumn/2-6; i++) modeView+=" ";
    modeView += "-- INSERT --";
    for (int i=0; i<TerminalColumn/2-5; i++) modeView+=" ";

    gotoxy (0, TerminalLine - 2);
    setColor(7);
    #if (defined (_WIN32) || defined (_WIN64))
    ColourizePrint(modeView, 97);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    ColourizePrint(modeView, 44);
    #endif
    gotoxy (0, TerminalLine - 1);
    ColourizePrint("\e[1mcmd@edit:\e[0m", 2);

    float percentageTextSeen = 100.0 / input.size() * (startPrintLine + TerminalLine - 1) < 100.0 ?\
          percentageTextSeen = 100.0 / input.size() * (startPrintLine + TerminalLine - 1) : 100.0;

    percentageTextSeen = percentageTextSeen < 1.0 ? 1.0 : percentageTextSeen;
    gotoxy (TerminalColumn - 14 - (floor(log10(lineSelected + 1) + 1) + floor(log10(columnSelected + 1) + 1) + floor(log10(int(percentageTextSeen)) + 1)),
            TerminalLine - 1);

    #if (defined (_WIN32) || defined (_WIN64))
    ColourizePrint("(" + int(percentageTextSeen) + "%) ", 11);
    #endif

    #if (defined (LINUX) || defined (__linux__))
    cout<<BOLD(FCYN(" ("<<int(percentageTextSeen)<<"%) "));
    #endif
    ColourizePrint("Ln " + to_string(lineSelected + 1) + ", Col " + to_string(columnSelected + 1), 3);
    ShowConsoleCursor(true);
}