#include <windows.h>
#include <iostream>
using std::cout;
using std::string;

HANDLE hConsole = GetStdHandle (STD_OUTPUT_HANDLE);
void gotoxy (SHORT x, SHORT y) { SetConsoleCursorPosition (hConsole, COORD {x, y}); }
void ShowConsoleCursor (bool showFlag) {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo (hConsole, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo (hConsole, &cursorInfo);
}
void colourizeText (const string &text, const int &selectedCharacterStart, const int &selectedCharacterEnd, const int &selectedLine, const int &currentLine) {
    string textPart;
    int8_t color=7, tempColor=7;
    int index=0;
    bool sharpArea=false, quotationArea=false, angleBracketsArea=false, commentArea=false, selectedArea=false;
    char temp;
    for (char character : text) {
        if ((index == selectedCharacterStart || index == selectedCharacterEnd) && selectedLine == currentLine) {
            selectedArea = !selectedArea;
            color = 79;
        } else if ((character == '/' || character == '*' || character == '-' ||
                    character == '+' || character == '^' || character == '%' ||
                    character == '=' || character == '<' || character == '>') &&
                    !quotationArea && !commentArea && !selectedArea) {

            if (temp == '/' && character == '/')
                commentArea=true;

            color = 4;
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

            color = 3;
            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;

        } else if ((character == '@' || character == '#' || character == '!' ||
                    character == '~' || character == '&' || character == '|' ||
                    character == '$' || character == '?') && !quotationArea &&
                    !angleBracketsArea && !commentArea && !selectedArea) {

            color = 13;
            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;

        } else if ((character == '0' || character == '1' || character == '2' ||
                    character == '3' || character == '4' || character == '5' ||
                    character == '6' || character == '7' || character == '8' ||
                    character == '9') && !quotationArea && !angleBracketsArea &&
                    !commentArea && !selectedArea) {

            color = 2;
            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;

        } else if ((character == '.' || character == ',' || character == ';' ||
                    character == ':' || character == '/' || character == '\\') &&
                    !quotationArea && !angleBracketsArea && !commentArea &&
                    !selectedArea) {

            color = 9;
            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;

        } else if (character == '\'' || character == '\"' && !angleBracketsArea &&
                   !commentArea && !selectedArea) {

            color = 6;
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

        } else
            if (!sharpArea && !quotationArea && !angleBracketsArea && !commentArea && !selectedArea)
                color = 7;

        sharpArea = character == '#' ? true : sharpArea;

        if (color != tempColor) {
            
            SetConsoleTextAttribute (hConsole, tempColor);
            cout<<textPart;
            textPart.clear();
            textPart += character;
            
        } else textPart += character;

        tempColor = color;
        index++;
        temp=character;

        if (selectedCharacterStart == selectedCharacterEnd)
            selectedArea=false;

    }
    SetConsoleTextAttribute (hConsole, color);
    cout<<textPart;
}