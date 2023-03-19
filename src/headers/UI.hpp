/* <<Ejib Interface>>
 * A summary of what this library does:
 * This library does all the work for display to the user,
 * so if you are looking for display functions, this library can help you.
 */
#include <cmath>
#include "consumption_info.hpp"
/// Global variables that almost every function and object should have access to
vector<vector<char>> mainText;
string currentMode = "command";
int numberOfTerminalColumn = 0, numberOfTerminalLine = 0, currentColumn = 0,
    currentLine = 0, startLineForDisplayPage = 0, startColumnForDisplayPage = 0;

class EditorUI
{
    private:
        void colourizeText(const string &text, const int &selectedCharacterStart, const int &selectedCharacterEnd,
                           const int &selectedLine, const int &currentLine, const int &column);
        unsigned int GetBiggestLineNumberInViewport(vector<vector<char>> &text, int startLineForDisplayPage);

    public:
        void displayPageOfText(const vector<vector<char>> &text, const int &selectedCharacterStart,
                               const int &selectedCharacterEnd);
        void printTabs();
        void displayLocationInfo();
};

unsigned int EditorUI::GetBiggestLineNumberInViewport(vector<vector<char>> &text, int startLineForDisplayPage)
{
    unsigned int BiggestLineNumber = 1;
    vector<int> LineNumbers;
    /// This range specifies the last line of the viewport
    int range = startLineForDisplayPage + numberOfTerminalLine - 4 <= text.size() ?\
                startLineForDisplayPage + numberOfTerminalLine - 4\
                : text.size();
    /// Saving all line numbers to find the biggest one
    for (int i=startLineForDisplayPage; i<range; i++)
        LineNumbers.push_back(i + 1);
    /// Finding the biggest number line and putting it into 'BiggestLineNumber'
    for (int i=0; i<LineNumbers.size(); i++)
        if (LineNumbers[i] > BiggestLineNumber)
            BiggestLineNumber = LineNumbers[i];
    return BiggestLineNumber;
}

void EditorUI::colourizeText (const string &text, const int &selectedCharacterStart, const int &selectedCharacterEnd,
                              const int &selectedLine, const int &numberLineForPrint, const int &column)
{
    string textPart;
    int8_t color = 7, tempColor = 7;
    int index = 0;
    bool sharpArea=false,   quotationArea=false, angleBracketsArea=false,
         commentArea=false, selectedArea=false;
    char temp;

    for (char character : text)
    {
        if ((index == selectedCharacterStart || index == selectedCharacterEnd)
            && selectedLine == currentLine)
        {
            selectedArea = !selectedArea;
            color = 79;
        } else if ((character == '/' || character == '*' || character == '-' ||
                    character == '+' || character == '^' || character == '%' ||
                    character == '=' || character == '<' || character == '>') &&
                    !quotationArea && !commentArea && !selectedArea) {

            commentArea = temp == '/' && character == '/' ? true : commentArea;
            color = 4;
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
        }
        else if ((character == '(' || character == ')' || character == '{' ||
                    character == '}' || character == '[' || character == ']') &&
                    !quotationArea && !angleBracketsArea && !commentArea &&
                    !selectedArea)
        {
            color = 11;
            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        }
        else if ((character == '@' || character == '#' || character == '!' ||
                    character == '~' || character == '&' || character == '|' ||
                    character == '$' || character == '?') && !quotationArea &&
                    !angleBracketsArea && !commentArea && !selectedArea)
        {
            color = 13;
            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        }
        else if ((character == '0' || character == '1' || character == '2' ||
                    character == '3' || character == '4' || character == '5' ||
                    character == '6' || character == '7' || character == '8' ||
                    character == '9') && !quotationArea && !angleBracketsArea &&
                    !commentArea && !selectedArea)
        {
            color = 2;
            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        }
        else if ((character == '.' || character == ',' || character == ';' ||
                    character == ':' || character == '/' || character == '\\') &&
                    !quotationArea && !angleBracketsArea && !commentArea &&
                    !selectedArea)
        {
            color = 9;
            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        }
        else if (character == '\'' || character == '\"' && !angleBracketsArea &&
                !commentArea && !selectedArea)
        {
            color = 6;
            sharpArea=false;

            if (character == '\"' || character == '\'')
                if (quotationArea)
                    quotationArea = false;
                else
                    for (int i=index + 1; i<text.size(); i++)
                        if (text.at(i) == '\"' || text.at(i) == '\'')
                        {
                            quotationArea = !quotationArea;
                            break;
                        }
        } else if (!sharpArea && !quotationArea && !angleBracketsArea &&
                   !commentArea && !selectedArea)
            color = 7;

        sharpArea = character == '#' ? true : sharpArea;

        if (color != tempColor)
        {
            ColorPrint(textPart, tempColor);
            textPart.clear();
            textPart += character;
        } else textPart += character;

        tempColor = color;
        index++;
        temp=character;

        if (selectedCharacterStart == selectedCharacterEnd)
            selectedArea=false;

    }
    ColorPrint(textPart, color);
}

