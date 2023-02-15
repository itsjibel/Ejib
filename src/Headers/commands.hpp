#include <cmath>
#include "file.hpp"

vector<char> emptyVector;
vector<vector<char>> input;
string mode="command";
int TerminalColumn=0,      TerminalLine=0,
    TerminalColumnTemp=-1, TerminalLineTemp=-1,
    columnSelected=0,      lineSelected=0,
    startPrintLine=0,      startPrintColumn=0;

class CommandLine : public File {
    private:
        bool _firstError=true;
        int8_t temp = 108;

    public:
        void printText(const vector<vector<char>> &text, const int &selectedCharacterStart, const int &selectedCharacterEnd, const int line);
        void printInfo();

        void clearEditFile() {
            columnSelected=0;
            lineSelected=0;
            startPrintLine=0;
            startPrintColumn=0;
            input.clear();
            input.push_back(emptyVector);
        }

        void showHelpFile(string helpFileName) {
            string line;
            ifstream myfile("../helps/" + helpFileName);

            if (myfile.is_open()) {
                while(getline (myfile, line)) {
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
                    #if (defined (_WIN32) || defined (_WIN64))
                    setColor(6);
                    #endif
                    #if (defined (LINUX) || defined (__linux__))
                    setColor(33);
                    #endif
                    cout<<'\t'<<key;
                    #if (defined (_WIN32) || defined (_WIN64))
                    setColor(5);
                    #endif
                    #if (defined (LINUX) || defined (__linux__))
                    setColor(35);
                    #endif
                    cout<<explanation<<'\n';
                }
                myfile.close();
            } else {
                #if (defined (_WIN32) || defined (_WIN64))
                setColor(4);
                #endif
                #if (defined (LINUX) || defined (__linux__))
                setColor(31);
                #endif
                cout << "\tUnable to open \'"<<helpFileName<<"' file!\n"; 
            }
        }

