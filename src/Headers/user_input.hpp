#include "commands.hpp"
#include "change_control.hpp"
#if (defined (LINUX) || defined (__linux__))
#include <stdio.h>
#include <thread>
#endif

class Editor: public CommandLine
{
	public:
		void INPUT_CHARACTER (char characterInput, int &line, int &column, vector<vector<char>> &text);
        void BACKSPACE       (int &line, int &column, vector<vector<char>> &text);
        void DELETE          (int &line, int &column, vector<vector<char>> &text);
        void DELETE_LINE     (int &line, int &column, vector<vector<char>> &text);
        void ENTER           (int &line, int &column, vector<vector<char>> &text);
        void TAB             (int &line, int &column, vector<vector<char>> &text);
        void PASTE           (int &line, int &column, vector<vector<char>> &text);

        void UP          (int &line, int &column,  const vector<vector<char>> &text);
        void LEFT        (int &line, int &column,  const vector<vector<char>> &text);
        void DOWN        (int &line, int &column,  const vector<vector<char>> &text);
        void RIGHT       (int &line, int &column,  const vector<vector<char>> &text);
        void QUICK_UP    (int &line, int &column,  const vector<vector<char>> &text);
        void QUICK_LEFT  (int &line, int &column,  const vector<vector<char>> &text);
        void QUICK_DOWN  (int &line, int &column,  const vector<vector<char>> &text);
        void QUICK_RIGHT (int &line, int &column,  const vector<vector<char>> &text);
        
        int  biggestLineNumber();
        bool updateViewport();
        void AdjustingViewportWithSizeOfTerminal();
        void EDIT_SYSTEM();
};

void Editor::INPUT_CHARACTER(char characterInput, int &line, int &column, vector<vector<char>> &text)
{
    if (int(characterInput) > 31 && int(characterInput) < 127)
    {
        RedoStack.clear();
        AddTrackToUndoStack (true, line, column, characterInput, "CharacterInput");

        if (column != text.at(line).size())
        {
            text.at(line).insert (text.at(line).begin() + column, characterInput);
        } else {
            text.at(line).push_back (characterInput);
            
            if (characterInput == '(')
                text.at(line).push_back (')');

            else if (characterInput == '\"')
                text.at(line).push_back ('\"');

            else if (characterInput == '\'')
                text.at(line).push_back ('\'');

            else if (characterInput == '[')
                text.at(line).push_back (']');

            else if (characterInput == '{')
                text.at(line).push_back ('}');
        }

        column++;
    }
}

void Editor::BACKSPACE(int &line, int &column, vector<vector<char>> &text)
{
    int numberLines[10000] = {0}, biggestNumberLine=0,
        range = startPrintLine + TerminalLine - 2 <= input.size() ? startPrintLine + TerminalLine - 2 : input.size();

    for (int i=startPrintLine; i<range; i++)
        numberLines[i - startPrintLine] = i + 1;

    for (int i=0; i<TerminalLine - 2; i++)
        if (numberLines[i] > biggestNumberLine)
            biggestNumberLine = numberLines[i];

    int numberLineDigits = floor(log10(biggestNumberLine) + 1);

    if (column > 0)
    {
        RedoStack.clear();
        AddTrackToUndoStack (false, line, column, text.at(line).at(column - 1), "Backspace");
        text.at(line).erase (text.at(line).begin() + column - 1);
        column--;
    } else {
        if (line > 0)
        {
            RedoStack.clear();
            string cutLineChange;

            for (int i=0; i<text.at(line).size(); i++)
                cutLineChange.push_back (text.at(line).at(i));

            AddTrackToUndoStack (false, line - 1, column, '\n' + cutLineChange, "Backspace");
            line--;
            column = text.at(line).size();
            vector<char> linkToEnd;

            for (int i=0; i<text.at(line).size(); i++)
                linkToEnd.push_back (text.at(line).at(i));

            for (int i=0; i<text.at(line + 1).size(); i++)
                linkToEnd.push_back (text.at(line+1).at(i));

            text.at(line) = linkToEnd;
            text.erase (text.begin() + line + 1);
        } else column=0;
    }
}

