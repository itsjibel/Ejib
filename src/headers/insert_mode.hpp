/* <<The backbone of the Ejib>>
 * A summary of what this library does:
 * The insert mode library is a kind of user input for text editing.
 * This library performs tasks such as entering, pasting, writing, erasing, moving etc on the text.
 */
#include "commands.hpp"
#if (defined (LINUX) || defined (__linux__))
#include <stdio.h>
#endif

class InsertMode: public CommandLine
{
    private:
        int tempNumberOfTerminalColumn=0,
            tempNumberOfTerminalLine=0;

    protected:
        /* To undo and redo things,
         * you need tracks that store enough information for undoing and redoing.
         */
        struct Track
        {
            bool isWrite;
            int startActionLine;
            int startActionColumn;
            string changeString;
            char changeMode;
        };
        /* Change Modes:
         *      'C' = 'Character Input'
         *      'S' = 'Space Input'
         *      'T' = 'Tab'
         *      'P' = 'Paste'
         *      'E' = 'Enter'
         *      'B' = 'Backspace'
         *      'D' = 'Delete'
         *      'L' = 'Delete Line'
         *      'b' = 'Quick Backspace'
         */
        vector<Track> UndoStack, RedoStack;
        template <class T>
        void AddTrackToUndoStack(bool isWrite, int startActionLine, int startActionColumn,
                                 T changeString, char changeMode)
        {
            Track TrackForAdd;
            TrackForAdd.changeString = changeString;
            TrackForAdd.startActionColumn = startActionColumn;
            TrackForAdd.startActionLine = startActionLine;
            TrackForAdd.isWrite = isWrite;
            TrackForAdd.changeMode = changeMode;
            UndoStack.push_back(TrackForAdd);
        }

        Track GetLastUndoStackTrack()
        {
            return UndoStack.at(UndoStack.size() - 1);
        }

        Track GetLastRedoStackTrack()
        {
            return RedoStack.at(RedoStack.size() - 1);
        }

	public:
        /// Text editing functions:
		void INSERT_CHARACTER (char &characterInput, int &line, int &column, vector<string> &text, bool UseForRedoing);
        void BACKSPACE       (int &line, int &column, vector<string> &text, bool UseForRedoing);
        void QUICK_BACKSPACE (int &line, int &column, vector<string> &text, bool UseForRedoing);
        void DELETE_          (int &line, int &column, vector<string> &text, bool UseForRedoing);
        void DELETE_LINE     (int &line, int &column, vector<string> &text, bool UseForRedoing);
        void ENTER           (int &line, int &column, vector<string> &text, bool UseForRedoing);
        void TAB             (int &line, int &column, vector<string> &text, bool UseForRedoing);
        void PASTE           (int &line, int &column, vector<string> &text);
        void PASTE           (int &line, int &column, vector<string> &text, string stringForPaste);

        void UP          (int &line, int &column,  const vector<string> &text);
        void LEFT        (int &line, int &column,  const vector<string> &text);
        void DOWN        (int &line, int &column,  const vector<string> &text);
        void RIGHT       (int &line, int &column,  const vector<string> &text);
        void QUICK_UP    (int &line, int &column,  const vector<string> &text);
        void QUICK_LEFT  (int &line, int &column,  const vector<string> &text);
        void QUICK_DOWN  (int &line, int &column,  const vector<string> &text);
        void QUICK_RIGHT (int &line, int &column,  const vector<string> &text);

        void UNDO (int &line, int &column, vector<string> &text);
        void REDO (int &line, int &column, vector<string> &text);

        unsigned int GetBiggestLineNumberInViewport();
        bool UpdateViewport();
        void AdjustingViewportWithSizeOfTerminal();
        void setLineWithMouseWheelAction();
};

void InsertMode::INSERT_CHARACTER(char &characterInput, int &line, int &column,
                                  vector<string> &text, bool UseForRedoing)
{
    /// Check whether the character input is keyboard characters or not
    if (characterInput > 31 && characterInput < 127)
    {
        /* If the user isn't in the visual mode and this function was not used for redoing,
         * add this action to undo-stack.
         */
        if (currentMode != "visual" && !UseForRedoing)
        {
            if (characterInput == ' ')
                AddTrackToUndoStack (true, line, column, characterInput, 'S');
            else
                AddTrackToUndoStack (true, line, column, characterInput, 'C');
            RedoStack.clear();
        }
        /// Adding character to main text
        if (column != text.at(line).size())
            text.at(line).insert (text.at(line).begin() + column, characterInput);
        else
            text.at(line).push_back (characterInput);

        column++;
    }
}

