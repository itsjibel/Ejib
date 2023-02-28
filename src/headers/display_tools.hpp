#include <fstream>
#include <iostream>
#include <limits.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include <X11/Xlib.h>

#if (defined (LINUX) || defined (__linux__))
#define RST  "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST
#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST
#endif

using std::cout;
using std::cerr;
using std::ifstream;
using std::string;
using std::to_string;
using std::vector;

#if (defined (_WIN32) || defined (_WIN64))
#include <windows.h>

HANDLE hConsole = GetStdHandle (STD_OUTPUT_HANDLE);
void gotoxy (SHORT x, SHORT y)
{
    SetConsoleCursorPosition (hConsole, COORD {x, y});
}

void ShowConsoleCursor (bool showFlag)
{
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo (hConsole, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo (hConsole, &cursorInfo);
}

void setColor(int ColorCode)
{
    SetConsoleTextAttribute (hConsole, ColorCode);
}
#endif

#if (defined (LINUX) || defined (__linux__))
void gotoxy(int x, int y)
{
    printf("%c[%d;%df", 0x1B, y + 1, x + 1);
}

void setColor(int ColorCode)
{
    ColorCode = ColorCode == 1 ? 34 : ColorCode;
    ColorCode = ColorCode == 2 ? 32 : ColorCode;
    ColorCode = ColorCode == 3 ? 36 : ColorCode;
    ColorCode = ColorCode == 4 ? 31 : ColorCode;
    ColorCode = ColorCode == 5 ? 35 : ColorCode;
    ColorCode = ColorCode == 6 ? 33 : ColorCode;
    ColorCode = ColorCode == 7 ? 37 : ColorCode;
    ColorCode = ColorCode == 8 ? 90 : ColorCode;
    ColorCode = ColorCode == 9 ? 94 : ColorCode;
    ColorCode = ColorCode == 10 ? 92 : ColorCode;
    ColorCode = ColorCode == 11 ? 96 : ColorCode;
    ColorCode = ColorCode == 12 ? 91 : ColorCode;
    ColorCode = ColorCode == 13 ? 95 : ColorCode;
    ColorCode = ColorCode == 14 ? 93 : ColorCode;
    ColorCode = ColorCode == 15 ? 97 : ColorCode;
    ColorCode = ColorCode == 16 ? 100 : ColorCode;
    cout<<"\033[" + to_string (ColorCode) + "m";
}

void ShowConsoleCursor (bool showFlag)
{
    if (showFlag)
        printf("\e[?25h");
    else
        printf("\e[?25l");
}
#endif

Bool PrintSelection(Display *display, Window window, const char *bufname,
                    const char *fmtname, string &text)
{
    char *result;
    unsigned long ressize, restail;
    int resbits;
    Atom bufid  = XInternAtom(display, bufname,     False),
         fmtid  = XInternAtom(display, fmtname,     False),
         propid = XInternAtom(display, "XSEL_DATA", False),
         incrid = XInternAtom(display, "INCR",      False);
    XEvent event;

    XConvertSelection(display, bufid, fmtid, propid, window, CurrentTime);

    do
    {
        XNextEvent(display, &event);
    } while (event.type != SelectionNotify || event.xselection.selection != bufid);

    if (event.xselection.property)
    {
        XGetWindowProperty(display, window, propid, 0, LONG_MAX/4, False, AnyPropertyType,
                           &fmtid, &resbits, &ressize, &restail, (unsigned char**)&result);

        if (fmtid == incrid)
            printf("Buffer is too large and INCR reading is not implemented yet.\n");
        else
            text = result;

        XFree(result);
        return True;
    } else
        return False;
}

bool GetCopiedText(string &copiedText)
{
    #if (defined (LINUX) || defined (__linux__))
    Display *display = XOpenDisplay(NULL);
    unsigned long color = BlackPixel(display, DefaultScreen(display));
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0,0, 1,1, 0, color, color);
    Bool result = PrintSelection(display, window, "CLIPBOARD", "UTF8_STRING", copiedText) || PrintSelection(display, window, "CLIPBOARD", "STRING", copiedText);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return result;
    #endif
    #if (defined (_WIN32) || defined (_WIN64))
    if (OpenClipboard(NULL))
    {
        string copiedText = (char*)GetClipboardData(CF_TEXT);
        CloseClipboard();
        return true;
    } else return false;
    #endif
}

