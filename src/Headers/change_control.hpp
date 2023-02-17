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

void AddTrackToUndoStack(bool isWirte, int startActionLine, int startActionColumn,
                         string changeString, string changeMode) {
    Track TrackForAdd;
    TrackForAdd.changeString = changeString;
    TrackForAdd.startActioncolumn = startActionColumn;
    TrackForAdd.startActionLine = startActionLine;
    TrackForAdd.isWirte = isWirte;
    TrackForAdd.changeMode = changeMode;

    UndoStack.push_back(TrackForAdd);
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
}

Track GetUndoTrack() {
    return UndoStack.at(UndoStack.size() - 1);
}

Track GetRedoTrack() {
    return RedoStack.at(RedoStack.size() - 1);
}

void undo(int &line, int &column, vector<vector<char>> &text) {
    if (UndoStack.size() > 0) {
        RedoStack.push_back(UndoStack.back());
        column = GetUndoTrack().startActioncolumn;
        line   = GetUndoTrack().startActionLine;
        vector<char> lastLineDeleted;
        bool isMultipleLine=false;

        if (GetUndoTrack().isWirte) {
            for (int i=0; i<GetUndoTrack().changeString.size(); i++) {
                if (GetUndoTrack().changeString.at(i) == '\n') {
                    lastLineDeleted = text.at(line + 1);
                    text.erase (text.begin() + line + 1);
                    isMultipleLine=true;
                }

                if (!isMultipleLine)
                    if (text.at(line).size() > 0)
                        text.at(line).erase (text.at(line).begin() + column);
            }

            for (char ch : lastLineDeleted)
                text.at(line).push_back(ch);

            if (isMultipleLine)
                if (GetRedoTrack().changeMode == "Paste") {
                    int j=GetRedoTrack().changeString.size()-1;

                    while (GetRedoTrack().changeString.at(j) != '\n') {
                        text.at(line).erase(text.at(line).begin() + column);
                        j--;
                    }
                }
        } else {
            for (int i=0; i<GetUndoTrack().changeString.size(); i++) {

                if (GetUndoTrack().changeString.at(i) == '\n') {
                    vector<char> StringToVector;
                    if (GetUndoTrack().changeMode != "Delete Line")
                        for (int j=0; j<GetUndoTrack().changeString.size() - 1; j++)
                            text.at(line).pop_back();

                    for (char ch : GetUndoTrack().changeString)
                        StringToVector.push_back(ch);
                    
                    StringToVector.erase(StringToVector.begin());
                    text.insert (text.begin() + line + 1, StringToVector);
                    if (GetUndoTrack().changeMode == "Backspace" || GetUndoTrack().changeMode == "Delete Line")
                        line++;
                    isMultipleLine=true;
                }
                if (!isMultipleLine) {
                    if (GetUndoTrack().changeMode == "Backspace")
                        text.at(line).insert (text.at(line).begin() + column - 1, GetUndoTrack().changeString.at(0));
                    else if (GetUndoTrack().changeMode == "Delete")
                        text.at(line).insert (text.at(line).begin() + column, GetUndoTrack().changeString.at(0));
                    else
                        text.at(line).insert (text.at(line).begin() + i, GetUndoTrack().changeString.at(i));
                }
            }
        }
        UndoStack.pop_back();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////

void redo(int &line, int &column, vector<vector<char>> &text) {
    if (RedoStack.size() > 0) {
        UndoStack.push_back(RedoStack.back());
        bool isMultipleLine=false, first_line=true;
        if (GetRedoTrack().isWirte) {
            int saveIndexLine=0;
            vector<char> secondPartOfString;

            for (int i=column; i<text.at(line).size(); i++)
                secondPartOfString.push_back(text.at(line).at(i));

            for (int i=0; i<secondPartOfString.size(); i++)
                text.at(line).pop_back();

            for (int i=0; i<GetRedoTrack().changeString.size(); i++) {
                if (GetRedoTrack().changeString.at(i) == '\n') {
                    vector<char> StringToVector;

                    for (int j=saveIndexLine; j<GetRedoTrack().changeString.size(); j++) {
                        if (GetRedoTrack().changeString.at(j) == '\n') {
                            saveIndexLine=j+1;
                            column = 0;
                            line = line + 1 <= text.size() - 1 ? line + 1 : line;
                            break;
                        } else
                            StringToVector.push_back(GetRedoTrack().changeString.at(j));
                    }

                    if (!first_line)
                        text.insert (text.begin() + line + 1, StringToVector);
                    else
                        first_line=false;

                    isMultipleLine=true;
                }

                if (!isMultipleLine) {
                    saveIndexLine = i+1;
                    text.at(line).insert (text.at(line).begin() + column, GetRedoTrack().changeString.at(i));
                }
                column++;
            }

            if (isMultipleLine) {
                if (GetRedoTrack().changeMode == "Paste") {
                    line+=2;
                    text.insert(text.begin() + line, secondPartOfString);
                    column--;
                    int j=GetRedoTrack().changeString.size()-1;

                    while (GetRedoTrack().changeString.at(j) != '\n') {
                        text.at(line).insert(text.at(line).begin(), GetRedoTrack().changeString.at(j));
                        j--;
                    }
                }
            } else {
                for (char ch : secondPartOfString)
                    text.at(line).push_back(ch);
            }
        } else {
            for (int i=0; i<GetRedoTrack().changeString.size(); i++) {
                if (text.at(line).size() > 0) {
                    if (GetRedoTrack().changeMode == "Backspace") {
                        column--;
                        text.at(line).erase (text.at(line).begin() + column);
                    } else if (GetRedoTrack().changeMode == "Delete") {
                        text.at(line).erase (text.at(line).begin() + column);
                    }
                }
            }
        }
        RedoStack.pop_back();
    }
}