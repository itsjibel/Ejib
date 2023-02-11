#include <iostream>
#include <string>
using std::cout;
using std::string;
using std::to_string;
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
void colourizeText (const string &text, const int &selectedCharacterStart, const int &selectedCharacterEnd, const int &selectedLine, const int &currentLine) {
    string textPart;
    int8_t color=37, tempColor=37;
    int index=0;
    bool sharpArea=false, quotationArea=false, angleBracketsArea=false, commentArea=false, selectedArea=false;
    char temp;
    for (char character : text) {
        if ((index == selectedCharacterStart || index == selectedCharacterEnd) && selectedLine == currentLine) {
            selectedArea = !selectedArea;
            color = 41;
        } else if ((character == '/' || character == '*' || character == '-' ||
                    character == '+' || character == '^' || character == '%' ||
                    character == '=' || character == '<' || character == '>') &&
                    !quotationArea && !commentArea && !selectedArea) {

            if (temp == '/' && character == '/')
                commentArea=true;

            color = 31;
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

            color = 36;
            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;

        } else if ((character == '@' || character == '#' || character == '!' ||
                    character == '~' || character == '&' || character == '|' ||
                    character == '$' || character == '?') && !quotationArea &&
                    !angleBracketsArea && !commentArea && !selectedArea) {

            color = 35;
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

            color = 34;
            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;

        } else if (character == '\'' || character == '\"' && !angleBracketsArea &&
                !commentArea && !selectedArea) {

            color = 33;
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
                color = 37;

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