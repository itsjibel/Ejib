#include <pthread.h>
#include "Headers/user_input.h"
using std::thread;
class Jibel: public EditCommand {
    private:
        bool _enterToCommantMode=false;

    public:
        void runSystem() {
            while (1) {
                if (mode == "edit") {
                    EDIT_SYSTEM();
                    _enterToCommantMode= true;
                }
                
                if (mode == "command") {
                    if (_enterToCommantMode)
                        system ("cls");
                    _enterToCommantMode=false;
                    commandLine();
                }

                if (mode == "visual") {
                    ShowConsoleCursor(false);
                    gotoxy (0, TerminalLine - 2);
                    string modeView;

                    for (int i=0; i<TerminalColumn/2-6; i++) modeView+=" ";
                    modeView += "-- VISUAL --";
                    for (int i=0; i<TerminalColumn/2-6; i++) modeView+=" ";

                    setColor(97);
                    cout<<modeView;
                    ShowConsoleCursor(true);
                    gotoxy (10, TerminalLine - 1);
                    editLine();
                }
            }
        }
};
int main() {
	Jibel Editor;
    thread TerminalColumnTheread = thread(&Editor::reSizeTerminal, Editor);
	Editor.runSystem();
    TerminalColumnTheread.join();
}