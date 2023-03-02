#include <fstream>
#include <iostream>
#include <limits.h>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include <X11/Xlib.h>
#include <thread>

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

#define pb 1125899906842624
#define tb 1099511627776
#define gb 1073741824
#define mb 1048576
#define kb 1024

using std::cout;
using std::ifstream;
using std::string;
using std::to_string;
using std::vector;

#if (defined (_WIN32) || defined (_WIN64))
#include <windows.h>
#include <conio.h>

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
#include <termios.h>

void gotoxy(int x, int y)
{
    printf("%c[%d;%df", 0x1B, y + 1, x + 1);
}

static struct termios old, current;
// Initialize new terminal i/o settings 
void initTermios(int echo)
{
    tcgetattr(0, &old); // grab old terminal i/o settings
    current = old; // make new settings same as old settings
    current.c_lflag &= ~ICANON; // disable buffered i/o
    if (echo)
        current.c_lflag |= ECHO; // set echo mode
    else
        current.c_lflag &= ~ECHO; // set no echo mode
    tcsetattr(0, TCSANOW, &current); // use these new terminal i/o settings now
}

// Restore old terminal i/o settings
void resetTermios(void)
{
    tcsetattr(0, TCSANOW, &old);
}

// Read 1 character - echo defines echo mode
char getch_(int echo)
{
    char ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return ch;
}

// Read 1 character without echo
char getch(void)
{
    return getch_(0);
}

// Read 1 character with echo
char getche(void)
{
    return getch_(1);
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

void Sleep(int milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}
#endif

string byteConverter(long long int bytes)
{
    string convertedSize;
    if (bytes >= pb)
        return to_string ((float)bytes/pb) + " PB";

    else if (bytes >= tb && bytes < pb)
        return to_string ((float)bytes/tb) + " TB";

    else if (bytes >= gb && bytes < tb)
        return to_string ((float)bytes / gb) + " GB";

    else if (bytes >= mb && bytes < gb)
        return to_string ((float)bytes/mb) + " MB";

    else if (bytes >= kb && bytes < mb)
        return to_string ((float)bytes/kb) + " KB";

    else if (bytes < kb)
        return to_string(bytes) + " Bytes";

    return "Unknown size";
}

Bool GetLinuxClipboard(Display *display, Window window, const char *bufname,
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
    Bool Clipboard_Info_Received = GetLinuxClipboard(display, window, "CLIPBOARD", "UTF8_STRING", copiedText) ||
                  GetLinuxClipboard(display, window, "CLIPBOARD", "STRING", copiedText);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return Clipboard_Info_Received;
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

template <class T>
void ColorPrint(T text, int color)
{
    setColor(color);
    cout<<text;
    setColor(0);
}

vector<int> GetTerminal_LineAndColumn()
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
    ifstream TextLogoFile ("../EjibTextLogo.txt");
    if (TextLogoFile.is_open())
    {
        while (getline(TextLogoFile, line))
        {
            bool ColorIsChangable=true;
            for (char ch : line)
            {
                if (ColorIsChangable)
                {
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
                }
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
    } else
        ColorPrint("[Path Error]: Unable to open logo file\n", 4);
}

bool IsSeparatorCharacter (char character)
{
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

void ClearTerminalScreen() {
    #if (defined (_WIN32) || defined (_WIN64))
    system("cls");
    #endif
    #if (defined (LINUX) || defined (__linux__))
    system("clear");
    #endif
}