        void commandLine() {
            string cmd;

            #if (defined (_WIN32) || defined (_WIN64))
            setColor(2);
            cout<<"cmd: ";
            #endif
            #if (defined (LINUX) || defined (__linux__))
            setColor(32);
            cout<<"\e[1mcmd: \e[0m";
            #endif
            
            getline (cin, cmd);
            
            if (cmd == "file -N" || cmd == "file --new") {
                mode = "edit";
                clearEditFile();
                #if (defined (_WIN32) || defined (_WIN64))
                system ("cls");
                #endif
                #if (defined (LINUX) || defined (__linux__))
                system ("clear");
                #endif
                printInfo();
                printText(input, -1, -1, -1);
                gotoxy (0, 0);
                _firstError = true;
            } else if (cmd == "file -O" || cmd == "file --open") {
                if (fileSystem("open", input) == true) {
                    mode = "edit";
                    printInfo();
                    printText(input, -1, -1, -1);
                } else {
                    #if (defined (_WIN32) || defined (_WIN64))
                    system ("cls");
                    #endif
                    #if (defined (LINUX) || defined (__linux__))
                    system ("clear");
                    #endif
                }
                _firstError = true;
            } else if (cmd == "clear") {
                #if (defined (_WIN32) || defined (_WIN64))
                system ("cls");
                #endif
                #if (defined (LINUX) || defined (__linux__))
                system ("clear");
                #endif
                _firstError = true;
            } else if (cmd == "exit") {
                exit (0);
            } else if (cmd == "help") {
                showHelpFile("help.txt");
                _firstError = true;
            } else if (cmd == "file -H" || cmd == "file --help") {
                showHelpFile("file_help.txt");
            } else if (cmd == "visual -H" || cmd == "visual --help") {
                showHelpFile("visual_mode_help.txt");
            } else if (cmd == "") {} else {
                if (_firstError) {

                    #if (defined (_WIN32) || defined (_WIN64))
                    setColor(4);
                    #endif
                    #if (defined (LINUX) || defined (__linux__))
                    setColor(31);
                    #endif

                    cout<<"[Command Error]: command not found (type \'help\' for help)\n";
                    _firstError = false;
                } else {

                    #if (defined (_WIN32) || defined (_WIN64))
                    setColor(4);
                    #endif
                    #if (defined (LINUX) || defined (__linux__))
                    setColor(31);
                    #endif

                    cout<<"[Command Error]: command not found\n";
                }
            }
        }
};
void CommandLine::printText(const vector<vector<char>> &text, const int &selectedCharacterStart, const int &selectedCharacterEnd, const int line) {
    int index=0, numberLines[10000] = {0};
    string blankView, lines[10000];
    int range = startPrintLine + TerminalLine - 2 <= text.size() ? startPrintLine + TerminalLine - 2 : text.size();

    ShowConsoleCursor(false);
    if (!(text.size() == 1 && text.at(0).size() == 0))
        for (int i=startPrintLine; i<range; i++) {
            numberLines[i - startPrintLine] = i + 1;
            int countOfCharacters=0;
            for (int j=startPrintColumn; j<text.at(i).size(); j++) {
                if (countOfCharacters > TerminalColumn - 1) {
                    index++;
                    break;
                }
                
                if (text.at(i).at(j) != '\r' && text.at(i).at(j) != '\t' &&
                    text.at(i).at(j) != '\v' && text.at(i).at(j) != '\0' &&
                    text.at(i).at(j) != '\f' && text.at(i).at(j) != '\a' &&
                    text.at(i).at(j) != '\e' && text.at(i).at(j) != '\b')
                    lines[index] += text.at(i).at(j);

                index = j == text.at(i).size() - 1 ? index + 1 : index;
                countOfCharacters++;
            }
            index = countOfCharacters == 0 ? index + 1 : index;
        }
        
    if ((text.at(0).size() > 0 && index == 0))
        index++;

    int biggestNumberLine=0, indexBiggestNumberLine=0;
    for (int i=0; i<TerminalLine - 2; i++)
        if (numberLines[i] > biggestNumberLine) {
            biggestNumberLine = numberLines[i];
            indexBiggestNumberLine = i;
        }
    int numberDigits_Of_LargestLineNumber = floor(log10(biggestNumberLine) + 1);
    gotoxy(0, 0);

    for (int j=0; j<TerminalLine - 2; j++)
        if (j<index) {
            for (int l=lines[j].size() % TerminalColumn; l<TerminalColumn; l++)
                if (!(lines[j].size() % TerminalColumn == 0 && lines[j].size() > 0))
                    lines[j]+=' ';

            while (numberDigits_Of_LargestLineNumber + lines[j].size() + 1 > TerminalColumn)
                lines[j].pop_back();

            if (numberDigits_Of_LargestLineNumber - floor(log10(numberLines[j]) + 1) != 0)
                for (int i=0; i<=numberDigits_Of_LargestLineNumber - floor(log10(numberLines[j]) + 1); i++) {
                    gotoxy (i, j);
                    cout<<" ";
                }

            gotoxy (numberDigits_Of_LargestLineNumber - floor(log10(numberLines[j]) + 1), j);

            #if (defined (_WIN32) || defined (_WIN64))
            setColor(15);
            cout<<numberLines[j];
            #endif

            #if (defined (LINUX) || defined (__linux__))
            cout<<BOLD(FWHT("\e[1m" + to_string(numberLines[j]) + "\e[0m"));
            #endif

            #if (defined (_WIN32) || defined (_WIN64))
            setColor(17);
            #endif
            #if (defined (LINUX) || defined (__linux__))
            setColor(44);
            #endif

            cout<<" ";
            setColor(0);
            colourizeText(lines[j], selectedCharacterStart, selectedCharacterEnd, line, j + startPrintLine);
        } else {
            blankView += "$";
            for (int i=0; i<TerminalColumn - 1; i++) blankView += " ";
        }

    #if (defined (_WIN32) || defined (_WIN64))
    setColor(5);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    setColor(35);
    #endif
    
    cout<<blankView;
    ShowConsoleCursor(true);

    if (text.at(0).size() == 0 && text.size() == 1)
        gotoxy (0, 0);
    else
        gotoxy (columnSelected - startPrintColumn + numberDigits_Of_LargestLineNumber + 1, lineSelected - startPrintLine);
}
void CommandLine::printInfo() {
    ShowConsoleCursor(false);
    gotoxy (0, TerminalLine - 2);
    string modeView;

    for (int i=0; i<TerminalColumn/2-6; i++) modeView+=" ";
    modeView += "-- INSERT --";
    for (int i=0; i<TerminalColumn/2-6; i++) modeView+=" ";

    #if (defined (_WIN32) || defined (_WIN64))
    setColor(7);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    setColor(37);
    #endif
    
    #if (defined (_WIN32) || defined (_WIN64))
    setColor(97);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    setColor(43);
    #endif

    cout<<modeView;
    setColor(0);
    gotoxy (0, TerminalLine - 1);

    #if (defined (_WIN32) || defined (_WIN64))
    setColor(10);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    setColor(32);
    #endif

    cout<<"\e[1mcmd@edit:\e[0m";
    float percentageTextSeen = 100.0 / ((float)input.size()) * ((float)(startPrintLine + TerminalLine - 1)) < 100.0 ? percentageTextSeen = 100.0 / ((float)input.size()) * ((float)(startPrintLine + TerminalLine - 1)) : 100.0;
    percentageTextSeen = percentageTextSeen < 1.0 ? 1.0 : percentageTextSeen;
    gotoxy (TerminalColumn - 14 - (floor(log10(lineSelected + 1) + 1) + floor(log10(columnSelected + 1) + 1) + floor(log10(int(percentageTextSeen)) + 1)), TerminalLine - 1);

    #if (defined (_WIN32) || defined (_WIN64))
    setColor(11);
    cout<<"("<<int(percentageTextSeen)<<"%) ";
    #endif

    #if (defined (LINUX) || defined (__linux__))
    cout<<BOLD(FCYN("("<<int(percentageTextSeen)<<"%) "));
    #endif
    
    #if (defined (_WIN32) || defined (_WIN64))
    setColor(3);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    setColor(36);
    #endif

    cout<<"Ln "<<lineSelected + 1<<", Col "<<columnSelected + 1;
    temp = TerminalColumn - 14 - (floor(log10(lineSelected + 1) + 1) + floor(log10(columnSelected + 1) + 1) + floor(log10(int(percentageTextSeen)) + 1));

    #if (defined (_WIN32) || defined (_WIN64))
    setColor(7);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    setColor(37);
    #endif
    ShowConsoleCursor(true);
}