void Editor::DELETE(int &line, int &column, vector<vector<char>> &text)
{
    if (text.size() != 0)
    {
        if (text.at(line).size() > column)
        {
            RedoStack.clear();
            AddTrackToUndoStack (false, line, column, text.at(line).at(column), "Delete");
            text.at(line).erase (text.at(line).begin() + column);
        } else {
            RedoStack.clear();
            if (line < text.size() - 1)
            {
                string cutLineChange;

                for (int i=0; i<text.at(line + 1).size(); i++)
                    cutLineChange.push_back (text.at(line + 1).at(i));

                AddTrackToUndoStack (false, line, column, '\n' + cutLineChange, "Delete");

                for (int i=0; i<text.at(line + 1).size(); i++)
                    text.at(line).push_back(text.at(line + 1).at(i));

                text.erase (text.begin() + line + 1);
            }
        }
    }
}

void Editor::DELETE_LINE(int &line, int &column, vector<vector<char>> &text)
{
    if (text.size() > line)
    {
        RedoStack.clear();
        string cutLineChange;

        for (int i=0; i<text.at(line).size(); i++)
            cutLineChange.push_back (text.at(line).at(i));

        if (line >= 1)
            AddTrackToUndoStack (false, line - 1, column, '\n' + cutLineChange, "Delete Line");
        else
            AddTrackToUndoStack (false, 0, column, cutLineChange, "Delete Line");

        text.erase(text.begin() + line);
        column=0;
    }
    
    line = text.size() <= line && line - 1 > 0 ? line - 1 : 0;

    if (text.size() == 0)
        text.push_back(emptyVector);
}

void Editor::ENTER(int &line, int &column, vector<vector<char>> &text)
{
    RedoStack.clear();
    vector<char> part1, part2;

    for (int i=0; i<column; i++)
        part1.push_back (text.at(line).at(i));

    for (int i=column; i<text.at(line).size(); i++)
        part2.push_back (text.at(line).at(i));

    text.at(line) = part1;
    text.insert (text.begin() + line + 1, part2);
    AddTrackToUndoStack (true, line, column, '\n', "Enter");
    line++;
    column=0;
}

void Editor::TAB(int &line, int &column, vector<vector<char>> &text)
{
    RedoStack.clear();

    for (int i = 0; i < 4; i++)
        text.at(line).insert (text.at(line).begin() + column, ' ');

    AddTrackToUndoStack (true, line, column, "    ", "Tab");

    column += 4;
}

void Editor::PASTE(int &line, int &column, vector<vector<char>> &text)
{
    string copiedText;

    if (GetCopiedText (copiedText))
    {
        RedoStack.clear();
        modificationText(copiedText);
        text.push_back(emptyVector);
        AddTrackToUndoStack (true, line, column, copiedText, "Paste");
        vector<char> linkToEnd;

        for (int j=column; j<text.at(line).size(); j++)
            linkToEnd.push_back(text.at(line).at(j));

        for (int j=0; j<linkToEnd.size(); j++)
            text.at(line).pop_back();

        for (int i=0; i<copiedText.size(); i++)
        {
            if (!(copiedText.at(i) == '\n'))
                text.at(line).insert(text.at(line).begin() + column, copiedText.at(i));

            column++;

            if (copiedText.at(i) == '\n' || i == copiedText.size() - 1)
            {
                if (i != copiedText.size() - 1)
                    column=0;
                text.push_back(emptyVector);
                line = i != copiedText.size() - 1 ? line + 1 : line;
            }
        }

        for (char ch : linkToEnd)
            text.at(line).push_back(ch);

        for (int i=0; i<2; i++) text.pop_back();
    }
}

void Editor::UP(int &line, int &column, const vector<vector<char>> &text)
{
    if (line > 0)
    {
        line--;
        column = column > text.at(line).size() ? text.at(line).size() : column;
    }
}

void Editor::LEFT(int &line, int &column, const vector<vector<char>> &text)
{
    column = column - 1 > -1 ? column - 1 : -1;

    if (column < 0)
        if (line > 0)
        {
            line--;
            column = text.at(line).size();
        } else {
            column=0;
        }
}

void Editor::DOWN(int &line, int &column, const vector<vector<char>> &text)
{
    if (line < text.size() - 1)
    {
        line++;
        column = column > text.at(line).size() ? text.at(line).size() : column;
    }
}

