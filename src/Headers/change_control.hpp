#include <string>
#include <vector>
using std::string;
using std::vector;

struct Track
{
    bool isWirte;
    int startActionLine;
    int startActioncolumn;
    string changeString;
    string changeMode;
};

vector<Track> UndoStack, RedoStack;
int currentUndoTrack=0, currentRedoTrack=0;



void AddTrackToUndoStack(bool isWirte, int startActionLine, int startActionColumn,
                         string changeString, string changeMode) {
    Track TrackForAdd;
    TrackForAdd.changeString = changeString;
    TrackForAdd.startActioncolumn = startActionColumn;
    TrackForAdd.startActionLine = startActionLine;
    TrackForAdd.isWirte = isWirte;
    TrackForAdd.changeMode = changeMode;

    UndoStack.push_back(TrackForAdd);
    currentUndoTrack++;
}

void AddTrackToUndoStack(bool isWirte, int startActionLine, int startActionColumn,
                         char changeCharacter, string changeMode) {
    Track TrackForAdd;
    TrackForAdd.changeString = changeCharacter;
    TrackForAdd.startActioncolumn = startActionColumn;
    TrackForAdd.startActionLine = startActionLine;
    TrackForAdd.isWirte = isWirte;
    TrackForAdd.changeMode = changeMode;

    UndoStack.push_back(TrackForAdd);
    currentUndoTrack++;
}

Track GetUndoTrack(int TrackNumber) {
    if (TrackNumber == -1)
        return UndoStack.back();
    else
        return UndoStack.at(TrackNumber);
}

Track GetRedoTrack(int TrackNumber) {
    if (TrackNumber == -1)
        return RedoStack.back();
    else
        return RedoStack.at(TrackNumber);
}

void undo(int &line, int &column, vector<vector<char>> &text) {
    if (currentUndoTrack > 0) {
        currentUndoTrack--;
        RedoStack.push_back(UndoStack.back());
        column = GetUndoTrack(currentUndoTrack).startActioncolumn;
        line   = GetUndoTrack(currentUndoTrack).startActionLine;

        bool isMultipleLine=false;

        if (GetUndoTrack(currentUndoTrack).isWirte) {
            for (int i=0; i<GetUndoTrack(currentUndoTrack).changeString.size(); i++) {
                if (GetUndoTrack(currentUndoTrack).changeString.at(i) == '\n') {
                    text.erase (text.begin() + line + 1);
                    isMultipleLine=true;
                }

                if (!isMultipleLine)
                    text.at(line).erase (text.at(line).begin() + column);
            }
        } else {
            for (int i=0; i<GetUndoTrack(currentUndoTrack).changeString.size(); i++) {

                if (GetUndoTrack(currentUndoTrack).changeString.at(i) == '\n') {
                    vector<char> StringToVector;
                    if (GetUndoTrack(currentUndoTrack).changeMode != "Delete Line")
                        for (int j=0; j<GetUndoTrack(currentUndoTrack).changeString.size() - 1; j++)
                            text.at(line).pop_back();

                    for (char ch : GetUndoTrack(currentUndoTrack).changeString)
                        StringToVector.push_back(ch);
                    
                    StringToVector.erase(StringToVector.begin());
                    text.insert (text.begin() + line + 1, StringToVector);
                    if (GetUndoTrack(currentUndoTrack).changeMode == "Backspace" || GetUndoTrack(currentUndoTrack).changeMode == "Delete Line")
                        line++;
                    isMultipleLine=true;
                }
                if (!isMultipleLine)
                    if (GetUndoTrack(currentUndoTrack).changeMode == "Backspace" || GetUndoTrack(currentUndoTrack).changeMode == "Delete")
                        text.at(line).insert (text.at(line).begin() + column - 1, GetUndoTrack(currentUndoTrack).changeString.at(0));
                    else
                        text.at(line).insert (text.at(line).begin() + i, GetUndoTrack(currentUndoTrack).changeString.at(i));
            }
        }
        UndoStack.pop_back();
        currentRedoTrack++;
    }
}



void redo(int &line, int &column, vector<vector<char>> &text) {
    if (currentRedoTrack > 0) {
        UndoStack.push_back(RedoStack.back());
        currentRedoTrack--;
        for (char ch : GetRedoTrack(currentRedoTrack).changeString) {
            column++;
            if (ch == '\n') {
                line++;
                column=0;
            }
        }
        int saveIndexLine=0;
        bool isMultipleLine=false;

        if (GetRedoTrack(currentRedoTrack).isWirte) {
            for (int i=0; i<GetRedoTrack(currentRedoTrack).changeString.size(); i++) {
                if (GetRedoTrack(currentRedoTrack).changeString.at(i) == '\n') {
                    vector<char> StringToVector;
                    
                    for (int j=saveIndexLine; j<GetRedoTrack(currentRedoTrack).changeString.size(); j++) {
                        StringToVector.push_back(GetRedoTrack(currentRedoTrack).changeString.at(j));
                        if (GetRedoTrack(currentRedoTrack).changeString.at(j) == '\n') {
                            saveIndexLine=j+1;
                            break;
                        }
                    }
                    
                    StringToVector.erase(StringToVector.begin());
                    text.insert (text.begin() + line, StringToVector);
                    isMultipleLine=true;
                }

                if (!isMultipleLine)
                    text.at(line).insert (text.at(line).begin() + column, GetRedoTrack(currentRedoTrack).changeString.at(i));
            }
        } else {
        }
        RedoStack.pop_back();
        currentUndoTrack++;
    }
}