#include "Headers/visual_commands.hpp"

#if (defined (_WIN32) || defined (_WIN64))
#include "Headers/mingw.thread.hpp"
#endif

class Ejib_System: public VisualCommand
{
    public:
        void runSystem() {
            while (1)
            {
                if (mode == "edit")
                    EDIT_SYSTEM();

                if (mode == "command")
                    commandLine();

                if (mode == "visual")
                {
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
                    setColor(44);
                    #endif
                    cout<<modeView;
                    setColor(0);
                    ShowConsoleCursor(true);
                    gotoxy (10, TerminalLine - 1);
                    VisualEdit();
                }
            }
        }
};
int main()
{
    #if (defined (_WIN32) || defined (_WIN64))
    system("cls");
    #endif
    #if (defined (LINUX) || defined (__linux__))
    system("clear");
    #endif
    loadLogo();
	Ejib_System Ejib;
    std::thread get_TerminalColumnTheread = std::thread(&Editor::reSizeTerminal, Ejib);
	Ejib.runSystem();
    get_TerminalColumnTheread.join();
}