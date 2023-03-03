#include <fstream>
#include <vector>
#include "tools.hpp"

using std::cin;
using std::getline;
using std::ios;
using std::ifstream;
using std::ofstream;
using std::streampos;
using std::vector;

string CurrentFilePath, CurrentFileName="untitled";

void RemoveTextSpoilerCharacters (string &text)
{
    for (int i=0; i<text.size(); i++)
    {
        if (text.at(i) == '\r' ||
            text.at(i) == '\t' ||
            text.at(i) > 127)
        {
            if (text.at(i) == '\t')
            {
                text.erase(text.begin() + i);
                text.insert(text.begin() + i, {' ', ' ', ' ', ' '});
            } else
                text.erase(text.begin() + i);
        }
    }
}

class FileSystem
{
    protected:
        bool haveFilePath=false;
        bool agreeWithFileSize=true;

    public:
        bool saveFile(string filePath, string fileName, const vector<vector<char>> &text);
        bool loadFile(string filePath, string fileName, vector<vector<char>> &outputText);
        bool fileSystem (string mode, vector<vector<char>> &outputText);
};

bool FileSystem::saveFile(string filePath, string fileName, const vector<vector<char>> &text)
{
    ofstream FileForSave(filePath + fileName);
    if (FileForSave.is_open())
    {
        for (int lineNumber=0; lineNumber<text.size(); lineNumber++)
        {
            string line;

            for (char character : text.at(lineNumber))
                line += character;

            if (lineNumber != text.size() - 1)
                FileForSave<<line<<'\n';
            else
                FileForSave<<line;
        }
        FileForSave.close();
        return true;
    }
    fileName = "untitled";
    return false;
}

bool FileSystem::loadFile(string filePath, string fileName, vector<vector<char>> &MainText)
{
    string line;
    int lineNumber=0;
    ifstream FileForLoad (filePath + fileName);
    if (FileForLoad.is_open())
    {
        streampos begin, end;
        ifstream file (filePath + fileName, ios::binary);
        begin = file.tellg();
        file.seekg (0, ios::end);
        end = file.tellg();
        file.close();
        ColorPrint("[Warning]: Are you sure you want to open a " + byteConverter((end-begin)) + " file? [y/*]", 6);
        if (getch() == 'y')
        {
            vector<char> emptyVector;
            while (getline(FileForLoad, line))
            {
                MainText.push_back(emptyVector);
                RemoveTextSpoilerCharacters(line);

                for (char ch : line)
                    MainText.at(lineNumber).push_back(ch);

                lineNumber++;
            }
            FileForLoad.close();
            return true;
        } else {
            agreeWithFileSize=false;
            fileName = "untitled";
            return false;
        }
    } else {
        fileName = "untitled";
        return false;
    }
}

bool FileSystem::fileSystem (string SaveMode, vector<vector<char>> &MainText)
{
    bool tryAgain=true;
    while (tryAgain == true)
    {
        tryAgain = false;
        if (!haveFilePath)
        {
            ShowConsoleCursor(false);
            ClearTerminalScreen();
            ClearTerminalScreen();
            gotoxy (0, 0);
            ColorPrint("Enter file name(With extension): ", 2);
            ShowConsoleCursor(true);
            getline (cin, CurrentFileName);
            ShowConsoleCursor(false);
            ColorPrint("Enter file path: ", 2);
            ShowConsoleCursor(true);
            getline (cin, CurrentFilePath);
        }

        if (SaveMode == "open")
        {
            if (loadFile(CurrentFilePath, CurrentFileName, MainText) == true)
            {
                haveFilePath=true;
                return true;
            } else {
                if (!agreeWithFileSize)
                {
                    tryAgain=true;
                    agreeWithFileSize=true;
                } else {
                    ColorPrint("[Path Error]: Unable to open file! (Make sure the path is correct)\n", 4);
                    ColorPrint("[Warning]: Try again? [y/*] ", 6);
                    tryAgain = getch() == 'y' ? true : false;

                    if (!tryAgain)
                    {
                        CurrentFileName = "untitled";
                        return false;
                    }
                }
            }
        }

        if (SaveMode == "save")
        {
            ShowConsoleCursor(false);
            if (saveFile (CurrentFilePath, CurrentFileName, MainText) == true)
            {
                if (haveFilePath)
                {
                    gotoxy (44, 29);
                    ColorPrint("[+]", 4);
                    gotoxy(10, 29);
                }
                haveFilePath=true;
                return true;
            } else {
                ColorPrint("[Path Error]: Unable to open file! (Make sure the path is correct)\n", 4);
                ColorPrint("[Warning]: Try again? [y/*] ", 6);
                tryAgain = getch() == 'y' ? true : false;

                if (!tryAgain)
                {
                    CurrentFileName = "untitled";
                    return false;
                }

                ClearTerminalScreen();
            }
            ShowConsoleCursor(true);
        }
    }
    CurrentFileName = "untitled";
    return false;
}