void EditorUI::printTabs()
{
    /// Display the name of the current file
    ShowConsoleCursor(false);
    gotoxy (0, 0);
    cout<<"  ";
    #if (defined (_WIN32) || defined (_WIN64))
    ColorPrint(' ', 91);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    ColorPrint(' ', 44);
    #endif
    ColorPrint(" \e[1m" + CurrentFileName + "\e[0m ", 7);
    #if (defined (_WIN32) || defined (_WIN64))
    ColorPrint(' ', 91);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    ColorPrint(' ', 44);
    #endif
    /* Clearing the front of the current file name,
     * for that if the user resizes the terminal,
     * the characters don't remain in front of the current file name
     */ 
    for (int i=18 + CurrentFileName.size(); i<numberOfTerminalColumn; i++)
        cout<<' ';
    /// Display text and tab separator bar
    string bar;
    for (int i=0; i<numberOfTerminalColumn; i++) bar+=" ";
    gotoxy(0, 1);
    ColorPrint(bar, 97);
    ShowConsoleCursor(true);
}

void EditorUI::displayPageOfText(const vector<vector<char>> &text, const int &selectedCharacterStart,
                                 const int &selectedCharacterEnd)
{
    int numberOfVisibleLines=0;
    vector<int> visibleNumberLines;
    vector<string> visibleLines;
    int range = startLineForDisplayPage + numberOfTerminalLine - 2 <= text.size() ?\
                startLineForDisplayPage + numberOfTerminalLine - 2 : text.size();

    for (int i=0; i<numberOfTerminalLine; i++)
        visibleLines.push_back("");

    ShowConsoleCursor(false);

    if (!(text.size() == 1 && text.at(0).size() == 0))
    {
        for (int i=startLineForDisplayPage; i<range; i++)
        {
            visibleNumberLines.push_back(i + 1);
            int countOfCharacters=0;
            for (int j=startColumnForDisplayPage; j<text.at(i).size(); j++)
            {
                if (countOfCharacters > numberOfTerminalColumn - 1) 
                {
                    numberOfVisibleLines++;
                    break;
                }

                visibleLines.at(numberOfVisibleLines).push_back(text.at(i).at(j));
                numberOfVisibleLines = j == text.at(i).size() - 1 ? numberOfVisibleLines + 1 : numberOfVisibleLines;
                countOfCharacters++;
            }
            numberOfVisibleLines = countOfCharacters == 0 ? numberOfVisibleLines + 1 : numberOfVisibleLines;
        }
    }
   
    if ((text.at(0).size() > 0 && numberOfVisibleLines == 0))
        numberOfVisibleLines++;

    int numberDigits_Of_LargestLineNumber = floor(log10(GetBiggestLineNumberInViewport(mainText, startLineForDisplayPage)) + 1);
    gotoxy(0, 2);
    for (int j=0; j<numberOfTerminalLine - 4; j++)
    {
        if (j < numberOfVisibleLines)
        {
            for (int l=visibleLines[j].size() % numberOfTerminalColumn; l<numberOfTerminalColumn; l++)
                if (!(visibleLines[j].size() % numberOfTerminalColumn == 0 && visibleLines[j].size() > 0))
                    visibleLines[j]+=' ';

            while (numberDigits_Of_LargestLineNumber + visibleLines[j].size() + 1 > numberOfTerminalColumn)
                visibleLines[j].pop_back();

            if (numberDigits_Of_LargestLineNumber - floor(log10(visibleNumberLines[j]) + 1) != 0)
                for (int i=0; i<=numberDigits_Of_LargestLineNumber - floor(log10(visibleNumberLines[j]) + 1); i++)
                {
                    gotoxy (i, j + 2);
                    cout<<" ";
                }

            gotoxy (numberDigits_Of_LargestLineNumber - floor(log10(visibleNumberLines[j]) + 1), j + 2);

            #if (defined (_WIN32) || defined (_WIN64))
            if (j + startLineForDisplayPage == line)
                ColorPrint(visibleNumberLines[j], 15);
            else
                ColorPrint(visibleNumberLines[j], 8);
            #endif

            #if (defined (LINUX) || defined (__linux__))
            if (j + startLineForDisplayPage == currentLine)
                cout<<BOLD(FWHT("\e[1m" + to_string(visibleNumberLines[j]) + "\e[0m"));
            else
                ColorPrint("\e[1m" + to_string(visibleNumberLines[j]) + "\e[0m", 90);
            #endif

            #if (defined (_WIN32) || defined (_WIN64))
            ColorPrint(' ', 91);
            #endif
            #if (defined (LINUX) || defined (__linux__))
            ColorPrint(' ', 44);
            #endif

            colourizeText(visibleLines[j], selectedCharacterStart, selectedCharacterEnd, currentLine, j + startLineForDisplayPage, currentColumn);
        } else {
            string blankLine = "~";
            for (int i=0; i<numberOfTerminalColumn - 1; i++) blankLine += " ";
            ColorPrint(blankLine, 6);
        }
    }

    ShowConsoleCursor(true);

    if (text.at(0).size() == 0 && text.size() == 1)
        gotoxy (0, 2);
    else
        gotoxy (currentColumn - startColumnForDisplayPage + numberDigits_Of_LargestLineNumber + 1,
                currentLine - startLineForDisplayPage + 2);
}