void InsertMode::BACKSPACE(int &line, int &column, vector<string> &text, bool UseForRedoing)
{
    /* Checking text is empty or not? If is empty,
     * there is no need to enter the process
     */
    if (text.size() == 1 && text.at(0).empty())
        return;

    if (column > 0)
    {
        if (currentMode != "visual" && !UseForRedoing)
        {
            RedoStack.clear();
            AddTrackToUndoStack (false, line, column, text.at(line).at(column - 1), 'B');
        }
        /* If the user didn't reach the beginning of the line and
         * doesn't want to paste the current line to the end of the previous line,
         * then we delete the previous character.
         */
        text.at(line).erase (text.at(line).begin() + column - 1);
        column--;
    } else {
        if (line > 0)
        {
            if (currentMode != "visual" && !UseForRedoing)
            {
                RedoStack.clear();
                AddTrackToUndoStack (false, line - 1, column, '\n' + text.at(line), 'b');
            }
            /* If the user reaches the beginning of the line:
             *      We paste the current line to the previous line and then delete the current line,
             *      then we subtract one from the current line and,
             *      change the current column to the length of the previous line,
             */
            line--;
            column = text.at(line).size();
            text.at(line).append(text.at(line + 1));
            text.erase (text.begin() + line + 1);
        }
    }
}

void InsertMode::QUICK_BACKSPACE(int &line, int &column, vector<string> &text, bool UseForRedoing)
{
    /* Checking text is empty or not? If is empty,
     * there is no need to enter the process
     */
    if (text.size() == 1 && text.at(0).size() == 0)
        return;

    if (column > 0)
    {
        if (currentMode != "visual" && !UseForRedoing)
        {
            RedoStack.clear();
            AddTrackToUndoStack (false, line, column, text.at(line).at(column - 1), 'b');
        }
        /* If the user didn't reach the beginning of the line and
         * doesn't want to paste the current line to the end of the previous line,
         * then we delete the previous character.
         */
        text.at(line).erase (text.at(line).begin() + column - 1);
        column--;
    } else {
        if (line > 0)
        {
            if (currentMode != "visual" && !UseForRedoing)
            {
                RedoStack.clear();
                AddTrackToUndoStack (false, line - 1, column, '\n' + text.at(line), 'b');
            }
            /* If the user reaches the beginning of the line:
             *      We paste the current line to the previous line and then delete the current line,
             *      then we subtract one from the current line and,
             *      change the current column to the length of the previous line,
             */
            line--;
            column = text.at(line).size();
            text.at(line).append(text.at(line + 1));
            text.erase (text.begin() + line + 1);
        }
    }
}

void InsertMode::DELETE_(int &line, int &column, vector<string> &text, bool UseForRedoing)
{
    if (text.size() == 1 && text.at(0).size() == 0)
        return;

    if (text.size() != 0)
    {
        if (text.at(line).size() > column)
        {
            if (currentMode != "visual" && !UseForRedoing)
            {
                RedoStack.clear();
                AddTrackToUndoStack (false, line, column, text.at(line).at(column), 'D');
            }
            text.at(line).erase (text.at(line).begin() + column);
        } else {
            if (line < text.size() - 1)
            {
                string AppendCurrentLineToNextLine;
                for (int i=0; i<text.at(line + 1).size(); i++)
                    AppendCurrentLineToNextLine.push_back (text.at(line + 1).at(i));

                if (currentMode != "visual" && !UseForRedoing)
                {
                    RedoStack.clear();
                    AddTrackToUndoStack (false, line, column, '\n' + AppendCurrentLineToNextLine, 'D');
                }

                for (int i=0; i<text.at(line + 1).size(); i++)
                    text.at(line).push_back(text.at(line + 1).at(i));

                text.erase (text.begin() + line + 1);
            }
        }
    }
}

