/* <<File system of Ejib>>
 * A summary of what this library does:
 * The functions of this library provide all the tasks that an editor can do with the file
 */
#include <fstream>
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
    /* These characters are the characters that violate the editor system.
     * For example, a tab is considered a character,
     * and this causes problems in moving forward and backwards in the text and displaying the text
     */
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
        bool saveFile(string filePath, string fileName, const vector<string> &text);
        bool loadFile(string filePath, string fileName, vector<string> &outputText);
        bool fileSystem (string mode, vector<string> &outputText);
};

bool FileSystem::saveFile(string filePath, string fileName, const vector<string> &text)
{
    /// Write vector<string> into a file
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

bool FileSystem::loadFile(string filePath, string fileName, vector<string> &MainText)
{
    /// Load string text into a vector<string>
    string line;
    int lineNumber=0;
    ifstream FileForLoad (filePath + fileName);
    if (FileForLoad.is_open())
    {
        /// Calculate the size of the file
        streampos begin, end;
        ifstream file (filePath + fileName, ios::binary);
        begin = file.tellg();
        file.seekg (0, ios::end);
        end = file.tellg();
        file.close();
        ColorPrint("[Warning]: Are you sure you want to open a " + byteConverter((end-begin)) + " file? [y/*]", YELLOW);

        if (getch() == 'y')
        {
            string emptyString;
            while (getline(FileForLoad, line))
            {
                /// Write the file line by line on the main text
                MainText.push_back(emptyString);
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

bool FileSystem::fileSystem (string SaveMode, vector<string> &MainText)
{
    /// Control all of the file actions
    bool tryAgain=true;
    while (tryAgain == true)
    {
        tryAgain = false;
        /* When the location of the file is empty, we need to get it accurately,
         * but when the user gives us the location of the file when opening the file or saving the file,
         * there is no need to get it again.
         */
        if (!haveFilePath)
        {
            ShowConsoleCursor(false);
            ClearTerminalScreen();
            ClearTerminalScreen();
            gotoxy (0, 0);
            ColorPrint("Enter file name(With extension): ", GREEN);
            ShowConsoleCursor(true);
            getline (cin, CurrentFileName);
            ShowConsoleCursor(false);
            ColorPrint("Enter file path: ", GREEN);
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
                    ColorPrint("[Path Error]: Unable to open file! (Make sure the path is correct)\n", RED);
                    ColorPrint("[Warning]: Try again? [y/*] ", YELLOW);
                    tryAgain = getch() == 'y' ? true : false;

                    if (!tryAgain)
                    {
                        CurrentFileName = "untitled";
                        return false;
                    }
                }
            }
        }
        else if (SaveMode == "save")
        {
            ShowConsoleCursor(false);
            if (saveFile (CurrentFilePath, CurrentFileName, MainText) == true)
            {
                if (haveFilePath)
                {
                    gotoxy (44, 29);
                    ColorPrint("[+]", GREEN);
                    gotoxy(10, 29);
                }
                haveFilePath=true;
                return true;
            } else {
                ColorPrint("[Path Error]: Unable to open file! (Make sure the path is correct)\n", RED);
                ColorPrint("[Warning]: Try again? [y/*] ", YELLOW);
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

void DisplayHelpFile(string helpFileName)
{
    /// Display the specific help file in colour(for better readability)
    string line;
    ifstream HelpFile("/opt/ejib/help/" + helpFileName);

    if (HelpFile.is_open())
    {
        while(getline (HelpFile, line))
        {
            string SwitchName, explanation;

            for (char ch : line)
                if (ch != ']')
                    SwitchName += ch;
                else {
                    SwitchName += ch;
                    break;
                }

            bool startGivingExplain = false;

            for (char ch : line)
                if (startGivingExplain)
                    explanation += ch;
                else if (ch == ']')
                    startGivingExplain=true;

            ColorPrint('\t' + SwitchName, YELLOW);
            ColorPrint(explanation + '\n', PURPLE);
        }
        HelpFile.close();
    } else
        ColorPrint("[Path Error]: Unable to open \'" + helpFileName + "' file!\n", RED);
}