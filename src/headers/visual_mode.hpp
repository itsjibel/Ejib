/* <<The thing that makes editing faster (The Visual mode)>>
 * A summary of what this library does:
 * This library is a kind of shortcut for manual editing work.
 * If you want to change all the words "cat" to "dog",
 * it doesn't make sense to delete all the "cat" words and write all the "dog" 
 * instead of this from a shortcut: text --replace "cat" "dog" you use
 */
#include "insert_mode.hpp"

class VisualMode: public InsertMode
{
    private:
        vector<vector<char>> visualCommandText;
        int _currentColumn = 0, _currentLine = 0;
        bool SomethingHappenInMainTextView = false;
        string Vector2StringVisualCommandText;

    protected:
        bool search(string key, vector<vector<char>> &text, int &line, int &column);
        void VisualCommandInput();
        void VisualEdit();

    public:
        /* Each key has its own ASCII code, this Enum defines all the ASCII codes of the required keys.
         * To understand better, write a program that casts a getch() into an integer value, and then prints it,
         * now try something like control + B and see the answer
         */
        enum keyASCII
        {
            SPECIAL_KEYS_LINUX = 91,
            UP_ARROW_KEY_LINUX = 65,
            DOWN_ARROW_KEY_LINUX = 66,
            RIGHT_ARROW_KEY_LINUX = 67,
            LEFT_ARROW_KEY_LINUX = 68,
            DELETE_KEY_LINUX = 126,
            BACKSPACE_KEY_LINUX = 127,
            CTRL_BACKSPACE_KEY_LINUX = 8,
            ENTER_KEY_LINUX = 10,
            SPECIAL_KEYS_WINDOWS = -32,
            UP_ARROW_KEY_WINDOWS = 72,
            LEFT_ARROW_KEY_WINDOWS = 75,
            RIGHT_ARROW_KEY_WINDOWS = 77,
            DOWN_ARROW_KEY_WINDOWS = 80,
            DELETE_KEY_WINDOWS = 83,
            BACKSPACE_KEY_WINDOWS = 8,
            CTRL_BACKSPACE_KEY_WINDOWS = 127,
            ENTER_KEY_WINDOWS = 13,
            ESCAPE_KEY = 27,
            TAB_KEY = 9,
            CTRL_B = 2,
            CTRL_V = 22,
            CTRL_F = 6,
            CTRL_X = 24,
            CTRL_P = 16,
            CTRL_U = 21,
            CTRL_R = 18
        };
};

