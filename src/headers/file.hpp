#include <fstream>
#include <vector>

using std::cin;
using std::getline;
using std::ios;
using std::ifstream;
using std::ofstream;
using std::streampos;
using std::vector;

string filePath, fileName="untitled";

void modificationText (string &text)
{
    for (int i=0; i<text.size(); i++)
        if (text.at(i) == '\r' || text.at(i) == '\t') {
                if (text.at(i) == '\t') {
                    text.erase(text.begin() + i);
                    text.insert(text.begin() + i, {' ', ' ', ' ', ' '});
                } else text.erase(text.begin() + i);
            }
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
    protected:
        bool haveFilePath=false;
        bool agreeFileSize=true;

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
                    clearTerminal();
                    clearTerminal();
                    gotoxy (0, 0);
                    ColourizePrint("Enter file name(With extension): ", 2);
                    ShowConsoleCursor(true);
                    getline (cin, fileName);
                    ShowConsoleCursor(false);
                    ColourizePrint("Enter file path: ", 2);
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
                            ColourizePrint("[Path Error]: Unable to open file! (Make sure the path is correct)\n", 4);
                            ColourizePrint("[Warning]: Try again? [y/*] ", 6);
                            tryAgain = getch() == 'y' ? true : false;

                            if (!tryAgain)
                            {
                                fileName = "untitled";
                                return false;
                            }
                        }
                    }
                }

                if (mode == "save")
                {
                    ShowConsoleCursor(false);
                    if (saveFile (filePath, fileName, outputText) == true)
                    {
                        if (haveFilePath)
                        {
                            gotoxy (44, 29);
                            ColourizePrint("[+]", 4);
                            gotoxy(10, 29);
                        }
                        haveFilePath=true;
                        return true;
                    } else {
                        ColourizePrint("[Path Error]: Unable to open file! (Make sure the path is correct)\n", 4);
                        ColourizePrint("[Warning]: Try again? [y/*] ", 6);
                        tryAgain = getch() == 'y' ? true : false;

                        if (!tryAgain)
                        {
                            fileName = "untitled";
                            return false;
                        }

                        clearTerminal();
                    }
                    ShowConsoleCursor(true);
                }
            }
            fileName = "untitled";
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
    } else {
        fileName = "untitled";
        return false;
    }
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
        ColourizePrint("[Warning]: Are you sure you want to open a " + byteConverter((end-begin)) + " file? [y/*]", 6);
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
            fileName = "untitled";
            return false;
        }
    } else {
        fileName = "untitled";
        return false;
    }
}