void InsertMode::DELETE_LINE(int &line, int &column, vector<string> &text, bool UseForRedoing)
{
    if (text.size() == 1 && text.at(0).size() == 0)
        return;

    if (text.size() > line)
    {
        string CurrentLineForDelete;
        for (int i=0; i<text.at(line).size(); i++)
            CurrentLineForDelete.push_back (text.at(line).at(i));

        if (currentMode != "visual" && !UseForRedoing)
        {
            RedoStack.clear();
            AddTrackToUndoStack (false, line, column, CurrentLineForDelete, 'L');
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

    string emptyString;
    if (text.size() == 0)
        text.push_back(emptyString);
}

void InsertMode::ENTER(int &line, int &column, vector<string> &text, bool UseForRedoing)
{
    if (currentMode != "visual" && !UseForRedoing)
    {
        RedoStack.clear();
        AddTrackToUndoStack (true, line, column, '\n', 'E');
    }

    string appendToNextLine;

    for (int i=column; i<text.at(line).size(); i++)
        appendToNextLine.push_back (text.at(line).at(i));

    for (int i=0; i<appendToNextLine.size(); i++)
        text.at(line).pop_back();

    text.insert (text.begin() + line + 1, appendToNextLine);
    line++;
    column=0;
}

void InsertMode::TAB(int &line, int &column, vector<string> &text, bool UseForRedoing)
{
    if (currentMode != "visual" && !UseForRedoing)
    {
        RedoStack.clear();
        AddTrackToUndoStack (true, line, column, "    ", 'T');
    }

    text.at(line).insert (text.at(line).begin() + column, {' ', ' ', ' ', ' '});
    column += 4;
}

void InsertMode::PASTE(int &line, int &column, vector<string> &text)
{
    string copiedText;
    string linkToEndOfPaste;

    for (int i=column; i<text.at(line).size(); i++)
        linkToEndOfPaste.push_back(text.at(line).at(i));

    for (int i=0; i<linkToEndOfPaste.size(); i++)
        text.at(line).pop_back();

    if (GetCopiedText(copiedText))
    {
        RemoveTextSpoilerCharacters(copiedText);
        string emptyString;
        if (text.size() == 0)
            text.insert(text.begin() + line + 1, emptyString);

        if (currentMode != "visual")
        {
            AddTrackToUndoStack (true, line, column, copiedText, 'P');
            RedoStack.clear();
        }

        for (int i=0; i<copiedText.size(); i++)
        {
            if (!(copiedText.at(i) == '\n'))
                text.at(line).insert(text.at(line).begin() + column, copiedText.at(i));

            column++;

            if (copiedText.at(i) == '\n')
            {
                column=0;
                text.insert(text.begin() + line + 1, emptyString);
                line++;
            }
        }
    }

    for (int i=0; i<linkToEndOfPaste.size(); i++)
        text.at(line).push_back(linkToEndOfPaste.at(i));
}

void InsertMode::PASTE(int &line, int &column, vector<string> &text, string stringForPaste)
{
    string linkToEndOfPaste;

    for (int i=column; i<text.at(line).size(); i++)
        linkToEndOfPaste.push_back(text.at(line).at(i));

    for (int i=0; i<linkToEndOfPaste.size(); i++)
        text.at(line).pop_back();
    string emptyString;
    if (text.size() == 0)
        text.insert(text.begin() + line + 1, emptyString);

    for (int i=0; i<stringForPaste.size(); i++)
    {
        if (stringForPaste.at(i) != '\n')
            text.at(line).insert(text.at(line).begin() + column, stringForPaste.at(i));

        column++;

        if (stringForPaste.at(i) == '\n')
        {
            column=0;
            text.insert(text.begin() + line + 1, emptyString);
            line++;
        }
    }

    for (int i=0; i<linkToEndOfPaste.size(); i++)
        text.at(line).push_back(linkToEndOfPaste.at(i));
}

void InsertMode::UP(int &line, int &column, const vector<string> &text)
{
    if (line > 0)
    {
        line--;
        column = column > text.at(line).size() ? text.at(line).size() : column;
    }
}

void InsertMode::LEFT(int &line, int &column, const vector<string> &text)
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

void InsertMode::DOWN(int &line, int &column, const vector<string> &text)
{
    if (line < text.size() - 1)
    {
        line++;
        column = column > text.at(line).size() ? text.at(line).size() : column;
    }
}

void InsertMode::RIGHT(int &line, int &column, const vector<string> &text)
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

void InsertMode::QUICK_UP(int &line, int &column, const vector<string> &text)
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

void InsertMode::QUICK_LEFT(int &line, int &column, const vector<string> &text)
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

void InsertMode::QUICK_DOWN(int &line, int &column, const vector<string> &text)
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

void InsertMode::QUICK_RIGHT(int &line, int &column, const vector<string> &text)
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

void InsertMode::UNDO(int &line, int &column, vector<string> &text)
{
    char tempChangeMode = UndoStack.size() > 0 ? GetLastUndoStackTrack().changeMode : ' ';
    do
    {
        if (UndoStack.size() > 0)
        {
            RedoStack.push_back(UndoStack.back());
            column = GetLastUndoStackTrack().startActionColumn;
            line = GetLastUndoStackTrack().startActionLine;
            bool isMultipleLine=false;

            if (GetLastUndoStackTrack().isWrite)
            {
                string lastLineDeleted;
                for (int i=0; i<GetLastUndoStackTrack().changeString.size(); i++)
                {
                    if (GetLastUndoStackTrack().changeString.at(i) == '\n')
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
                    if (GetLastUndoStackTrack().changeMode == 'P')
                    {
                        int j=GetLastUndoStackTrack().changeString.size()-1;

                        while (GetLastUndoStackTrack().changeString.at(j) != '\n')
                        {
                            text.at(line).erase(text.at(line).begin() + column);
                            j--;
                        }
                    }
            } else {
                if (GetLastUndoStackTrack().changeMode == 'L')
                {
                    if (text.size() == 1 && text.at(0).size() == 0)
                        text.clear();
                        
                    text.insert(text.begin() + line, GetLastUndoStackTrack().changeString);
                } else {
                    for (int i=0; i<GetLastUndoStackTrack().changeString.size(); i++)
                    {
                        if (GetLastUndoStackTrack().changeString.at(i) == '\n')
                        {
                            for (int j=0; j<GetLastUndoStackTrack().changeString.size() - 1; j++)
                                text.at(line).pop_back();

                            string temp = GetLastUndoStackTrack().changeString;
                            temp.erase(temp.begin());
                            text.insert (text.begin() + line + 1, temp);

                            if (GetLastUndoStackTrack().changeMode == 'B')
                                line++;
                            isMultipleLine=true;
                        }

                        if (!isMultipleLine)
                        {
                            if (GetLastUndoStackTrack().changeMode == 'B')
                                text.at(line).insert (text.at(line).begin() + column - 1, GetLastUndoStackTrack().changeString.at(0));
                            else if (GetLastUndoStackTrack().changeMode == 'D')
                                text.at(line).insert (text.at(line).begin() + column, GetLastUndoStackTrack().changeString.at(0));
                            else
                                text.at(line).insert (text.at(line).begin() + i, GetLastUndoStackTrack().changeString.at(i));
                        }
                    }
                }
            }
            UndoStack.pop_back();
        }
    } while (
        UndoStack.size() > 0 &&
        tempChangeMode == GetLastUndoStackTrack().changeMode &&
        (GetLastUndoStackTrack().changeMode == 'C' ||
         GetLastUndoStackTrack().changeMode == 'S' ||
         (GetLastUndoStackTrack().changeMode == 'B' &&
          GetLastUndoStackTrack().changeString[0] != '\n') ||
         (GetLastUndoStackTrack().changeMode == 'D' &&
          GetLastUndoStackTrack().changeString[0] != '\n'))
        );
}

void InsertMode::REDO(int &line, int &column, vector<string> &text)
{
    char tempChangeMode = RedoStack.size() > 0 ? GetLastRedoStackTrack().changeMode : ' ';
    do
    {
        if (RedoStack.size() > 0)
        {
            UndoStack.push_back(RedoStack.back());
            column = GetLastUndoStackTrack().startActionColumn;
            line   = GetLastUndoStackTrack().startActionLine;
            bool isMultipleLine=false, first_line=true;

            if (GetLastRedoStackTrack().isWrite)
            {
                if (GetLastRedoStackTrack().changeMode == 'E')
                    ENTER(line, column, text, true);
                else if (GetLastRedoStackTrack().changeMode == 'C' ||
                         GetLastRedoStackTrack().changeMode == 'S')
                    INSERT_CHARACTER(GetLastRedoStackTrack().changeString.at(0), line, column, text, true);
                else if (GetLastRedoStackTrack().changeMode == 'P')
                    PASTE(line, column, text, GetLastRedoStackTrack().changeString);
                else if (GetLastRedoStackTrack().changeMode == 'T')
                    TAB(line, column, text, true);
            } else {
                if (GetLastRedoStackTrack().changeMode == 'B')
                {
                    line = GetLastRedoStackTrack().changeString[0] == '\n' ? line + 1 : line;
                    BACKSPACE(line, column, text, true);
                } else if (GetLastRedoStackTrack().changeMode == 'b') {
                    line = GetLastRedoStackTrack().changeString[0] == '\n' ? line + 1 : line;
                    QUICK_BACKSPACE(line, column, text, true);
                }
                else if (GetLastRedoStackTrack().changeMode == 'L') 
                    DELETE_LINE(line, column, text, true);
                else if (GetLastRedoStackTrack().changeMode == 'D')
                    DELETE_(line, column, text, true);
            }

            RedoStack.pop_back();
        }
    } while (
        RedoStack.size() > 0 &&
        tempChangeMode == GetLastRedoStackTrack().changeMode &&
        (GetLastRedoStackTrack().changeMode == 'C' ||
         GetLastRedoStackTrack().changeMode == 'S' ||
         (GetLastRedoStackTrack().changeMode == 'B' &&
          GetLastRedoStackTrack().changeString[0] != '\n') ||
         (GetLastRedoStackTrack().changeMode == 'D' &&
          GetLastRedoStackTrack().changeString[0] != '\n'))
    );
}

unsigned int InsertMode::GetBiggestLineNumberInViewport()
{
    unsigned int BiggestLineNumber=1;
    vector<int> LineNumbers;
    int range = startLineForDisplayPage + numberOfTerminalLine - 2 <= mainText.size() ?\
                startLineForDisplayPage + numberOfTerminalLine - 2\
                : mainText.size();

    for (int i=startLineForDisplayPage; i<range; i++)
        LineNumbers.push_back(i + 1);

    for (int i=0; i<LineNumbers.size(); i++)
        if (LineNumbers[i] > BiggestLineNumber)
            BiggestLineNumber = LineNumbers[i];

    return BiggestLineNumber;
}

bool InsertMode::UpdateViewport()
{
    int biggestNumberLine = GetBiggestLineNumberInViewport();
    bool updated=false;

    while (currentLine + 2 - startLineForDisplayPage > numberOfTerminalLine - 3)
    {
        startLineForDisplayPage++;
        updated=true;
    }
    
    while (currentLine + 2 - startLineForDisplayPage < 2)
    {
        startLineForDisplayPage--;
        updated=true;
    }

    while (currentColumn - startColumnForDisplayPage + floor(log10(biggestNumberLine) + 1) + 1 > numberOfTerminalColumn - 1)
    {
        startColumnForDisplayPage++;
        updated=true;
    }
    
    while (currentColumn - startColumnForDisplayPage + floor(log10(biggestNumberLine) + 1) + 1 < floor(log10(biggestNumberLine) + 1) + 1)
    {
        startColumnForDisplayPage--;
        updated=true;
    }

    return updated;
}

void InsertMode::AdjustingViewportWithSizeOfTerminal()
{
    while(1)
    {
        bool sizeIsChanged=false;
        numberOfTerminalLine = GetTerminal_LineAndColumn().at(0);
        numberOfTerminalColumn = GetTerminal_LineAndColumn().at(1);

        if ((numberOfTerminalColumn != tempNumberOfTerminalColumn ||
             numberOfTerminalLine   != tempNumberOfTerminalLine) &&
            (currentMode == "visual" || currentMode == "edit"))
        {
            UpdateViewport();
            ClearTerminalScreen();
            displayLocationInfo();
            printTabs();
            displayPageOfText(mainText, -1, -1);
            sizeIsChanged=true;
            ShowConsoleCursor(true);
        }

        if (sizeIsChanged)
        {
            displayLocationInfo();
            printTabs();
            displayPageOfText(mainText, -1, -1);
        }

        tempNumberOfTerminalColumn = numberOfTerminalColumn;
        tempNumberOfTerminalLine = numberOfTerminalLine;
        Sleep(1);
    }
}

string execute_command(const char* cmd)
{
    std::array<char, 128> buffer;
    string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe)
        throw std::runtime_error("popen() failed!");

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();

    return result;
}

void InsertMode::setLineWithMouseWheelAction()
{
    int fd;
    struct input_event ie;

    if (execute_command("cat /etc/issue | grep -c \"Kali\"")[0] == '1')
    {
        if ((fd = open("/dev/input/event4", O_RDONLY)) == -1)
        {
            perror("/dev/input/event4 for mouse wheel detection");
            return;
        }
    } else if (execute_command("cat /etc/issue | grep -c \"Ubuntu\"")[0] == '1') {
        if ((fd = open("/dev/input/event7", O_RDONLY)) == -1)
        {
            perror("/dev/input/event7 for mouse wheel detection");
            return;
        }
    } else {
        cout<<"\n[Support Error]: Your OS is unsupported by Ejib\n";
        return;
    }


    while (read(fd, &ie, sizeof(struct input_event)))
    {
        bool somethingHappenTextView = false;
        if (ie.type == EV_REL && ie.code == REL_WHEEL)
            if (ie.value > 0)
            {
                QUICK_UP(currentLine, currentColumn, mainText);
                if (UpdateViewport())
                    displayPageOfText(mainText, -1, -1);
                displayLocationInfo();
                displayPageOfText(mainText, -1, -1);
            }
            else if (ie.value < 0)
            {
                QUICK_DOWN(currentLine, currentColumn, mainText);
                if (UpdateViewport())
                    displayPageOfText(mainText, -1, -1);
                displayLocationInfo();
                displayPageOfText(mainText, -1, -1);
            }
    }
}