void VisualMode::VisualCommandInput()
{
    bool enter = false;
    /* Writing a command in visual mode is a type of insert mode
     * because the same functions are used for editing commands,
     * and for this reason, the input system is like insert mode,
     * but because we don't have things like going up and down in visual mode,
     * it is logical to will not define them here
     */
    /// We have to continue editing the visual mode command until the user does not press enter (to submit the command).
    while (!enter)
    {
        char ch;
        vector<char> emptyVector;
        if (visualCommandText.size() == 0)
            visualCommandText.push_back(emptyVector); /// For not giving segmentation fault
        /// The current column shouldn't be bigger than the command text size
        _currentColumn = _currentColumn > visualCommandText.at(0).size() ? visualCommandText.at(0).size() : _currentColumn;
        /// Giving inputs (just like insert mode)
        #if (defined (_WIN32) || defined (_WIN64))
        switch (ch = getch())
        {
            case SPECIAL_KEYS_WINDOWS:
                switch(ch = getch())
                {
                    case LEFT_ARROW_KEY_WINDOWS:
                        LEFT(_currentLine, _currentColumn, visualCommandText);
                        SomethingHappenInMainTextView=true;
                        break;
                    case RIGHT_ARROW_KEY_WINDOWS:
                        RIGHT(_currentLine, _currentColumn, visualCommandText);
                        SomethingHappenInMainTextView=true;
                        break;
                    case DELETE_KEY_WINDOWS:
                        DELETE_(_currentLine, _currentColumn, visualCommandText, false);
                        SomethingHappenInMainTextView=true;
                        break;
                    default:
                        INSERT_CHARACTER(ch, _currentLine, _currentColumn, visualCommandText, false);
                        SomethingHappenInMainTextView=true;
                }
                break;
            case BACKSPACE_KEY_WINDOWS:
                BACKSPACE(_currentLine, _currentColumn, visualCommandText, false);
                SomethingHappenInMainTextView=true;
                break;
            case CTRL_BACKSPACE_KEY_WINDOWS:
                QUICK_BACKSPACE(_currentLine, _currentColumn, visualCommandText, false);
                SomethingHappenInMainTextView=true;
                break;
            case TAB_KEY:
                TAB(_currentLine, _currentColumn, visualCommandText, false);
                SomethingHappenInMainTextView=true;
                break;
            case CTRL_V:
                PASTE(_currentLine, _currentColumn, visualCommandText);
                SomethingHappenInMainTextView=true;
                break;
            case ENTER_KEY_WINDOWS:
                enter = true;
                break;
            default:
                INSERT_CHARACTER(ch, _currentLine, _currentColumn, visualCommandText, false);
                SomethingHappenInMainTextView=true;
        }
        #endif
        #if (defined (LINUX) || defined (__linux__))
        switch (ch = getch())
        {
            case ESCAPE_KEY:
                switch(ch = getch())
                {
                    case SPECIAL_KEYS_LINUX:
                        switch(ch = getch())
                        {
                            case LEFT_ARROW_KEY_LINUX:
                                LEFT(_currentLine, _currentColumn, visualCommandText);
                                break;
                            case RIGHT_ARROW_KEY_LINUX:
                                RIGHT(_currentLine, _currentColumn, visualCommandText);
                                break;
                            case 51:
                                switch(ch = getch()) {
                                    case DELETE_KEY_LINUX:
                                        DELETE_(_currentLine, _currentColumn, visualCommandText, false);
                                        break;
                                }
                                break;
                        }
                        break;
                }
                break;
            case BACKSPACE_KEY_LINUX:
                BACKSPACE(_currentLine, _currentColumn, visualCommandText, false);
                SomethingHappenInMainTextView=true;
                break;
            case CTRL_BACKSPACE_KEY_LINUX:
                QUICK_BACKSPACE(_currentLine, _currentColumn, visualCommandText, false);
                SomethingHappenInMainTextView=true;
                break;
            case TAB_KEY:
                TAB(_currentLine, _currentColumn, visualCommandText, false);
                SomethingHappenInMainTextView=true;
                break;
            case CTRL_V:
                PASTE(_currentLine, _currentColumn, visualCommandText);
                SomethingHappenInMainTextView=true;
                break;
            case ENTER_KEY_LINUX:
                enter = true;
                break;
            default:
                INSERT_CHARACTER(ch, _currentLine, _currentColumn, visualCommandText, false);
                SomethingHappenInMainTextView=true;
        }
        #endif
        /* The user should not be able to enter more than some range command,
         * because this will disrupt the display of the user command,
         * and when she tried to enter more command characters,
         * we should show a warning to the user that you can't enter more characters.
         */
        bool showBigCommandWarning=false;
        while (visualCommandText.at(0).size() > 32)
        {
            visualCommandText.at(0).pop_back();
            showBigCommandWarning=true;
        }
        
        ShowConsoleCursor(false);
        gotoxy (0, numberOfTerminalLine - 1);
        #if (defined (_WIN32) || defined (_WIN64))
        ColorPrint("cmd@edit: ", BGREEN);
        #endif
        #if (defined (LINUX) || defined (__linux__))
        ColorPrint("\e[1mcmd@edit: \e[0m", BGREEN);
        #endif

        if (showBigCommandWarning)
        {
            gotoxy (44, numberOfTerminalLine - 1);
            ColorPrint("[B]", YELLOW);
        }
        /// We should put vector<char> into the string to check better what is the command
        Vector2StringVisualCommandText="";
        for (int i=0; i<visualCommandText.at(0).size(); i++)
            Vector2StringVisualCommandText += visualCommandText.at(0).at(i);
        /// Display the command for the user
        gotoxy(10, numberOfTerminalLine - 1);
        cout<<Vector2StringVisualCommandText;
        /// Clear command statuses of visual mode
        int8_t range = Vector2StringVisualCommandText.length() == 32 ? 0 :  32 - Vector2StringVisualCommandText.length() % 32;
        for (int i=0; i<range; i++)
            cout<<' ';
        /// We check the status of the column so that it is not wrong and out of range, if it is, we fix it
        _currentColumn = Vector2StringVisualCommandText.size() == 0 ? _currentColumn - 1 :_currentColumn;
        _currentColumn = _currentColumn > 32 ? 32 : _currentColumn;
        _currentColumn = _currentColumn < 0 ? 0 : _currentColumn;
        ShowConsoleCursor(true);
        gotoxy (10 + _currentColumn, numberOfTerminalLine - 1);
    }
};

void showMassage(string massageType)
{
    /// The command statuses
    if (massageType == "accept")
    {
        gotoxy (44, numberOfTerminalLine - 1);
        ColorPrint("[+]", GREEN);
    } else if (massageType == "command not found") {
        gotoxy (44, numberOfTerminalLine - 1);
        ColorPrint("[!]", RED);
    } else if (massageType == "nothing found") {
        gotoxy (44, numberOfTerminalLine - 1);
        ColorPrint("[?]", RED);
    }
}

