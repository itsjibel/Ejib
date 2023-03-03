#include "commands.hpp"
#if (defined (LINUX) || defined (__linux__))
#include <stdio.h>
#endif

class Editor: public CommandLine
{
    protected:
        struct Track {
            bool isWirte;
            int startActionLine;
            int startActioncolumn;
            string changeString;
            string changeMode;
        };

        vector<Track> UndoStack, RedoStack;

        template <class T>
        void AddTrackToUndoStack(bool isWirte, int startActionLine, int startActionColumn,
                                 T changeString, string changeMode)
        {
            Track TrackForAdd;
            TrackForAdd.changeString = changeString;
            TrackForAdd.startActioncolumn = startActionColumn;
            TrackForAdd.startActionLine = startActionLine;
            TrackForAdd.isWirte = isWirte;
            TrackForAdd.changeMode = changeMode;
            UndoStack.push_back(TrackForAdd);
        }

        Track GetLastUndoTrack()
        {
            return UndoStack.at(UndoStack.size() - 1);
        }

        Track GetLastRedoTrack()
        {
            return RedoStack.at(RedoStack.size() - 1);
        }

	public:
		void INPUT_CHARACTER (char characterInput, int &line, int &column, vector<vector<char>> &text);
        void BACKSPACE       (int &line, int &column, vector<vector<char>> &text, bool USE_FOR_REDO);
        void DELETE          (int &line, int &column, vector<vector<char>> &text, bool USE_FOR_REDO);
        void DELETE_LINE     (int &line, int &column, vector<vector<char>> &text, bool USE_FOR_REDO);
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
        
        void UNDO (int &line, int &column, vector<vector<char>> &text);
        void REDO (int &line, int &column, vector<vector<char>> &text);

        unsigned int GetBiggestLineNumberInViewport();
        bool UpdateViewport();
        void AdjustingViewportWithSizeOfTerminal();
};