void EditorUI::displayLocationInfo()
{
    /// Display the location information such as the column selected, line selected etc.
    ShowConsoleCursor(false);
    /// Display current mode bar
    string currentModeView;
    for (int i=0; i<numberOfTerminalColumn / 2 - 6; i++) currentModeView += " ";
    currentModeView += "-- INSERT --";
    for (int i=0; i<numberOfTerminalColumn / 2 - 6; i++) currentModeView += " ";
    /// Display cmd text for visual mode
    gotoxy (0, numberOfTerminalLine - 2);
    setColor(7);
    ColorPrint(currentModeView, 97);
    gotoxy (0, numberOfTerminalLine - 1);
    ColorPrint("\e[1mcmd@edit:\e[0m", 2);
    /// Calculate the percentage of text read at this moment
    float percentageTextSeen = 100.0/mainText.size() * (startLineForDisplayPage + numberOfTerminalLine - 1) < 100.0 ?\
          percentageTextSeen = 100.0/mainText.size() * (startLineForDisplayPage + numberOfTerminalLine - 1) : 100.0;
    percentageTextSeen = percentageTextSeen < 1.0 ? 1.0 : percentageTextSeen;
    /// This variable specifies from which column we should display the information about the current location
    int columnStartPrintInfoOfCurrentLocation =
        numberOfTerminalColumn - 12 - (floor(log10(currentLine + 1) + 1) +
                                       floor(log10(currentColumn + 1) + 1) +
                                       floor(log10(int(percentageTextSeen)) + 1));
    /// Now we go to the same column to display the information
    gotoxy (columnStartPrintInfoOfCurrentLocation,
            numberOfTerminalLine - 1);

    ColorPrint(to_string(int(percentageTextSeen)) + "% ", 12);
    ColorPrint("Ln " + to_string(currentLine + 1) + ", Col " + to_string(currentColumn + 1), 3);
    /* Clearing the back of the location information for that if the user resizes the terminal,
     * the characters don't remain in the back of the location information
     */
    gotoxy(9, numberOfTerminalLine - 1);
    for (int i=9; i<columnStartPrintInfoOfCurrentLocation; i++)
        cout<<' ';
    ShowConsoleCursor(true);
}