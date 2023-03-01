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

class EditorUI
{
    private:
        void colourizeText (const string &text, const int &selectedCharacterStart, const int &selectedCharacterEnd,
                    const int &selectedLine, const int &currentLine, const int &column);

    public:
        void printText(const vector<vector<char>> &text, const int &selectedCharacterStart,
                        const int &selectedCharacterEnd,  const int &line, const int column);
        void printTabs();
        void printInfo();
};

void EditorUI::colourizeText (const string &text, const int &selectedCharacterStart, const int &selectedCharacterEnd,
                    const int &selectedLine, const int &currentLine, const int &column)
{
    string textPart;
    int8_t color=7, tempColor=7;
    int index=0;
    bool sharpArea=false,   quotationArea=false, angleBracketsArea=false,
         commentArea=false, selectedArea=false;
    char temp;
    for (char character : text)
    {
        if ((index == selectedCharacterStart || index == selectedCharacterEnd)
            && selectedLine == currentLine)
        {
            selectedArea = !selectedArea;
            #if (defined (_WIN32) || defined (_WIN64))
            color = 79;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 41;
            #endif
        } else if ((character == '/' || character == '*' || character == '-' ||
                    character == '+' || character == '^' || character == '%' ||
                    character == '=' || character == '<' || character == '>') &&
                    !quotationArea && !commentArea && !selectedArea) {

            if (temp == '/' && character == '/')
                commentArea=true;

            #if (defined (_WIN32) || defined (_WIN64))
            color = 4;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 31;
            #endif

            sharpArea=false;

            if (character == '<')
            {
                for (int i=index; i<text.size(); i++)
                    if (text.at(i) == '>')
                    {
                        angleBracketsArea = angleBracketsArea ? false : true;
                        break;
                    }
            } else if (character == '>') angleBracketsArea=false;

            quotationArea=false;
        } else if ((character == '(' || character == ')' || character == '{' ||
                    character == '}' || character == '[' || character == ']') &&
                    !quotationArea && !angleBracketsArea && !commentArea &&
                    !selectedArea) {
            color = 11;
            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        } else if ((character == '@' || character == '#' || character == '!' ||
                    character == '~' || character == '&' || character == '|' ||
                    character == '$' || character == '?') && !quotationArea &&
                    !angleBracketsArea && !commentArea && !selectedArea) {

            #if (defined (_WIN32) || defined (_WIN64))
            color = 13;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 35;
            #endif

            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        } else if ((character == '0' || character == '1' || character == '2' ||
                    character == '3' || character == '4' || character == '5' ||
                    character == '6' || character == '7' || character == '8' ||
                    character == '9') && !quotationArea && !angleBracketsArea &&
                    !commentArea && !selectedArea) {

            #if (defined (_WIN32) || defined (_WIN64))
            color = 2;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 32;
            #endif

            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        } else if ((character == '.' || character == ',' || character == ';' ||
                    character == ':' || character == '/' || character == '\\') &&
                    !quotationArea && !angleBracketsArea && !commentArea &&
                    !selectedArea) {

            #if (defined (_WIN32) || defined (_WIN64))
            color = 9;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 34;
            #endif

            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        } else if (character == '\'' || character == '\"' && !angleBracketsArea &&
                !commentArea && !selectedArea) {

            #if (defined (_WIN32) || defined (_WIN64))
            color = 6;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 33;
            #endif

            sharpArea=false;
            if (character == '\"' || character == '\'') {

                if (quotationArea)
                    quotationArea = false;
                else
                    for (int i=index + 1; i<text.size(); i++)
                        if (text.at(i) == '\"' || text.at(i) == '\'')
                        {
                            quotationArea = !quotationArea;
                            break;
                        }

            }

        } else {
            if (!sharpArea && !quotationArea && !angleBracketsArea && !commentArea && !selectedArea)
            {
                #if (defined (_WIN32) || defined (_WIN64))
                color = 7;
                #endif
                #if (defined (LINUX) || defined (__linux__))
                color = 37;
                #endif
            }
        }
        sharpArea = character == '#' ? true : sharpArea;

        if (color != tempColor)
        {
            ColourizePrint(textPart, tempColor);
            textPart.clear();
            textPart += character;
        } else textPart += character;

        tempColor = color;
        index++;
        temp=character;

        if (selectedCharacterStart == selectedCharacterEnd)
            selectedArea=false;

    }
    ColourizePrint(textPart, color);
}

void EditorUI::printTabs()
{
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

void EditorUI::printText(const vector<vector<char>> &text, const int &selectedCharacterStart,
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

void EditorUI::printInfo()
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