void Editor::RIGHT(int &line, int &column, const vector<vector<char>> &text)
{
    column++;
    if (column > text.at(line).size())
        if (line < text.size() - 1)
        {
            line++;
            column = 0;
        } else {
            column = text.at(line).size();
    }
}

void Editor::QUICK_UP(int &line, int &column, const vector<vector<char>> &text)
{
    for (int i=0; i<5; i++) {
        if (line > 0)
        {
            line--;
            column = column > text.at(line).size() ? text.at(line).size() : column;
        }
    }
}

void Editor::QUICK_LEFT(int &line, int &column, const vector<vector<char>> &text)
{
    do
    {
        column = column - 1 > -1 ? column - 1 : -1;
        if (column < 0) {
            if (line > 0)
            {
                line--;
                column = text.at(line).size();
            } else {
                column=0;
            }
            return;
        }
    } while (!IsSeparatorCharacter(text.at(line).at(column)));
}

void Editor::QUICK_DOWN(int &line, int &column, const vector<vector<char>> &text)
{
    for (int i=0; i<5; i++) {
        if (line < text.size() - 1)
        {
            line++;
            column = column > text.at(line).size() ? text.at(line).size() : column;
        }
    }
}

void Editor::QUICK_RIGHT(int &line, int &column, const vector<vector<char>> &text)
{
    do
    {
        column++;
        if (column > text.at(line).size()) {
            if (line < text.size() - 1)
            {
                line++;
                column = 0;
            } else {
                column = text.at(line).size();
            }
            return;
        }
    } while (!IsSeparatorCharacter(text.at(line).at(column - 1)));
}

int Editor::biggestLineNumber()
{
    int numberLines[10000] = {0}, biggestNumberLine=0,
        range = startPrintLine + TerminalLine - 2 <= input.size() ? startPrintLine + TerminalLine - 2 : input.size();

    for (int i=startPrintLine; i<range; i++)
        numberLines[i - startPrintLine] = i + 1;

    for (int i=0; i<TerminalLine - 2; i++)
        if (numberLines[i] > biggestNumberLine)
            biggestNumberLine = numberLines[i];

    return biggestNumberLine;
}

bool Editor::updateViewport()
{
    int biggestNumberLine = biggestLineNumber();
    bool updated=false;
    while (lineSelected + 2 - startPrintLine > TerminalLine - 3) {
        startPrintLine++;
        updated=true;
    }
    while (lineSelected + 2 - startPrintLine < 2) {
        startPrintLine--;
        updated=true;
    }
    while (columnSelected - startPrintColumn + floor(log10(biggestNumberLine) + 1) + 1 > TerminalColumn - 1)
    {
        startPrintColumn++;
        updated=true;
    }
    
    while (columnSelected - startPrintColumn + floor(log10(biggestNumberLine) + 1) + 1 < floor(log10(biggestNumberLine) + 1) + 1)
    {
        startPrintColumn--;
        updated=true;
    }
    return updated;
}

void Editor::AdjustingViewportWithSizeOfTerminal()
{
    while(1)
    {
        bool sizeChanged=false;
        TerminalLine = getTerminaLine_Column().at(0);
        TerminalColumn = getTerminaLine_Column().at(1);

        if ((TerminalColumn != TerminalColumnTemp || TerminalLine != TerminalLineTemp)
            && (mode == "visual" || mode == "edit"))
        {
            updateViewport();
            #if (defined (_WIN32) || defined (_WIN64))
            system("cls");
            #endif
            #if (defined (LINUX) || defined (__linux__))
            system("clear");
            #endif
            printInfo();
            printTabs();
            printText(input, -1, -1, -1);
            sizeChanged=true;
            ShowConsoleCursor(true);
        }

        if (sizeChanged)
        {
            printInfo();
            printTabs();
            printText(input, -1, -1, -1);
        }

        TerminalColumnTemp = TerminalColumn;
        TerminalLineTemp = TerminalLine;
        #if (defined (_WIN32) || defined (_WIN64))
        Sleep(1);
        #endif
        #if (defined (LINUX) || defined (__linux__))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        #endif
    }
}