vector<int> getTerminaLine_Column()
{
    vector<int> TerminalSize;
    #if (defined (_WIN32) || defined (_WIN64))
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    TerminalSize.push_back(csbi.srWindow.Right - csbi.srWindow.Left + 1);
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    TerminalSize.push_back(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
    #endif
    #if (defined (LINUX) || defined (__linux__))
    struct winsize w;
    ioctl (STDOUT_FILENO, TIOCGWINSZ, &w);
    TerminalSize.push_back(w.ws_row);
    TerminalSize.push_back(w.ws_col);
    #endif
    return TerminalSize;
}

void loadLogo()
{
    string line;
    ifstream myfile ("../EjibTextLogo.txt");
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            bool ColorIsChangable=true;
            for (char ch : line) {
                if (ColorIsChangable)
                    if (ch == 'E')
                        setColor(14);
                    else if (ch == 'B')
                        setColor(12);
                    else if (ch == 'J' || ch == 'I' || ch == '#' || ch == '=')
                        setColor(13);
                    else if (ch == '|' || ch == '_')
                        setColor(1);
                    else if (ch == '(') {
                        ColorIsChangable=false;
                        setColor(6);
                    } else if (ch == '\'') {
                        ColorIsChangable=false;
                        setColor(3);
                    }
                    else
                        setColor(0);
                else if (ch == '|')
                    setColor(1);
                else if (ch == 'v')
                    setColor(7);
                else if (ch == '\'')
                    setColor(3);
                cout<<ch;
            }
            cout<<'\n';
        }
    } else {
        setColor(4);
        cerr<<"[Path Error]: Unable to open logo file\n";
    }
}

bool IsSeparatorCharacter (char character) {
    if
    (
        character == ' ' || character == '.' ||
        character == '(' || character == ')' ||
        character == '{' || character == '}' ||
        character == '[' || character == ']' ||
        character == '\''|| character == '\"'||
        character == '<' || character == '>' ||
        character == ':' || character == ';' ||
        character == '+' || character == '-' ||
        character == '/' || character == '*' ||
        character == '^' || character == '='
    )
        return true;
    else
        return false;
}

bool ScopeCharacterIsOpen=false, ScopeCharacterIsClose=false,
     tempScopeCharacterIsClose=false, tempScopeCharacterIsOpen=false,
     scopesAreScaned=false;
int CloseScopeIndex=0, CloseScopeLine=0,
    OpenScopeIndex=0, OpenScopeLine=0,
    NumberOfOpenScopes=0, NumberOfCloseScopes=0,
    open=0;

