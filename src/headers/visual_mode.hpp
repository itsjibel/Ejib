#include "insert_mode.hpp"

class VisualMode: public InsertMode
{
    private:
        vector<vector<char>> visualCommandText;
        int _currentColumn=0, _currentLine=0;
        bool SomethingHappenInMainTextView=false;
        string Vector2StringVisualCommandText;

    protected:
        bool search(string key, vector<vector<char>> &text, int &line, int &column);
        void VisualCommandInput();
        void VisualEdit();
};

void VisualMode::VisualCommandInput()
{
    bool enter=false;
    while (!enter)
    {
        char ch;
        vector<char> emptyVector;
        if (visualCommandText.size() == 0)
            visualCommandText.push_back(emptyVector);

        _currentColumn = _currentColumn > visualCommandText.at(0).size() ? visualCommandText.at(0).size() : _currentColumn;

        #if (defined (_WIN32) || defined (_WIN64))
        switch (ch = getch()) {
            case 0:
                case 0xE0:
                    switch(ch = getch())
                    {
                        case 75:
                            left(line, _currentColumn, _editCommand);
                            SomethingHappenInMainTextView=true;
                            break;
                        case 77:
                            right(line, _currentColumn, _editCommand);
                            SomethingHappenInMainTextView=true;
                            break;
                        case 'S':
                            _delete(line, _currentColumn, _editCommand);
                            SomethingHappenInMainTextView=true;
                            break;
                        default:
                            getCharacter(ch, line, _currentColumn, _editCommand);
                            SomethingHappenInMainTextView=true;
                    }
                    break;
                    case 8:
                        backspace(line, _currentColumn, _editCommand);
                        SomethingHappenInMainTextView=true;
                        break;
                    case 9:
                        tab(line, _currentColumn, _editCommand);
                        SomethingHappenInMainTextView=true;
                        break;
                    case 22:
                        paste(line, _currentColumn, _editCommand);
                        SomethingHappenInMainTextView=true;
                        break;
                    case 13:
                        enter = true;
                        break;
                    default:
                        getCharacter(ch, line, _columnSelected, _editCommand);
                        SomethingHappenInMainTextView=true;
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
                            case 68:
                                LEFT(_currentLine, _currentColumn, visualCommandText);
                                break;
                            case 67:
                                RIGHT(_currentLine, _currentColumn, visualCommandText);
                                break;
                            case 51:
                                switch(ch = getch()) {
                                    case 126:
                                        DELETE(_currentLine, _currentColumn, visualCommandText, false);
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
                BACKSPACE(_currentLine, _currentColumn, visualCommandText, false);
                SomethingHappenInMainTextView=true;
                break;
            case 9:
                TAB(_currentLine, _currentColumn, visualCommandText, false);
                SomethingHappenInMainTextView=true;
                break;
            case 22:
                PASTE(_currentLine, _currentColumn, visualCommandText);
                SomethingHappenInMainTextView=true;
                break;
            case 10:
                enter = true;
                break;
            default:
                INSERT_CHARACTER(ch, _currentLine, _currentColumn, visualCommandText, false);
                SomethingHappenInMainTextView=true;
        }
        #endif
        bool showBigCommandWarning=false;

        while (visualCommandText.at(0).size() > 32)
        {
            visualCommandText.at(0).pop_back();
            showBigCommandWarning=true;
        }
        
        ShowConsoleCursor(false);
        gotoxy (0, numberOfTerminalLine - 1);
        #if (defined (_WIN32) || defined (_WIN64))
        ColorPrint("cmd@edit: ", 10);
        #endif
        #if (defined (LINUX) || defined (__linux__))
        ColorPrint("\e[1mcmd@edit: \e[0m", 32);
        #endif

        if (showBigCommandWarning)
        {
            gotoxy (44, numberOfTerminalLine - 1);
            ColorPrint("[B]", 6);
        }

        Vector2StringVisualCommandText="";

        for (int i=0; i<visualCommandText.at(0).size(); i++)
            Vector2StringVisualCommandText += visualCommandText.at(0).at(i);

        gotoxy(10, numberOfTerminalLine - 1);
        cout<<Vector2StringVisualCommandText;
        int8_t range = Vector2StringVisualCommandText.length() == 32 ? 0 :  32 - Vector2StringVisualCommandText.length() % 32;
        
        for (int i=0; i<range; i++)
            cout<<' ';

        _currentColumn = Vector2StringVisualCommandText.size() == 0 ? _currentColumn - 1 :_currentColumn;
        _currentColumn = _currentColumn > 32 ? 32 : _currentColumn;
        _currentColumn = _currentColumn < 0 ? 0 : _currentColumn;
        ShowConsoleCursor(true);
        gotoxy (10 + _currentColumn, numberOfTerminalLine - 1);
    }
};

void showMassage(string massageType) {
    if (massageType == "accept")
    {
        gotoxy (44, numberOfTerminalLine - 1);
        ColorPrint("[+]", 2);
    } else if (massageType == "command not found") {
        gotoxy (44, numberOfTerminalLine - 1);
        ColorPrint("[!]", 4);
    } else if (massageType == "nothing found") {
        gotoxy (44, numberOfTerminalLine - 1);
        ColorPrint("[?]", 4);
    }
    
}

void VisualMode::VisualEdit()
{
    ShowConsoleCursor(false);
    string currentMode_View;

    for (int i=0; i<numberOfTerminalColumn / 2 - 6; i++) currentMode_View+=" ";
    currentMode_View += "-- VISUAL --";
    for (int i=0; i<numberOfTerminalColumn / 2 - 6; i++) currentMode_View+=" ";

    gotoxy (0, numberOfTerminalLine - 2);
    setColor(7);
    ColorPrint(currentMode_View, 97);
    ShowConsoleCursor(true);
    gotoxy (10, numberOfTerminalLine - 1);
    VisualCommandInput();
    visualCommandText.clear();
    gotoxy (10, numberOfTerminalLine - 1);
    cout<<"                                     ";

    if (Vector2StringVisualCommandText == "edit" ||
        Vector2StringVisualCommandText == "E")
    {
        showMassage("accept");
        currentMode = "edit";
        displayLocationInfo();
        displayPageOfText(mainText, -1, -1, currentLine, currentColumn);
    } else if (Vector2StringVisualCommandText == "text -D" ||
               Vector2StringVisualCommandText == "text --delete") {

        showMassage("accept");
        ClearTerminalScreen();
        displayLocationInfo();
        displayPageOfText(mainText, -1, -1, currentLine, currentColumn);
        ShowConsoleCursor (false);
        gotoxy (44, numberOfTerminalLine - 1);
        ColorPrint("[+]", 2);
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
        displayPageOfText(mainText, -1, -1, currentLine, currentColumn);
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
    int previousColumn=column, tempLine=line, firstCatchColumn, firstCatchLine;
    bool first_time=true, anythingFound=false;
    column=-1; line=0;
    do {
        string s;
        int find_index;

        for (char ch : text.at(line))
            s.push_back(ch);

        if (key.size() > 0)
            find_index = column > 0 ? column : 0;
        else
            find_index = column;

        column = s.find(key, find_index);
            
        if (column != string::npos)
        {
            if (anythingFound == false)
            {
                firstCatchColumn = column;
                firstCatchLine = line;
            }
            anythingFound=true;
        }
        
        while (column < 0)
        {
            if (text.size() > line + 1)
                line++;
            else {
                if (anythingFound)
                {
                    line=firstCatchLine;
                    column=firstCatchColumn;
                    break;
                } else {
                    column = previousColumn;
                    line = tempLine;
                    return false;
                }
            }
            s.clear();

            for (char ch : text.at(line))
                s.push_back(ch);

            column = s.find(key, 0);

            if (column != string::npos)
            {
                if (anythingFound == false)
                {
                    firstCatchColumn = column;
                    firstCatchLine = line;
                }
                anythingFound=true;
            }
        }

        column = column + key.size();
        UpdateViewport();
        ShowConsoleCursor(false);
        displayLocationInfo();
        displayPageOfText(mainText, column - key.size() - startColumnForDisplayPage, column - 1 - startColumnForDisplayPage, line, column);
    } while (getch() != 27);

    currentMode = "edit";
    return true;
}