void VisualMode::VisualEdit()
{
    /// In this section, we only check what command the user gave and execute that command with functions
    ShowConsoleCursor(false);
    string currentMode_View;
    for (int i=0; i<numberOfTerminalColumn / 2 - 6; i++) currentMode_View+=" ";
    currentMode_View += "-- VISUAL --";
    for (int i=0; i<numberOfTerminalColumn / 2 - 6; i++) currentMode_View+=" ";

    gotoxy (0, numberOfTerminalLine - 2);
    setColor(WHITE);
    ColorPrint(currentMode_View, YELLOW_BACKGROUND);
    ShowConsoleCursor(true);
    /// Giving the command
    gotoxy (10, numberOfTerminalLine - 1);
    VisualCommandInput();
    /// Clear the command text, for the next command
    visualCommandText.clear();
    gotoxy (10, numberOfTerminalLine - 1);
    /// Clear previous command entries using spaces
    cout<<"                                     ";
    /// Just check the command
    if (Vector2StringVisualCommandText == "edit" ||
        Vector2StringVisualCommandText == "E")
    {
        showMassage("accept");
        currentMode = "edit";
        displayLocationInfo();
        printTabs();
        displayPageOfText(mainText, -1, -1);
    } else if (Vector2StringVisualCommandText == "text -D" ||
               Vector2StringVisualCommandText == "text --delete") {
        showMassage("accept");
        ClearTerminalScreen();
        ResetAllEditFileData();
        displayLocationInfo();
        printTabs();
        displayPageOfText(mainText, -1, -1);
        ShowConsoleCursor (false);
        gotoxy (44, numberOfTerminalLine - 1);
        ColorPrint("[+]", GREEN);
    } else if (Vector2StringVisualCommandText.find("text -S \"") == 0 ||
               Vector2StringVisualCommandText.find("text --search \"") == 0) {
        string key;
        bool isKey=false;
        for (char ch : Vector2StringVisualCommandText)
        {
            if (isKey && ch != '\"')
                key += ch;
            if (isKey && ch == '\"')
                isKey=false;
            if (ch == '\"')
                isKey=true;
        }
        if (!search(key, mainText, currentLine, currentColumn))
            showMassage("nothing found");
    } else if (Vector2StringVisualCommandText == "file -S" ||
               Vector2StringVisualCommandText == "file --save") {
        currentMode = "file";
        fileSystem("save", mainText);
        ClearTerminalScreen();
        displayLocationInfo();
        printTabs();
        displayPageOfText(mainText, -1, -1);
        currentMode = "visual";
    } else {
        showMassage("command not found");
        gotoxy (10, numberOfTerminalLine - 1);
        _currentColumn=0;
        Vector2StringVisualCommandText="";
    }
}

bool VisualMode::search(string key, vector<vector<char>> &text, int &line, int &column)
{
    int previousColumn = column, tempLine = line, firstMatchColumn, firstMatchLine;
    bool first_time = true, anythingFound = false;
    column = -1; line = 0;
    do
    {
        string vector2string;
        int find_index;
        /// Convert main text line to string
        for (char ch : text.at(line))
            vector2string.push_back(ch);

        if (key.size() > 0)
            find_index = column > 0 ? column : 0;
        else
            find_index = column;
        /// Find the search key in the current line
        column = vector2string.find(key, find_index);
        /// If the search key matches the line:
        if (column != string::npos)
        {
            if (anythingFound == false)
            {
                firstMatchColumn = column;
                firstMatchLine = line;
            }
            anythingFound=true;
        }
        /// While the search key matches the line:
        while (column < 0)
        {
            if (text.size() > line + 1)
                line++;
            else {
                if (anythingFound)
                {
                    line=firstMatchLine;
                    column=firstMatchColumn;
                    break;
                } else {
                    column = previousColumn;
                    line = tempLine;
                    /// Nothing was found because we are in the last line and column and the search key was not match any line
                    return false;
                }
            }
            vector2string.clear();

            for (char ch : text.at(line))
                vector2string.push_back(ch);
            /// Find the search key in the new line
            column = vector2string.find(key, 0);
            /// If the search key matches the line:
            if (column != string::npos)
            {
                if (anythingFound == false)
                {
                    firstMatchColumn = column;
                    firstMatchLine = line;
                }
                anythingFound=true;
            }
        }
        /// Search key searched one time
        /// Now we should add to column to the size of the search key
        column = column + key.size();
        /// Display the viewport
        UpdateViewport();
        ShowConsoleCursor(false);
        displayLocationInfo();
        displayPageOfText(mainText, column - key.size() - startColumnForDisplayPage, column - 1 - startColumnForDisplayPage);
    } while (getch() != 27);
    /// Display the viewport again to show the user we are no more in the search
    displayLocationInfo();
    printTabs();
    displayPageOfText(mainText, -1, -1);
    currentMode = "edit";
    return true;
}