void Editor::INPUT_CHARACTER(char characterInput, int &line, int &column, vector<vector<char>> &text)
{
    if (characterInput > 31 && characterInput < 127)
    {
        RedoStack.clear();
        if (mode != "visual")
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

void Editor::BACKSPACE(int &line, int &column, vector<vector<char>> &text, bool USE_FOR_REDO)
{
    if (column > 0)
    {
        if (mode != "visual" && !USE_FOR_REDO)
        {
            RedoStack.clear();
            AddTrackToUndoStack (false, line, column, text.at(line).at(column - 1), "Backspace");
        }
        text.at(line).erase (text.at(line).begin() + column - 1);
        column--;
    } else {
        if (line > 0)
        {
            if (mode != "visual" && !USE_FOR_REDO)
            {
                string LineForAppending;
                for (int i=0; i<text.at(line).size(); i++)
                    LineForAppending.push_back (text.at(line).at(i));
                RedoStack.clear();
                AddTrackToUndoStack (false, line - 1, column, '\n' + LineForAppending, "Backspace");
            }
            line--;
            column = text.at(line).size();
            vector<char> AppendCurrentLineToPreviousLine;

            for (int i=0; i<text.at(line).size(); i++)
                AppendCurrentLineToPreviousLine.push_back (text.at(line).at(i));

            for (int i=0; i<text.at(line + 1).size(); i++)
                AppendCurrentLineToPreviousLine.push_back (text.at(line+1).at(i));

            text.at(line) = AppendCurrentLineToPreviousLine;
            text.erase (text.begin() + line + 1);
        }
    }
}

void Editor::DELETE(int &line, int &column, vector<vector<char>> &text, bool USE_FOR_REDO)
{
    if (text.size() != 0)
    {
        if (text.at(line).size() > column)
        {
            if (mode != "visual" && !USE_FOR_REDO)
            {
                RedoStack.clear();
                AddTrackToUndoStack (false, line, column, text.at(line).at(column), "Delete");
            }
            text.at(line).erase (text.at(line).begin() + column);
        } else {
            if (line < text.size() - 1)
            {
                string AppendCurrentLineToNextLine;

                for (int i=0; i<text.at(line + 1).size(); i++)
                    AppendCurrentLineToNextLine.push_back (text.at(line + 1).at(i));
                if (mode != "visual" && !USE_FOR_REDO)
                {
                    RedoStack.clear();
                    AddTrackToUndoStack (false, line, column, '\n' + AppendCurrentLineToNextLine, "Delete");
                }

                for (int i=0; i<text.at(line + 1).size(); i++)
                    text.at(line).push_back(text.at(line + 1).at(i));

                text.erase (text.begin() + line + 1);
            }
        }
    }
}

void Editor::DELETE_LINE(int &line, int &column, vector<vector<char>> &text, bool USE_FOR_REDO)
{
    if (text.size() > line)
    {
        string CurrentLineForDelete;

        for (int i=0; i<text.at(line).size(); i++)
            CurrentLineForDelete.push_back (text.at(line).at(i));

        if (line >= 1)
        {
            if (mode != "visual" && !USE_FOR_REDO)
            {
                RedoStack.clear();
                AddTrackToUndoStack (false, line - 1, column, '\n' + CurrentLineForDelete, "Delete Line");
            }
        } else {
            if (mode != "visual" && !USE_FOR_REDO)
            {
                RedoStack.clear();
                AddTrackToUndoStack (false, 0, column, CurrentLineForDelete, "Delete Line");
            }
        }
        text.erase(text.begin() + line);
        column=0;
    }
    if (line > 0)
    {
        if (text.size() <= line)
            line--;
    } else
        line=0;

    if (text.size() == 0)
        text.push_back(emptyVector);
}

void Editor::ENTER(int &line, int &column, vector<vector<char>> &text)
{
    RedoStack.clear();
    vector<char> AppentToNextLine;

    for (int i=column; i<text.at(line).size(); i++)
        AppentToNextLine.push_back (text.at(line).at(i));

    for (int i=0; i<AppentToNextLine.size(); i++)
        text.at(line).pop_back();

    text.insert (text.begin() + line + 1, AppentToNextLine);

    if (mode != "visual")
        AddTrackToUndoStack (true, line, column, '\n', "Enter");

    line++;
    column=0;
}

void Editor::TAB(int &line, int &column, vector<vector<char>> &text)
{
    RedoStack.clear();
    text.at(line).insert (text.at(line).begin() + column, {' ', ' ', ' ', ' '});

    if (mode != "visual")
        AddTrackToUndoStack (true, line, column, "    ", "Tab");

    column += 4;
}

void Editor::PASTE(int &line, int &column, vector<vector<char>> &text)
{
    string copiedText;
    vector<char> linkToEndOfPaste;

    for (int i=column; i<text.at(line).size(); i++)
        linkToEndOfPaste.push_back(text.at(line).at(i));
    for (int i=0; i<linkToEndOfPaste.size(); i++)
        text.at(line).pop_back();

    if (GetCopiedText(copiedText))
    {
        RedoStack.clear();
        RemoveTextSpoilerCharacters(copiedText);
        if (text.size() == 0)
            text.insert(text.begin() + line + 1, emptyVector);
        if (mode != "visual")
            AddTrackToUndoStack (true, line, column, copiedText, "Paste");

        for (int i=0; i<copiedText.size(); i++)
        {
            if (!(copiedText.at(i) == '\n'))
                text.at(line).insert(text.at(line).begin() + column, copiedText.at(i));

            column++;

            if (copiedText.at(i) == '\n')
            {
                column=0;
                text.insert(text.begin() + line + 1, emptyVector);
                line++;
            }
        }
    }

    for (int i=0; i<linkToEndOfPaste.size(); i++)
        text.at(line).push_back(linkToEndOfPaste.at(i));
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
        } else
            column=0;
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
        } else
            column = text.at(line).size();
}

void Editor::QUICK_UP(int &line, int &column, const vector<vector<char>> &text)
{
    for (int i=0; i<5; i++)
    {
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
        if (column < 0)
        {
            if (line > 0)
            {
                line--;
                column = text.at(line).size();
            } else
                column=0;
            return;
        }
    } while (!IsSeparatorCharacter(text.at(line).at(column)));
}

void Editor::QUICK_DOWN(int &line, int &column, const vector<vector<char>> &text)
{
    for (int i=0; i<5; i++)
    {
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
        if (column > text.at(line).size())
        {
            if (line < text.size() - 1)
            {
                line++;
                column = 0;
            } else
                column = text.at(line).size();
            return;
        }
    } while (!IsSeparatorCharacter(text.at(line).at(column - 1)));
}

