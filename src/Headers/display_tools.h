#include <iostream>
#include <limits.h>
#include <string>
#include <X11/Xlib.h>

#if (defined (LINUX) || defined (__linux__))
#define RST  "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST
#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST
#endif

using std::cout;
using std::string;
using std::to_string;

#if (defined (_WIN32) || defined (_WIN64))
#include <windows.h>

HANDLE hConsole = GetStdHandle (STD_OUTPUT_HANDLE);
void gotoxy (SHORT x, SHORT y) { SetConsoleCursorPosition (hConsole, COORD {x, y}); }

void ShowConsoleCursor (bool showFlag) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo (hConsole, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo (hConsole, &cursorInfo);
}

void setColor(int textColor) {
    SetConsoleTextAttribute (hConsole, textColor);
}
#endif

#if (defined (LINUX) || defined (__linux__))
void gotoxy(int x, int y) {
    printf("%c[%d;%df", 0x1B, y + 1, x + 1);
}

void setColor(int textColor) {
    cout<<"\033[" + to_string (textColor) + "m";
}

void ShowConsoleCursor (bool showFlag) {
    if (showFlag)
        printf("\e[?25h");
    else
        printf("\e[?25l");
}
#endif

void colourizeText (const string &text, const int &selectedCharacterStart, const int &selectedCharacterEnd, const int &selectedLine, const int &currentLine) {
    string textPart;
    #if (defined (_WIN32) || defined (_WIN64))
    int8_t color=7, tempColor=7;
    #endif
    #if (defined (LINUX) || defined (__linux__))
    int8_t color=37, tempColor=37;
    #endif
    int index=0;
    bool sharpArea=false, quotationArea=false, angleBracketsArea=false, commentArea=false, selectedArea=false;
    char temp;
    for (char character : text) {
        if ((index == selectedCharacterStart || index == selectedCharacterEnd) && selectedLine == currentLine) {
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

            if (character == '<') {
                for (int i=index; i<text.size(); i++)
                    if (text.at(i) == '>') {
                        angleBracketsArea = angleBracketsArea ? false : true;
                        break;
                    }
            } else if (character == '>') angleBracketsArea=false;

            quotationArea=false;
        } else if ((character == '(' || character == ')' || character == '{' ||
                    character == '}' || character == '[' || character == ']') &&
                    !quotationArea && !angleBracketsArea && !commentArea &&
                    !selectedArea) {

            #if (defined (_WIN32) || defined (_WIN64))
            color = 3;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 36;
            #endif

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
                        if (text.at(i) == '\"' || text.at(i) == '\'') {
                            quotationArea = !quotationArea;
                            break;
                        }

            }

        } else {
            if (!sharpArea && !quotationArea && !angleBracketsArea && !commentArea && !selectedArea) {
                #if (defined (_WIN32) || defined (_WIN64))
                color = 7;
                #endif
                #if (defined (LINUX) || defined (__linux__))
                color = 37;
                #endif
            }
        }
        sharpArea = character == '#' ? true : sharpArea;

        if (color != tempColor) {
            setColor(tempColor);
            cout<<textPart;
            setColor(0);
            textPart.clear();
            textPart += character;
        } else textPart += character;

        tempColor = color;
        index++;
        temp=character;

        if (selectedCharacterStart == selectedCharacterEnd)
            selectedArea=false;

    }
    setColor(color);
    cout<<textPart;
    setColor(0);
}