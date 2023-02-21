#include "display_tools.hpp"
#include <fstream>
#include <vector>

using std::cin;
using std::getline;
using std::ios;
using std::ifstream;
using std::ofstream;
using std::streampos;
using std::vector;

#define pb 1125899906842624
#define tb 1099511627776
#define gb 1073741824
#define mb 1048576
#define kb 1024

string filePath, fileName="untitled";

void modificationText (string &text)
{
    for (int i=0; i<text.size(); i++)
        if (text.at(i) == '\r' || text.at(i) == '\t' ||
            text.at(i) == '\v' || text.at(i) == '\0' ||
            text.at(i) == '\f' || text.at(i) == '\a' ||
            text.at(i) == '\e' || text.at(i) == '\b')
            text.erase(text.begin() + i);
}

#if (defined (LINUX) || defined (__linux__))
#include <termios.h>

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
#endif

#if (defined (_WIN32) || defined (_WIN64))
#include <conio.h>
#endif

class File
{
    public:
        bool haveFilePath=false;
        bool agreeFileSize=true;
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

    public:
        bool saveFile(string filePath, string fileName, const vector<vector<char>> &text);
        bool loadFile(string filePath, string fileName, vector<vector<char>> &outputText);

        bool fileSystem (string mode, vector<vector<char>> &outputText)
        {
            bool tryAgain=true;
            while (tryAgain == true)
            {
                tryAgain = false;
                if (!haveFilePath)
                {
                    ShowConsoleCursor(false);

                    #if (defined (LINUX) || defined (__linux__))
                    system("clear");
                    #endif
                    #if (defined (_WIN32) || defined (_WIN64))
                    system("cls");
                    #endif

                    #if (defined (LINUX) || defined (__linux__))
                    setColor(32);
                    #endif
                    #if (defined (_WIN32) || defined (_WIN64))
                    setColor(2);
                    #endif
                    system("clear");
                    gotoxy (0, 0);
                    cout<<"Enter file name(With extension): ";
                    ShowConsoleCursor(true);
                    getline (cin, fileName);
                    ShowConsoleCursor(false);
                    cout<<"Enter file path: ";
                    ShowConsoleCursor(true);
                    getline (cin, filePath);
                }

                if (mode == "open")
                {
                    if (loadFile(filePath, fileName, outputText) == true)
                    {
                        haveFilePath=true;
                        return true;
                    } else {
                        if (!agreeFileSize)
                        {
                            tryAgain=true;
                            agreeFileSize=true;
                        } else {
                            #if (defined (LINUX) || defined (__linux__))
                            setColor(31);
                            #endif
                            #if (defined (_WIN32) || defined (_WIN64))
                            setColor(4);
                            #endif

                            cout<<"[Path Error]: Unable to open file! (Make sure the path is correct)\n";

                            #if (defined (LINUX) || defined (__linux__))
                            setColor(33);
                            #endif
                            #if (defined (_WIN32) || defined (_WIN64))
                            setColor(6);
                            #endif

                            cout<<"[Warning]: Try again? [y/*] ";
                            tryAgain = getch() == 'y' ? true : false;
                            if (!tryAgain) return false;
                        }
                    }
                }

                if (mode == "save")
                {
                    ShowConsoleCursor(false);
                    if (saveFile (filePath, fileName, outputText) == true)
                    {
                        #if (defined (LINUX) || defined (__linux__))
                        setColor(32);
                        #endif
                        #if (defined (_WIN32) || defined (_WIN64))
                        setColor(10);
                        #endif
                        if (haveFilePath)
                        {
                            #if (defined (LINUX) || defined (__linux__))
                            setColor(31);
                            #endif
                            #if (defined (_WIN32) || defined (_WIN64))
                            setColor(4);
                            #endif
                            gotoxy (44, 29);
                            cout<<"[+]";
                            gotoxy(10, 29);
                        }
                        haveFilePath=true;
                        return true;
                    } else {
                        #if (defined (LINUX) || defined (__linux__))
                        setColor(31);
                        #endif
                        #if (defined (_WIN32) || defined (_WIN64))
                        setColor(4);
                        #endif

                        cout<<"[Path Error]: Unable to open file! (Make sure the path is correct)\n";

                        #if (defined (LINUX) || defined (__linux__))
                        setColor(33);
                        #endif
                        #if (defined (_WIN32) || defined (_WIN64))
                        setColor(6);
                        #endif
                        
                        cout<<"[Warning]: Try again? [y/*] ";
                        tryAgain = getch() == 'y' ? true : false;
                        if (!tryAgain) return false;

                        #if (defined (LINUX) || defined (__linux__))
                        system("clear");
                        #endif
                        #if (defined (_WIN32) || defined (_WIN64))
                        system("cls");
                        #endif
                    }
                    ShowConsoleCursor(true);
                }
            }
            return false;
        }
};

bool File::saveFile(string filePath, string fileName, const vector<vector<char>> &text)
{
    ofstream myfile(filePath + fileName);
    if (myfile.is_open())
    {
        for (int lineNumber=0; lineNumber<text.size(); lineNumber++)
        {
            string line;

            for (char character : text.at(lineNumber))
                line += character;

            if (lineNumber != text.size() - 1)
                myfile<<line<<'\n';
            else
                myfile<<line;
        }
        myfile.close();
        return true;
    } else return false;
}

bool File::loadFile(string filePath, string fileName, vector<vector<char>> &outputText)
{
    string line;
    int lineNumber=0;
    ifstream myfile (filePath + fileName);
    if (myfile.is_open())
    {
        streampos begin, end;
        ifstream file (filePath + fileName, ios::binary);
        begin = file.tellg();
        file.seekg (0, ios::end);
        end = file.tellg();
        file.close();
        #if (defined (LINUX) || defined (__linux__))
        setColor(33);
        #endif
        #if (defined (_WIN32) || defined (_WIN64))
        setColor(6);
        #endif
        cout<<"[Warning]: Are you sure you want to open a "<<byteConverter((end-begin))<<" file? [y/*]";
        if (getch() == 'y')
        {
            vector<char> emptyVector;
            while (getline(myfile, line))
            {
                outputText.push_back(emptyVector);
                modificationText(line);

                for (char ch : line)
                    outputText.at(lineNumber).push_back(ch);

                lineNumber++;
            }
            myfile.close();
            return true;
        } else {
            agreeFileSize=false;
            return false;
        }
    } else return false;
}