void Editor::UNDO(int &line, int &column, vector<vector<char>> &text)
{
    string tempChangeMode = UndoStack.size() > 0 ? GetLastUndoTrack().changeMode : "";
    do
    {
        if (UndoStack.size() > 0)
        {
            RedoStack.push_back(UndoStack.back());
            column = GetLastUndoTrack().startActioncolumn;
            line   = GetLastUndoTrack().startActionLine;
            bool isMultipleLine=false;

            if (GetLastUndoTrack().isWirte)
            {
                vector<char> lastLineDeleted;
                for (int i=0; i<GetLastUndoTrack().changeString.size(); i++)
                {
                    if (GetLastUndoTrack().changeString.at(i) == '\n')
                    {
                        lastLineDeleted = text.at(line + 1);
                        text.erase (text.begin() + line + 1);
                        isMultipleLine=true;
                    }

                    if (!isMultipleLine)
                        if (text.at(line).size() > 0)
                            text.at(line).erase (text.at(line).begin() + column);
                }

                for (char ch : lastLineDeleted)
                    text.at(line).push_back(ch);

                if (isMultipleLine)
                    if (GetLastUndoTrack().changeMode == "Paste")
                    {
                        int j=GetLastUndoTrack().changeString.size()-1;

                        while (GetLastUndoTrack().changeString.at(j) != '\n')
                        {
                            text.at(line).erase(text.at(line).begin() + column);
                            j--;
                        }
                    }
            } else {
                for (int i=0; i<GetLastUndoTrack().changeString.size(); i++)
                {
                    if (GetLastUndoTrack().changeString.at(i) == '\n')
                    {
                        vector<char> StringToVector;
                        if (GetLastUndoTrack().changeMode != "Delete Line")
                            for (int j=0; j<GetLastUndoTrack().changeString.size() - 1; j++)
                                text.at(line).pop_back();

                        for (char ch : GetLastUndoTrack().changeString)
                            StringToVector.push_back(ch);
                        
                        StringToVector.erase(StringToVector.begin());
                        text.insert (text.begin() + line + 1, StringToVector);

                        if (GetLastUndoTrack().changeMode == "Backspace" ||
                            GetLastUndoTrack().changeMode == "Delete Line")
                            line++;

                        isMultipleLine=true;
                    }
                    if (!isMultipleLine)
                    {
                        if (GetLastUndoTrack().changeMode == "Backspace")
                            text.at(line).insert (text.at(line).begin() + column - 1, GetLastUndoTrack().changeString.at(0));

                        else if (GetLastUndoTrack().changeMode == "Delete")
                            text.at(line).insert (text.at(line).begin() + column, GetLastUndoTrack().changeString.at(0));

                        else
                            text.at(line).insert (text.at(line).begin() + i, GetLastUndoTrack().changeString.at(i));
                    }
                }
            }
            UndoStack.pop_back();
        }
    } while (
        UndoStack.size() > 0 &&
        tempChangeMode == GetLastUndoTrack().changeMode &&
        (GetLastUndoTrack().changeMode == "CharacterInput" ||
         GetLastUndoTrack().changeMode == "Backspace"      ||
         GetLastUndoTrack().changeMode == "Delete")
        );
}

