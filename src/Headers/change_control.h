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

void undo(int &line, int &column, vector<vector<char>> &text) {
    if (currentTrack > 0) {
        currentTrack--;
        column = GetTrack(currentTrack).startActioncolumn;
        line   = GetTrack(currentTrack).startActionLine;

        bool isMultipleLine=false;

        if (GetTrack(currentTrack).isWirte) {
            for (int i=0; i<GetTrack(currentTrack).changeString.size(); i++) {
                if (GetTrack(currentTrack).changeString.at(i) == '\n') {
                    text.erase (text.begin() + line + 1);
                    isMultipleLine=true;
                }

                if (!isMultipleLine)
                    text.at(line).erase (text.at(line).begin() + column);
            }
        } else {
            for (int i=0; i<GetTrack(currentTrack).changeString.size(); i++) {

                if (GetTrack(currentTrack).changeString.at(i) == '\n') {
                    vector<char> StringToVector;
                    if (GetTrack(currentTrack).changeMode != "Delete Line")
                        for (int j=0; j<GetTrack(currentTrack).changeString.size() - 1; j++)
                            text.at(line).pop_back();

                    for (char ch : GetTrack(currentTrack).changeString)
                        StringToVector.push_back(ch);
                    
                    StringToVector.erase(StringToVector.begin());
                    text.insert (text.begin() + line + 1, StringToVector);
                    if (GetTrack(currentTrack).changeMode == "Backspace" || GetTrack(currentTrack).changeMode == "Delete Line")
                        line++;
                    isMultipleLine=true;
                }
                if (!isMultipleLine)
                    if (GetTrack(currentTrack).changeMode == "Backspace" || GetTrack(currentTrack).changeMode == "Delete")
                        text.at(line).insert (text.at(line).begin() + column - 1, GetTrack(currentTrack).changeString.at(0));
                    else
                        text.at(line).insert (text.at(line).begin() + i, GetTrack(currentTrack).changeString.at(i));
            }
        }
        stack.pop_back();
    }
}