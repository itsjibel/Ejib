#include "Headers/user_input.h"

#if (defined (_WIN32) || defined (_WIN64))
#include "Headers/mingw.thread.h"
#endif

using std::thread;

class Ejib_System: public EditCommand {
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
                        system ("clear");
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
                    #if (defined (_WIN32) || defined (_WIN64))
                    setColor(97);
                    #endif
                    #if (defined (LINUX) || defined (__linux__))
                    setColor(37);
                    setColor(43);
                    #endif
                    cout<<modeView;
                    setColor(0);
                    ShowConsoleCursor(true);
                    gotoxy (10, TerminalLine - 1);
                    editLine();
                }
            }
        }
};
int main() {
	Ejib_System Ejib;
    std::thread get_TerminalColumnTheread = std::thread(&Editor::reSizeTerminal, Ejib);
	Ejib.runSystem();
    get_TerminalColumnTheread.join();
}