void Editor::REDO(int &line, int &column, vector<vector<char>> &text)
{
    string tempChangeMode = RedoStack.size() > 0 ? GetLastRedoTrack().changeMode : "";
    do
    {
        if (RedoStack.size() > 0)
        {
            UndoStack.push_back(RedoStack.back());
            column = GetLastUndoTrack().startActioncolumn;
            line   = GetLastUndoTrack().startActionLine;
            bool isMultipleLine=false, first_line=true;
            if (GetLastRedoTrack().isWirte)
            {
                int saveIndexLine=0;
                vector<char> secondPartOfString;

                for (int i=column; i<text.at(line).size(); i++)
                    secondPartOfString.push_back(text.at(line).at(i));

                for (int i=0; i<secondPartOfString.size(); i++)
                    text.at(line).pop_back();

                for (int i=0; i<GetLastRedoTrack().changeString.size(); i++)
                {
                    if (GetLastRedoTrack().changeString.at(i) == '\n')
                    {
                        vector<char> StringToVector;
                        if (GetLastRedoTrack().changeMode == "Enter")
                        {
                            text.insert (text.begin() + line + 1, StringToVector);
                            column = 0;
                            line = line + 1 <= text.size() - 1 ? line + 1 : line;
                            break;
                        } else {
                            for (int j=saveIndexLine; j<GetLastRedoTrack().changeString.size(); j++)
                                if (GetLastRedoTrack().changeString.at(j) == '\n')
                                {
                                    saveIndexLine=j+1;
                                    column = 0;
                                    line = line + 1 <= text.size() - 1 ? line + 1 : line;
                                    break;
                                } else
                                    StringToVector.push_back(GetLastRedoTrack().changeString.at(j));

                            if (!first_line)
                            {
                                line++;
                                text.insert (text.begin() + line, StringToVector);
                            } else
                                first_line=false;
                        }
                        isMultipleLine=true;
                    }

                    if (!isMultipleLine)
                    {
                        saveIndexLine = i+1;
                        text.at(line).insert (text.at(line).begin() + column, GetLastRedoTrack().changeString.at(i));
                    }

                    column++;
                }

                if (isMultipleLine)
                {
                    if (GetLastRedoTrack().changeMode == "Paste")
                    {
                        line++;
                        text.insert(text.begin() + line, secondPartOfString);
                        column--;
                        int j=GetLastRedoTrack().changeString.size()-1;

                        while (GetLastRedoTrack().changeString.at(j) != '\n')
                        {
                            text.at(line).insert(text.at(line).begin(), GetLastRedoTrack().changeString.at(j));
                            j--;
                        }
                    }
                } else {
                    for (char ch : secondPartOfString)
                        text.at(line).push_back(ch);
                }
            } else {
                if (GetLastRedoTrack().changeMode == "Backspace")
                {
                    if (GetLastRedoTrack().changeString[0] == '\n')
                        line++;
                    BACKSPACE(line, column, text, true);
                } else if (GetLastRedoTrack().changeMode == "Delete Line") {
                    int temp = text.size() == 1 && line == 0 ? 0 : line + 1;
                    DELETE_LINE(temp, column, text, true);
                } else if (GetLastRedoTrack().changeMode == "Delete") {
                    DELETE(line, column, text, true);
                }
            }
            RedoStack.pop_back();
        }
    } while (
        RedoStack.size() > 0 &&
        tempChangeMode == GetLastRedoTrack().changeMode &&
        (GetLastRedoTrack().changeMode == "CharacterInput" ||
         GetLastRedoTrack().changeMode == "Backspace"      ||
         GetLastRedoTrack().changeMode == "Delete")
        );
}

unsigned int Editor::GetBiggestLineNumberInViewport()
{
    unsigned int BiggestLineNumber=1;
    vector<int> LineNumbers;
    int range = startPrintLine + TerminalLine - 2 <= input.size() ?\
                startPrintLine + TerminalLine - 2\
                : input.size();

    for (int i=startPrintLine; i<range; i++)
        LineNumbers.push_back(i + 1);

    for (int i=0; i<LineNumbers.size(); i++)
        if (LineNumbers[i] > BiggestLineNumber)
            BiggestLineNumber = LineNumbers[i];

    return BiggestLineNumber;
}

bool Editor::UpdateViewport()
{
    int biggestNumberLine = GetBiggestLineNumberInViewport();
    bool updated=false;

    while (lineSelected + 2 - startPrintLine > TerminalLine - 3)
    {
        startPrintLine++;
        updated=true;
    }
    
    while (lineSelected + 2 - startPrintLine < 2)
    {
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
        TerminalLine = GetTerminal_LineAndColumn().at(0);
        TerminalColumn = GetTerminal_LineAndColumn().at(1);

        if ((TerminalColumn != TerminalColumnTemp || TerminalLine != TerminalLineTemp)
            && (mode == "visual" || mode == "edit"))
        {
            UpdateViewport();
            ClearTerminalScreen();
            printInfo();
            printTabs();
            printText(input, -1, -1, lineSelected, columnSelected);
            sizeChanged=true;
            ShowConsoleCursor(true);
        }

        if (sizeChanged)
        {
            printInfo();
            printTabs();
            printText(input, -1, -1, lineSelected, columnSelected);
        }

        TerminalColumnTemp = TerminalColumn;
        TerminalLineTemp = TerminalLine;
        Sleep(1);
    }
}