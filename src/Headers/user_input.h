#include "commands.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <chrono>
class Editor: public CommandLine {
	public:
		void getCharacter(char characterInput, int &line, int &column, vector<vector<char>> &text);
        void backspace   (int &line, int &column,       vector<vector<char>> &text);
        void _delete     (int &line, int &column,       vector<vector<char>> &text);
        void deleteLine  (int &line, int &column,       vector<vector<char>> &text);
        void enter       (int &line, int &column,       vector<vector<char>> &text);
        void tab         (int &line, int &column,       vector<vector<char>> &text);
        void paste       (int &line, int &column,       vector<vector<char>> &text);
        void up          (int &line, int &column, const vector<vector<char>> &text);
        void left        (int &line, int &column, const vector<vector<char>> &text);
        void down        (int &line, int &column, const vector<vector<char>> &text);
        void right       (int &line, int &column, const vector<vector<char>> &text);
        void reSizeTerminal();
        void EDIT_SYSTEM();
};
void Editor::getCharacter(char characterInput, int &line, int &column, vector<vector<char>> &text) {
    if (column != text.at(line).size()) {
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
void Editor::backspace(int &line, int &column, vector<vector<char>> &text) {
    int numberLines[10000] = {0}, biggestNumberLine=0,
        range = startPrintLine + TerminalLine - 2 <= input.size() ? startPrintLine + TerminalLine - 2 : input.size();

    for (int i=startPrintLine; i<range; i++)
        numberLines[i - startPrintLine] = i + 1;
    for (int i=0; i<TerminalLine - 2; i++)
        if (numberLines[i] > biggestNumberLine)
            biggestNumberLine = numberLines[i];
    int numberLineDigits = floor(log10(biggestNumberLine) + 1);

    if (column > 0) {
        text.at(line).erase (text.at(line).begin() + column - 1);
        column--;
    } else {
        if (line > 0) {
            line--;
            column = text.at(line).size();
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
void Editor::_delete(int &line, int &column, vector<vector<char>> &text) {
    if (text.size() != 0)
        if (text.at(line).size() > column)
            text.at(line).erase (text.at(line).begin() + column);
        else
            if (line < text.size() - 1) {

                for (int i=0; i<text.at(line + 1).size(); i++)
                    text.at(line).push_back(text.at(line + 1).at(i));

                text.erase (text.begin() + line + 1);
            }
}
void Editor::deleteLine(int &line, int &column, vector<vector<char>> &text) {
    if (text.size() > 0) {
        line = line - 1 > 0 ? line - 1 : 0;
        text.erase(text.begin() + line + 1);
        column=0;
    }
    if (text.size() == 0)
        text.push_back(emptyVector);
}
void Editor::enter(int &line, int &column, vector<vector<char>> &text) {
    vector<char> part1, part2;

    for (int i=0; i<column; i++)
        part1.push_back (text.at(line).at(i));

    for (int i=column; i<text.at(line).size(); i++)
        part2.push_back (text.at(line).at(i));

    text.at(line) = part1;
    text.insert (text.begin() + line + 1, part2);
    line++;
    column=0;
}
void Editor::tab(int &line, int &column, vector<vector<char>> &text) {
    if (column != text.at(line).size())
        for (int i = 0; i < 4; i++)
            text.at(line).insert (text.at(line).begin() + column, ' ');
    else
        for (int i = 0; i < 4; i++)
            text.at(line).push_back (' ');

    column += 4;
}
void Editor::paste(int &line, int &column, vector<vector<char>> &text) {
    string copiedText;
    Display *display = XOpenDisplay(NULL);
    unsigned long color = BlackPixel(display, DefaultScreen(display));
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0,0, 1,1, 0, color, color);
    Bool result = PrintSelection(display, window, "CLIPBOARD", "UTF8_STRING", copiedText) || PrintSelection(display, window, "CLIPBOARD", "STRING", copiedText);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    if (result) {
        text.push_back(emptyVector);
        for (int i=0; i<copiedText.size(); i++)
            if (copiedText.at(i) != '\r' && copiedText.at(i) != '\t' &&
                copiedText.at(i) != '\v' && copiedText.at(i) != '\0' &&
                copiedText.at(i) != '\f' && copiedText.at(i) != '\a' &&
                copiedText.at(i) != '\e' && copiedText.at(i) != '\b') {

                if (!(copiedText.at(i) == '\n'))
                    text.at(line).push_back(copiedText.at(i));

                if (copiedText.at(i) == '\n' || i == copiedText.size() - 1) {
                    text.push_back(emptyVector);
                    line = i != copiedText.size() - 1 ? line + 1 : line;
                }

            }

        for (int i=0; i<2; i++) text.pop_back();

        column = text.at(line).size();
    }
}
void Editor::up(int &line, int &column, const vector<vector<char>> &text) {
    if (line > 0) {
        line--;
        column = column > text.at(line).size() ? text.at(line).size() : column;
    }
}
void Editor::left(int &line, int &column, const vector<vector<char>> &text) {
    column = column - 1 > -1 ? column - 1 : -1;

    if (column < 0)
        if (line > 0) {
            line--;
            column = text.at(line).size();
        } else {
            column=0;
        }
}
void Editor::right(int &line, int &column, const vector<vector<char>> &text) {
    column++;
    if (column > text.at(line).size()) {

        if (line < text.size() - 1) {
            line++;
            column = 0;
        } else {
            column = text.at(line).size();
        }

    }
}
void Editor::down(int &line, int &column, const vector<vector<char>> &text) {
    if (line < text.size() - 1) {
        line++;
        column = column > text.at(line).size() ? text.at(line).size() : column;
    }
}
void Editor::EDIT_SYSTEM() {
    int ch;
    bool something_happen_in_text_view=false;
    switch (ch = getch()) {
        case 27:
            switch(ch = getch()) {
                case 91:
                    switch(ch = getch()) {
                        case 65:  up     (lineSelected, columnSelected, input); something_happen_in_text_view=true; break;
                        case 68:  left   (lineSelected, columnSelected, input); something_happen_in_text_view=true; break;
                        case 67:  right  (lineSelected, columnSelected, input); something_happen_in_text_view=true; break;
                        case 66:  down   (lineSelected, columnSelected, input); something_happen_in_text_view=true; break;
                        case 51:
                            switch(ch = getch()) {
                                case 126:  _delete(lineSelected, columnSelected, input); something_happen_in_text_view=true; break;
                                default: ;
                            }
                            break;
                        default: ;
                    }
                    break;
                default: ;
            }
            break;
        case 127:  backspace(lineSelected, columnSelected, input);        something_happen_in_text_view=true;    break;
        case 2:  mode = "visual";                                                                                break;
        case 10: enter(lineSelected, columnSelected, input);            something_happen_in_text_view=true;      break;
        case 9:  tab(lineSelected, columnSelected, input);              something_happen_in_text_view=true;      break;
        case 22: paste(lineSelected, columnSelected, input);            something_happen_in_text_view=true;      break;
        case 19: if (!fileSystem("save", input)) { system("clear"); printInfo(); printText(input, -1, -1, -1); } break;
        case 24: deleteLine(lineSelected, columnSelected, input);       something_happen_in_text_view=true;      break;
        case 16:  mode = "command";                                      setColor(37);                            break;
        default: getCharacter(ch, lineSelected, columnSelected, input); something_happen_in_text_view=true;
    }
    int numberLines[10000] = {0}, biggestNumberLine=0,
        range = startPrintLine + TerminalLine - 2 <= input.size() ? startPrintLine + TerminalLine - 2 : input.size();
    for (int i=startPrintLine; i<range; i++)
        numberLines[i - startPrintLine] = i + 1;

    for (int i=0; i<TerminalLine - 2; i++)
        if (numberLines[i] > biggestNumberLine)
            biggestNumberLine = numberLines[i];

    while (lineSelected - startPrintLine > TerminalLine - 3)
        startPrintLine++;

    while (lineSelected - startPrintLine < 0)
        startPrintLine--;

    while (columnSelected - startPrintColumn + floor(log10(biggestNumberLine) + 1) + 1 > TerminalColumn - 1)
        startPrintColumn++;

    while (columnSelected - startPrintColumn + floor(log10(biggestNumberLine) + 1) + 1 < floor(log10(biggestNumberLine) + 1) + 1)
        startPrintColumn--;

    if (something_happen_in_text_view) {
        printInfo();
        printText(input, -1, -1, -1);
    }
}
void Editor::reSizeTerminal() {
    while (1) {
        bool sizeChanged=false;
        struct winsize w;
        ioctl (STDOUT_FILENO, TIOCGWINSZ, &w);
        TerminalLine = w.ws_row;
        TerminalColumn = w.ws_col;

        if (mode == "visual" || mode == "edit") {
            if (TerminalColumn != TerminalColumnTemp || TerminalLine != TerminalLineTemp) {
                int numberLines[10000] = {0}, biggestNumberLine=0,
                    range = startPrintLine + TerminalLine - 2 <= input.size() ? startPrintLine + TerminalLine - 2 : input.size();
                for (int i=startPrintLine; i<range; i++)
                    numberLines[i - startPrintLine] = i + 1;

                for (int i=0; i<TerminalLine - 2; i++)
                    if (numberLines[i] > biggestNumberLine)
                        biggestNumberLine = numberLines[i];
                while (lineSelected - startPrintLine > TerminalLine - 3)
                    startPrintLine++;
                while (lineSelected - startPrintLine < 0)
                    startPrintLine--;
                while (columnSelected - startPrintColumn + floor(log10(biggestNumberLine) + 1) + 1 > TerminalColumn - 1)
                    startPrintColumn++;
                while (columnSelected - startPrintColumn + floor(log10(biggestNumberLine) + 1) + 1 < floor(log10(biggestNumberLine) + 1) + 1)
                    startPrintColumn--;

                system("clear");
                printInfo();
                printText(input, -1, -1, -1);
                sizeChanged=true;
            }
        }
        if (sizeChanged) {
            printInfo();
            printText(input, -1, -1, -1);
        }
        TerminalColumnTemp = TerminalColumn;
        TerminalLineTemp = TerminalLine;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}


class EditCommand: public Editor {
    private:
        vector<vector<char>> _editCommand;
        int _columnSelected=0, line=0;
        bool _something_happen_in_text_view=false;
        string _string_editCommand;

    protected:
        bool search(string key, vector<vector<char>> &text, int &line, int &column) {
            int previousColumn=column, tempLine=line, firstCatchColumn, firstCatchLine;
            bool first_time=true, anythingFound=false;
            column=-1; line=0;
            do {
                string s;
                for (char ch : text.at(line))
                    s.push_back(ch);
                    column = s.find(key, column + 1);
                    
                if (column != string::npos) {
                    if (anythingFound == false) {
                        firstCatchColumn = column;
                        firstCatchLine = line;
                    }
                    anythingFound=true;
                }
                
                while (column < 0) {
                    if (text.size() > line + 1) {
                        line++;
                    } else {
                        if (anythingFound) {
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

                    if (column != string::npos) {
                        if (anythingFound == false) {
                            firstCatchColumn = column;
                            firstCatchLine = line;
                        }
                        anythingFound=true;
                    }
                }

                column = column + key.size();

                int numberLines[10000] = {0}, biggestNumberLine=0,
                    range = startPrintLine + TerminalLine - 2 <= input.size() ? startPrintLine + TerminalLine - 2 : input.size();
                if (!(input.size() == 1 && input.at(0).size() == 0))
                    for (int i=startPrintLine; i<range; i++)
                        numberLines[i - startPrintLine] = i + 1;
                for (int i=0; i<TerminalLine - 2; i++)
                    if (numberLines[i] > biggestNumberLine)
                        biggestNumberLine = numberLines[i];
                while (lineSelected - startPrintLine > 27)
                    startPrintLine++;
                while (lineSelected - startPrintLine < 0)
                    startPrintLine--;
                while (columnSelected - startPrintColumn + floor(log10(biggestNumberLine) + 1) + 1 + key.size() > TerminalColumn - 1)
                    startPrintColumn++;
                while (columnSelected - startPrintColumn + floor(log10(biggestNumberLine) + 1) + 1 - key.size() < floor(log10(biggestNumberLine) + 1) + 1)
                    startPrintColumn--;
                    
                ShowConsoleCursor(false);
                printInfo();
                printText(input, column - key.size() - startPrintColumn, column - 1 - startPrintColumn, line);
            } while (getch() != 27);
            mode = "edit";
            return true;
        }
        void editCommand() {
            bool enter=false;
            while (!enter) {

                if (_editCommand.size() == 0)
                    _editCommand.push_back(emptyVector);

                _columnSelected = _columnSelected > _editCommand.at(0).size() ? _editCommand.at(0).size() : _columnSelected;
                int ch;
                switch (ch = getch()) {
                    case 27:
                        switch(ch = getch()) {
                            case 91:
                                switch(ch = getch()) {
                                    case 68:  left   (line, _columnSelected, _editCommand); break;
                                    case 67:  right  (line, _columnSelected, _editCommand); break;
                                    case 51:
                                        switch(ch = getch()) {
                                            case 126:  _delete(line, _columnSelected, _editCommand); break;
                                            default: ;
                                        }
                                        break;
                                    default: ;
                                }
                                break;
                            default: ;
                        }
                        break;
                    case 127: backspace     (line,     _columnSelected, _editCommand); _something_happen_in_text_view=true; break;
                    case 9:   tab           (line,     _columnSelected, _editCommand); _something_happen_in_text_view=true; break;
                    case 22:  paste         (line,     _columnSelected, _editCommand); _something_happen_in_text_view=true; break;
                    case 10:  enter = true;                                                                                 break;
                    default:  getCharacter  (ch, line, _columnSelected, _editCommand); _something_happen_in_text_view=true;
                }
                bool showBigCommandWarning=false;

                while (_editCommand.at(0).size() > 32) {
                    _editCommand.at(0).pop_back();
                    showBigCommandWarning=true;
                }
                
                ShowConsoleCursor(false);
                gotoxy (0, TerminalLine - 1);
                setColor(32);
                cout<<"\e[1mcmd@edit: \e[0m";
                setColor(32);
                if (showBigCommandWarning) {
                    setColor(33);
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
            if (massageType == "accept") {
                setColor(32);
                gotoxy (44, TerminalLine - 1);
                cout<<"[+]";
            } else if (massageType == "command not found") {
                setColor(31);
                gotoxy (44, TerminalLine - 1);
                cout<<"[!]";
            } else if (massageType == "nothing found") {
                setColor(31);
                gotoxy (44, TerminalLine - 1);
                cout<<"[?]";
            }
            
        }

        void editLine() {
            editCommand();
            _editCommand.clear();
            gotoxy (10, TerminalLine - 1);
            cout<<"                                     ";

            if (_string_editCommand == "edit" || _string_editCommand == "E") {

                showMassage("accept");
                mode = "edit";
                printInfo();
                printText(input, -1, -1, -1);
            
            } else if (_string_editCommand == "text -D" || _string_editCommand == "text --delete") {

                showMassage("accept");
                clearEditFile();
                printInfo();
                printText(input, -1, -1, -1);
                ShowConsoleCursor (false);
                setColor(33);
                gotoxy (44, TerminalLine - 1);
                cout<<"[+]";

            } else if (_string_editCommand.find("text -S \"") == 0 || _string_editCommand.find("text --search \"") == 0) {
                string key;
                bool isKey=false;
                for (char ch : _string_editCommand) {
                    if (isKey && ch != '\"')
                        key += ch;
                    if (isKey && ch == '\"')
                        isKey=false;
                    if (ch == '\"')
                        isKey=true;
                }
                if (!search(key, input, lineSelected, columnSelected)) {
                    showMassage("nothing found");
                }
            } else if (_string_editCommand == "file -S" || _string_editCommand == "file --save") {
                mode = "file";
                if (!fileSystem("save", input)) {
                    system("clear");
                    printInfo();
                    printText(input, -1, -1, -1);
                }
                mode = "visual";
            } else {
                showMassage("command not found");
                gotoxy (10, TerminalLine - 1);
                _columnSelected=0;
                _string_editCommand="";
            }
        }
};