void Editor::EDIT_SYSTEM()
{
    int ch;
    bool something_happen_in_text_view=false;
    #if (defined (_WIN32) || defined (_WIN64))
    switch (ch = getch())
    {
        case 0:
            case 0xE0:
                switch(ch = getch())
                {
                    case 72:
                        UP(lineSelected, columnSelected, input);
                        something_happen_in_text_view=true;
                        break;
                    case 75:
                        LEFT(lineSelected,    columnSelected, input);
                        something_happen_in_text_view=true;
                        break;
                    case 77:
                        RIGHT(lineSelected,   columnSelected, input);
                        something_happen_in_text_view=true;
                        break;
                    case 80:
                        DOWN(lineSelected, columnSelected, input);
                        something_happen_in_text_view=true;
                        break;
                    case 83:
                        DELETE(lineSelected, columnSelected, input);
                        something_happen_in_text_view=true;
                        break;
                    default:
                        getCharacter(ch, lineSelected, columnSelected, input);
                        something_happen_in_text_view=true;
                }
                break;
                case 8:
                    BACKSPACE(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                case 2:
                    mode = "visual";
                    break;
                case 13:
                    ENTER(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                case 9:
                    TAB(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                case 22:
                    PASTE(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                case 6:
                    mode = "file";
                    fileSystem("save", input);
                    system("clear");
                    printInfo();
                    printTabs();
                    printText(input, -1, -1, -1);
                    mode = "visual";
                    break;
                case 24:
                    DELETE_LINE(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                case 16:
                    mode = "command";
                    SetConsoleTextAttribute(hConsole, 7);
                    break;
                case 21:
                    UNDO(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                case 18:
                    REDO(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                default:
                    INPUT_CHARACTER(ch, lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
    }
    #endif
    #if (defined (LINUX) || defined (__linux__))
    switch (ch = getch())
    {
        case 27:
            switch(ch = getch())
            {
                case 91:
                    switch(ch = getch())
                    {
                        case 65:
                            UP(lineSelected, columnSelected, input);
                            something_happen_in_text_view=true;
                            break;
                        case 68:
                            LEFT(lineSelected, columnSelected, input);
                            something_happen_in_text_view=true;
                            break;
                        case 67:
                            RIGHT(lineSelected, columnSelected, input);
                            something_happen_in_text_view=true;
                            break;
                        case 66:
                            DOWN(lineSelected, columnSelected, input);
                            something_happen_in_text_view=true;
                            break;
                        case 51:
                            switch(ch = getch())
                            {
                                case 126: 
                                    DELETE(lineSelected, columnSelected, input);
                                    something_happen_in_text_view=true;
                                    break;
                                default: ;
                            }
                            break;
                        case 49:
                            switch(ch = getch())
                            {
                                case 59: 
                                    switch(ch = getch())
                                    {
                                        case 53:
                                            switch(ch = getch())
                                            {
                                                case 68:
                                                    QUICK_LEFT(lineSelected, columnSelected, input);
                                                    something_happen_in_text_view=true;
                                                    break;
                                                case 67:
                                                    QUICK_RIGHT(lineSelected, columnSelected, input);
                                                    something_happen_in_text_view=true;
                                                    break;
                                                case 65:
                                                    QUICK_UP(lineSelected, columnSelected, input);
                                                    something_happen_in_text_view=true;
                                                    break;
                                                case 66:
                                                    QUICK_DOWN(lineSelected, columnSelected, input);
                                                    something_happen_in_text_view=true;
                                                    break;
                                                default: ;
                                            }
                                            break;
                                        default: ;
                                    }
                                    break;
                                default: ;
                            }
                            break;
                        default: ;
                    }
                    break;
                default: ;
            }
            break;
        case 127:
            BACKSPACE(lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        case 2:
            mode = "visual";
            break;
        case 10:
            ENTER(lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        case 9:
            TAB(lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        case 22:
            PASTE(lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        case 6:
            mode = "file";
            fileSystem("save", input);
            system("clear");
            printInfo();
            printTabs();
            printText(input, -1, -1, -1);
            mode = "edit";
            break;
        case 24:
            DELETE_LINE(lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        case 16:
            mode = "command";
            system("clear");
            setColor(37);
            break;
        case 21:
            UNDO(1, lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        case 18:
            REDO(1, lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        case 25:
            UNDO(5, lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        case 5:
            REDO(5, lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        default:
            INPUT_CHARACTER(ch, lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
    }
    #endif

    if (updateViewport())
        something_happen_in_text_view=true;

    if (something_happen_in_text_view)
    {
        printInfo();
        printText(input, -1, -1, -1);
    }
}