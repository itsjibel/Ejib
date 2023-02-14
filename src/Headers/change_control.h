#include <string>
#include <vector>
#include <chrono>
#include <thread>
using std::vector;
using std::string;

struct Track
{
    bool isWirte;
    int startActionLine;
    int startActioncolumn;
    string changeString;
};
vector<Track> stack;
int currentTrack=0;

void AddTrack(bool isWirte, int &startActionLine, int &startActionColumn, string changeString) {
    Track TrackForAdd;
    TrackForAdd.changeString = changeString;
    TrackForAdd.startActioncolumn = startActionColumn;
    TrackForAdd.startActionLine = startActionLine;
    TrackForAdd.isWirte = isWirte;

    stack.push_back(TrackForAdd);
    currentTrack++;
}

Track GetTrack(int TrackNumber) {
    if (TrackNumber == -1)
        return stack.at(stack.size() - 1);
    else
        return stack.at(TrackNumber);
}

void controlUndoStack(int &line, int &column, string &changeString) {
    string tempChangeString;
    int startLine=0, startColumn=0;
    while (1) {
        if (tempChangeString != changeString) {
            AddTrack (true, startLine, startColumn, changeString);
            changeString = "";
            startColumn = column;
            startLine = line;
        }

        tempChangeString = changeString;
        #if (defined (_WIN32) || defined (_WIN64))
        Sleep(2000);
        #endif
        #if (defined (LINUX) || defined (__linux__))
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        #endif
    }
    return;
}