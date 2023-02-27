#include <cmath>
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
                    setColor(6);
                    cout<<'\t'<<key;
                    setColor(5);
                    cout<<explanation<<'\n';
                }
                myfile.close();
            } else {
                setColor(4);
                cout << "\tUnable to open \'"<<helpFileName<<"' file!\n"; 
            }
        }

        void COMMAND_LINE()
        {
            string cmd;
            setColor(2);

            #if (defined (_WIN32) || defined (_WIN64))
            cout<<"cmd: ";
            #endif
            #if (defined (LINUX) || defined (__linux__))
            cout<<"\e[1mcmd: \e[0m";
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
                    setColor(4);
                    cout<<"[Command Error]: command not found (type \'help\' for help)\n";
                    _firstError = false;
                } else {
                    setColor(4);
                    cout<<"[Command Error]: command not found\n";
                }
            }
        }
};

void CommandLine::printTabs() {
    gotoxy (0, 0);
    cout<<"  ";
    #if (defined (_WIN32) || defined (_WIN64))
    setColor(91);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    setColor(44);
    #endif
    cout<<"  ";
    setColor(0);
    cout<<" \e[1m"<<fileName<<"\e[0m ";
    #if (defined (_WIN32) || defined (_WIN64))
    setColor(91);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    setColor(44);
    #endif
    cout<<"  ";
    setColor(0);

    for (int i=8 + fileName.size(); i<TerminalColumn; i++)
        cout<<" ";

    string bar;
    for (int i=0; i<TerminalColumn; i++) bar+=" ";
    gotoxy(0, 1);
    #if (defined (_WIN32) || defined (_WIN64))
    setColor(97);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    setColor(44);
    #endif
    cout<<bar;
    setColor(0);
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
            if (j + startPrintColumn == line)
                setColor(15);
            else
                setColor(8);
            cout<<numberLines[j];
            #endif
            #if (defined (LINUX) || defined (__linux__))
            if (j + startPrintColumn == line)
                cout<<BOLD(FWHT("\e[1m" + to_string(numberLines[j]) + "\e[0m"));
            else {
                setColor(90);
                cout<<"\e[1m" + to_string(numberLines[j]) + "\e[0m";
            }
            #endif

            #if (defined (_WIN32) || defined (_WIN64))
            setColor(91);
            #endif
            #if (defined (LINUX) || defined (__linux__))
            setColor(44);
            #endif

            cout<<" ";
            setColor(0);
            colourizeText(lines[j], selectedCharacterStart, selectedCharacterEnd, line, j + startPrintLine, column);
        } else {
            blankView += "~";
            for (int i=0; i<TerminalColumn - 1; i++) blankView += " ";
        }
    setColor(6);
    cout<<blankView;
    ShowConsoleCursor(true);

    gotoxy(10, 10);
    cout<<"-"<<NumberOfCloseScopes<<"|"<<open<<"-";

    if (text.at(0).size() == 0 && text.size() == 1)
        gotoxy (0, 2);
    else
        gotoxy (columnSelected - startPrintColumn + numberDigits_Of_LargestLineNumber + 1,
                lineSelected - startPrintLine + 2);
}

void CommandLine::printInfo()
{
    ShowConsoleCursor(false);
    gotoxy (0, TerminalLine - 2);
    string modeView;
    
    for (int i=0; i<TerminalColumn/2-6; i++) modeView+=" ";
    modeView += "-- INSERT --";
    for (int i=0; i<TerminalColumn/2-5; i++) modeView+=" ";

    setColor(7);
    #if (defined (_WIN32) || defined (_WIN64))
    setColor(97);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    setColor(44);
    #endif

    cout<<modeView;
    setColor(0);
    gotoxy (0, TerminalLine - 1);
    setColor(2);

    cout<<"\e[1mcmd@edit:\e[0m";
    float percentageTextSeen = 100.0 / input.size() * (startPrintLine + TerminalLine - 1) < 100.0 ?\
          percentageTextSeen = 100.0 / input.size() * (startPrintLine + TerminalLine - 1) : 100.0;

    percentageTextSeen = percentageTextSeen < 1.0 ? 1.0 : percentageTextSeen;
    gotoxy (TerminalColumn - 14 - (floor(log10(lineSelected + 1) + 1) + floor(log10(columnSelected + 1) + 1) + floor(log10(int(percentageTextSeen)) + 1)),
            TerminalLine - 1);

    #if (defined (_WIN32) || defined (_WIN64))
    setColor(11);
    cout<<"("<<int(percentageTextSeen)<<"%) ";
    #endif

    #if (defined (LINUX) || defined (__linux__))
    cout<<BOLD(FCYN(" ("<<int(percentageTextSeen)<<"%) "));
    #endif
    setColor(3);
    cout<<"Ln "<<lineSelected + 1<<", Col "<<columnSelected + 1;
    ShowConsoleCursor(true);
}