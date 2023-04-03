/* <<Tools to make things faster and easier>>
 * A summary of what this library does:
 * In this library, we have prepared big and small tools in the functions,
 * so we don't have to rewrite the tool code every time.
 */
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <linux/input.h>
#include <string>
#include <sys/ioctl.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <X11/Xlib.h>
/// Defining normal colors and full bright colors codes
#define BLUE 1
#define GREEN 2
#define AQUA 3
#define RED 4
#define PURPLE 5
#define YELLOW 6
#define WHITE 7
#define GRAY 8
#define BBLUE 9
#define BGREEN 10
#define BAQUA 11
#define BRED 12
#define BPURPLE 13
#define BYELLOW 14
#define BWHITE 15
#define BGRAY 16
#define RED_BACKGROUND 79
#define YELLOW_BACKGROUND 97

/// Defining normal colors and full bright colors strings
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
/// Defining kilobytes, megabytes, etc. to byte units
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

void gotoxy(int x, int y)
{
    printf("\033[%d;%dH", y + 1, x + 1);
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
    /// Translate Windows color codes to Linux color codes
    ColorCode = ColorCode == BLUE ? 34 : ColorCode;
    ColorCode = ColorCode == GREEN ? 32 : ColorCode;
    ColorCode = ColorCode == AQUA ? 36 : ColorCode;
    ColorCode = ColorCode == RED ? 31 : ColorCode;
    ColorCode = ColorCode == PURPLE ? 35 : ColorCode;
    ColorCode = ColorCode == YELLOW ? 33 : ColorCode;
    ColorCode = ColorCode == WHITE ? 37 : ColorCode;
    ColorCode = ColorCode == GRAY ? 90 : ColorCode;
    ColorCode = ColorCode == BBLUE ? 94 : ColorCode;
    ColorCode = ColorCode == BGREEN ? 92 : ColorCode;
    ColorCode = ColorCode == BAQUA ? 96 : ColorCode;
    ColorCode = ColorCode == BRED ? 91 : ColorCode;
    ColorCode = ColorCode == BPURPLE ? 95 : ColorCode;
    ColorCode = ColorCode == BYELLOW ? 93 : ColorCode;
    ColorCode = ColorCode == BWHITE ? 97 : ColorCode;
    ColorCode = ColorCode == BGRAY ? 100 : ColorCode;
    ColorCode = ColorCode == RED_BACKGROUND ? 41 : ColorCode;
    ColorCode = ColorCode == YELLOW_BACKGROUND ? 44 : ColorCode;
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


bool GetCopiedText(string &copiedText)
{
    Display *display = XOpenDisplay(NULL);
    unsigned long color = BlackPixel(display, DefaultScreen(display));
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, color, color);
    Bool Clipboard_Info_Received = GetLinuxClipboard(display, window, "CLIPBOARD", "UTF8_STRING", copiedText) ||
                                   GetLinuxClipboard(display, window, "CLIPBOARD", "STRING", copiedText);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return Clipboard_Info_Received;
}

template <class T>
void ColorPrint(T text, int color)
{
    setColor(color);
    cout<<text;
    /// Set color to default]
    setColor(0);
}

vector<int> GetTerminal_LineAndColumn()
{
    vector<int> TerminalSize;
    struct winsize w;
    ioctl (STDOUT_FILENO, TIOCGWINSZ, &w);
    TerminalSize.push_back(w.ws_row);
    TerminalSize.push_back(w.ws_col);
    return TerminalSize;
}

void loadLogo()
{
    string line;
    ifstream TextLogoFile ("/opt/ejib/EjibTextLogo.txt");
    if (TextLogoFile.is_open())
    {
        while (getline(TextLogoFile, line))
        {
            bool ColorIsChangeable=true;
            for (char ch : line)
            {
                /// Setting color to each character for better logo viewability
                if (ColorIsChangeable)
                {
                    if (ch == 'E')
                        setColor(BYELLOW);
                    else if (ch == 'B')
                        setColor(BRED);
                    else if (ch == 'J' || ch == 'I' || ch == '#' || ch == '=')
                        setColor(BPURPLE);
                    else if (ch == '|' || ch == '_')
                        setColor(BLUE);
                    else if (ch == '(') {
                        ColorIsChangeable=false;
                        setColor(YELLOW);
                    } else if (ch == '\'') {
                        ColorIsChangeable=false;
                        setColor(AQUA);
                    }
                    else
                        setColor(0);
                }
                else if (ch == '|')
                    setColor(BLUE);
                else if (ch == 'v')
                    setColor(WHITE);
                else if (ch == '\'')
                    setColor(AQUA);

                cout<<ch;
            }
            cout<<'\n';
        }
    } else
        ColorPrint("[Path Error]: Unable to open logo file\n", RED);
}

bool IsSeparatorCharacter (char character)
{
    if (character == ' ' || character == '.' ||
        character == '(' || character == ')' ||
        character == '{' || character == '}' ||
        character == '[' || character == ']' ||
        character == '\''|| character == '\"'||
        character == '<' || character == '>' ||
        character == ':' || character == ';' ||
        character == '+' || character == '-' ||
        character == '/' || character == '*' ||
        character == '^' || character == '=')
        return true;
    else
        return false;
}

void ClearTerminalScreen()
{
    system("clear");
}