void colourizeText (const string &text, const int &selectedCharacterStart, const int &selectedCharacterEnd,
                    const int &selectedLine, const int &currentLine, const int &column)
{
    string textPart;
    int8_t color=7, tempColor=7;
    int index=0;
    bool sharpArea=false,   quotationArea=false, angleBracketsArea=false,
         commentArea=false, selectedArea=false;
    char temp;
    for (char character : text)
    {
        if ((index == selectedCharacterStart || index == selectedCharacterEnd)
            && selectedLine == currentLine)
        {
            selectedArea = !selectedArea;
            #if (defined (_WIN32) || defined (_WIN64))
            color = 79;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 41;
            #endif
        } else if ((character == '/' || character == '*' || character == '-' ||
                    character == '+' || character == '^' || character == '%' ||
                    character == '=' || character == '<' || character == '>') &&
                    !quotationArea && !commentArea && !selectedArea) {

            if (temp == '/' && character == '/')
                commentArea=true;

            #if (defined (_WIN32) || defined (_WIN64))
            color = 4;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 31;
            #endif

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
        } else if ((character == '(' || character == ')' || character == '{' ||
                    character == '}' || character == '[' || character == ']') &&
                    !quotationArea && !angleBracketsArea && !commentArea &&
                    !selectedArea) {
            if ((column == index && selectedLine == currentLine) && character == '{')
            {
                color = 46;
                if (column == index && selectedLine == currentLine)
                {
                    ScopeCharacterIsOpen = true;
                    OpenScopeIndex = index;
                    OpenScopeLine = currentLine;
                }
            } else if (((column == index && selectedLine == currentLine) ||
                        (ScopeCharacterIsOpen && NumberOfOpenScopes == 1)) &&
                        character == '}') {
                if (ScopeCharacterIsOpen)
                {
                    if (OpenScopeLine <= currentLine)
                    {
                        if (OpenScopeLine == currentLine)
                        {
                            if (OpenScopeIndex < index)
                            {
                                color = 46;
                                ScopeCharacterIsOpen=false;
                            } else {
                                color = 36;
                            }
                        } else {
                            color = 46;
                            ScopeCharacterIsOpen=false;
                        }
                    } else {
                        color = 36;
                    }
                } else {
                    color = 46;
                    if (column == index && selectedLine == currentLine)
                    {
                        ScopeCharacterIsClose = true;
                        CloseScopeIndex = index;
                        CloseScopeLine = currentLine;
                    }
                }
            } else
                color = 36;

            if (character == '{')
            {
                if (ScopeCharacterIsOpen && selectedLine <= currentLine)
                {
                    if (selectedLine == currentLine)
                    {
                        if (index >= OpenScopeIndex)
                        {
                            tempScopeCharacterIsOpen = !ScopeCharacterIsOpen;
                            NumberOfOpenScopes++;
                        }
                    } else
                        NumberOfOpenScopes++;
                }
                if (selectedLine >= currentLine)
                {
                    if (selectedLine == currentLine)
                    {
                        if (index <= CloseScopeIndex)
                        {
                            tempScopeCharacterIsClose = !ScopeCharacterIsClose;
                            if (!scopesAreScaned)
                                NumberOfCloseScopes++;
                            open++;
                        }
                    } else {
                        tempScopeCharacterIsClose = !ScopeCharacterIsClose;
                        if (!scopesAreScaned)
                            NumberOfCloseScopes++;
                        open++;
                    }
                }
            }

            if (character == '}')
            {
                if (ScopeCharacterIsOpen && selectedLine <= currentLine)
                {
                    if (selectedLine == currentLine)
                    {
                        if (index > OpenScopeIndex)
                        {
                            NumberOfOpenScopes--;
                        }
                    } else
                        NumberOfOpenScopes--;
                }
                if (selectedLine >= currentLine)
                {
                    if (selectedLine == currentLine)
                    {
                        if (index <= CloseScopeIndex)
                        {
                            tempScopeCharacterIsClose = !ScopeCharacterIsClose;
                            if (!scopesAreScaned)
                                NumberOfCloseScopes--;
                        }
                    } else {
                        tempScopeCharacterIsClose = !ScopeCharacterIsClose;
                        if (!scopesAreScaned)
                            NumberOfCloseScopes--;
                    }
                } 
            }

            if (ScopeCharacterIsClose && NumberOfCloseScopes == open - 1 && scopesAreScaned)
            {
                if (CloseScopeLine >= currentLine)
                {
                    if (CloseScopeLine == currentLine)
                    {
                        if (CloseScopeIndex > index)
                        {
                            color = 46;
                            ScopeCharacterIsClose=false;
                        } else {
                            if (CloseScopeIndex == index)
                                color = 46;
                            else
                                color = 36;
                        }
                    } else {
                        color = 46;
                        ScopeCharacterIsClose=false;
                    }
                } else {
                    color = 36;
                }
            }

            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        } else if ((character == '@' || character == '#' || character == '!' ||
                    character == '~' || character == '&' || character == '|' ||
                    character == '$' || character == '?') && !quotationArea &&
                    !angleBracketsArea && !commentArea && !selectedArea) {

            #if (defined (_WIN32) || defined (_WIN64))
            color = 13;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 35;
            #endif

            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        } else if ((character == '0' || character == '1' || character == '2' ||
                    character == '3' || character == '4' || character == '5' ||
                    character == '6' || character == '7' || character == '8' ||
                    character == '9') && !quotationArea && !angleBracketsArea &&
                    !commentArea && !selectedArea) {

            #if (defined (_WIN32) || defined (_WIN64))
            color = 2;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 32;
            #endif

            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        } else if ((character == '.' || character == ',' || character == ';' ||
                    character == ':' || character == '/' || character == '\\') &&
                    !quotationArea && !angleBracketsArea && !commentArea &&
                    !selectedArea) {

            #if (defined (_WIN32) || defined (_WIN64))
            color = 9;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 34;
            #endif

            sharpArea=false;
            angleBracketsArea=false;
            quotationArea=false;
        } else if (character == '\'' || character == '\"' && !angleBracketsArea &&
                !commentArea && !selectedArea) {

            #if (defined (_WIN32) || defined (_WIN64))
            color = 6;
            #endif
            #if (defined (LINUX) || defined (__linux__))
            color = 33;
            #endif

            sharpArea=false;
            if (character == '\"' || character == '\'') {

                if (quotationArea)
                    quotationArea = false;
                else
                    for (int i=index + 1; i<text.size(); i++)
                        if (text.at(i) == '\"' || text.at(i) == '\'')
                        {
                            quotationArea = !quotationArea;
                            break;
                        }

            }

        } else {
            if (!sharpArea && !quotationArea && !angleBracketsArea && !commentArea && !selectedArea)
            {
                #if (defined (_WIN32) || defined (_WIN64))
                color = 7;
                #endif
                #if (defined (LINUX) || defined (__linux__))
                color = 37;
                #endif
            }
        }
        sharpArea = character == '#' ? true : sharpArea;

        if (color != tempColor)
        {
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

void clearTerminal() {
    #if (defined (_WIN32) || defined (_WIN64))
    system("cls");
    #endif
    #if (defined (LINUX) || defined (__linux__))
    system("clear");
    #endif
}