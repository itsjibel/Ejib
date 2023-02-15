#include <string>
using std::string;

struct Track
{
    bool isWirte;
    int startActionLine;
    int startActioncolumn;
    string changeString;
    string changeMode;
};
vector<Track> stack;
int currentTrack=0;

void AddTrack(bool isWirte, int startActionLine, int startActionColumn, string changeString, string changeMode) {
    Track TrackForAdd;
    TrackForAdd.changeString = changeString;
    TrackForAdd.startActioncolumn = startActionColumn;
    TrackForAdd.startActionLine = startActionLine;
    TrackForAdd.isWirte = isWirte;
    TrackForAdd.changeMode = changeMode;

    stack.push_back(TrackForAdd);
    currentTrack++;
}

void AddTrack(bool isWirte, int startActionLine, int startActionColumn, char changeCharacter, string changeMode) {
    Track TrackForAdd;
    TrackForAdd.changeString = changeCharacter;
    TrackForAdd.startActioncolumn = startActionColumn;
    TrackForAdd.startActionLine = startActionLine;
    TrackForAdd.isWirte = isWirte;
    TrackForAdd.changeMode = changeMode;

    stack.push_back(TrackForAdd);
    currentTrack++;
}

Track GetTrack(int TrackNumber) {
    if (TrackNumber == -1)
        return stack.at(stack.size() - 1);
    else
        return stack.at(TrackNumber);
}