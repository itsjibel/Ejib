#include "user_input.hpp"

class VisualCommand: public Editor
{
    private:
        vector<vector<char>> _editCommand;
        int _columnSelected=0, line=0;
        bool _something_happen_in_text_view=false;
        string _string_editCommand;

    protected:
        bool search(string key, vector<vector<char>> &text, int &line, int &column);
        void VisualCommandInput();
        void VisualEdit();
};

void VisualCommand::VisualCommandInput()
{
    bool enter=false;
    while (!enter)
    {
        int ch;

        if (_editCommand.size() == 0)
            _editCommand.push_back(emptyVector);

        _columnSelected = _columnSelected > _editCommand.at(0).size() ? _editCommand.at(0).size() : _columnSelected;

        #if (defined (_WIN32) || defined (_WIN64))
        switch (ch = getch()) {
            case 0:
                case 0xE0:
                    switch(ch = getch())
                    {
                        case 75:
                            left(line, _columnSelected, _editCommand);
                            _something_happen_in_text_view=true;
                            break;
                        case 77:
                            right(line, _columnSelected, _editCommand);
                            _something_happen_in_text_view=true;
                            break;
                        case 'S':
                            _delete(line, _columnSelected, _editCommand);
                            _something_happen_in_text_view=true;
                            break;
                        default:
                            getCharacter(ch, line, _columnSelected, _editCommand);
                            _something_happen_in_text_view=true;
                    }
                    break;
                    case 8:
                        backspace(line, _columnSelected, _editCommand);
                        _something_happen_in_text_view=true;
                        break;
                    case 9:
                        tab(line, _columnSelected, _editCommand);
                        _something_happen_in_text_view=true;
                        break;
                    case 22:
                        paste(line, _columnSelected, _editCommand);
                        _something_happen_in_text_view=true;
                        break;
                    case 13:
                        enter = true;
                        break;
                    default:
                        getCharacter(ch, line, _columnSelected, _editCommand);
                        _something_happen_in_text_view=true;
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
                                LEFT(line, _columnSelected, _editCommand);
                                break;
                            case 67:
                                RIGHT(line, _columnSelected, _editCommand);
                                break;
                            case 51:
                                switch(ch = getch()) {
                                    case 126:
                                        DELETE(line, _columnSelected, _editCommand);
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
                BACKSPACE(line, _columnSelected, _editCommand);
                _something_happen_in_text_view=true;
                break;
            case 9:
                TAB(line, _columnSelected, _editCommand);
                _something_happen_in_text_view=true;
                break;
            case 22:
                PASTE(line, _columnSelected, _editCommand);
                _something_happen_in_text_view=true;
                break;
            case 10:
                enter = true;
                break;
            default:
                INPUT_CHARACTER(ch, line, _columnSelected, _editCommand);
                _something_happen_in_text_view=true;
        }
        #endif
        bool showBigCommandWarning=false;

        while (_editCommand.at(0).size() > 32)
        {
            _editCommand.at(0).pop_back();
            showBigCommandWarning=true;
        }
        
        ShowConsoleCursor(false);
        gotoxy (0, TerminalLine - 1);
        #if (defined (_WIN32) || defined (_WIN64))
        setColor(10);
        cout<<"cmd@edit: ";
        #endif
        #if (defined (LINUX) || defined (__linux__))
        setColor(32);
        cout<<"\e[1mcmd@edit: \e[0m";
        setColor(32);
        #endif

        if (showBigCommandWarning)
        {
            #if (defined (_WIN32) || defined (_WIN64))
            setColor(6);
            #endif
            #if (defined (LINUX) || defined (__linux__))
            setColor(33);
            #endif
            gotoxy (44, TerminalLine - 1);
            cout<<"[B]";
        }

        _string_editCommand="";

        for (int i=0; i<_editCommand.at(0).size(); i++)
            _string_editCommand+=_editCommand.at(0).at(i);

        gotoxy(10, TerminalLine - 1);
        cout<<_string_editCommand;
        int8_t range = _string_editCommand.length() == 32 ? 0 :  32 - _string_editCommand.length() % 32;
        
        for (int i=0; i<range; i++)
            cout<<' ';

        _columnSelected = _string_editCommand.size() == 0 ? _columnSelected - 1 :_columnSelected;
        _columnSelected = _columnSelected > 32 ? 32 : _columnSelected;
        _columnSelected = _columnSelected < 0 ? 0 : _columnSelected;
        ShowConsoleCursor(true);
        gotoxy (10 + _columnSelected, TerminalLine - 1);
        TerminalColumnTemp = TerminalColumn;
        TerminalLineTemp = TerminalLine;
    }
};

void showMassage(string massageType) {
    if (massageType == "accept")
    {
        #if (defined (_WIN32) || defined (_WIN64))
        setColor(10);
        #endif
        #if (defined (LINUX) || defined (__linux__))
        setColor(32);
        #endif
        gotoxy (44, TerminalLine - 1);
        cout<<"[+]";
    } else if (massageType == "command not found") {
        #if (defined (_WIN32) || defined (_WIN64))
        setColor(4);
        #endif
        #if (defined (LINUX) || defined (__linux__))
        setColor(31);
        #endif
        gotoxy (44, TerminalLine - 1);
        cout<<"[!]";
    } else if (massageType == "nothing found") {
        #if (defined (_WIN32) || defined (_WIN64))
        setColor(4);
        #endif
        #if (defined (LINUX) || defined (__linux__))
        setColor(31);
        #endif
        gotoxy (44, TerminalLine - 1);
        cout<<"[?]";
    }
    
}

void VisualCommand::VisualEdit()
{
    VisualCommandInput();
    _editCommand.clear();
    gotoxy (10, TerminalLine - 1);
    cout<<"                                     ";

    if (_string_editCommand == "edit" || _string_editCommand == "E")
    {

        showMassage("accept");
        mode = "edit";
        printInfo();
        printText(input, -1, -1, lineSelected, columnSelected);
    
    } else if (_string_editCommand == "text -D" || _string_editCommand == "text --delete") {

        showMassage("accept");
        clearEditFile();
        printInfo();
        printText(input, -1, -1, lineSelected, columnSelected);
        ShowConsoleCursor (false);

        #if (defined (_WIN32) || defined (_WIN64))
        setColor(10);
        #endif
        #if (defined (LINUX) || defined (__linux__))
        setColor(32);
        #endif

        gotoxy (44, TerminalLine - 1);
        cout<<"[+]";

    } else if (_string_editCommand.find("text -S \"")       == 0 ||
                _string_editCommand.find("text --search \"") == 0) {
        string key;
        bool isKey=false;
        for (char ch : _string_editCommand)
        {
            if (isKey && ch != '\"')
                key += ch;

            if (isKey && ch == '\"')
                isKey=false;

            if (ch == '\"')
                isKey=true;
        }
        if (!search(key, input, lineSelected, columnSelected))
            showMassage("nothing found");
    } else if (_string_editCommand == "file -S" || _string_editCommand == "file --save") {
        mode = "file";
        fileSystem("save", input);
        system("clear");
        printInfo();
        printTabs();
        printText(input, -1, -1, lineSelected, columnSelected);
        mode = "visual";
    } else {
        showMassage("command not found");
        gotoxy (10, TerminalLine - 1);
        _columnSelected=0;
        _string_editCommand="";
    }
}

bool VisualCommand::search(string key, vector<vector<char>> &text, int &line, int &column)
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
        updateViewport();
        ShowConsoleCursor(false);
        printInfo();
        printText(input, column - key.size() - startPrintColumn, column - 1 - startPrintColumn, line, column);
    } while (getch() != 27);

    mode = "edit";
    return true;
}