#include "headers/visual_mode.hpp"

#if (defined (_WIN32) || defined (_WIN64))
#include "headers/mingw.thread.hpp"
#endif

class EditorSystem: public VisualMode
{
    private:
        void TextEditSystem();

    public:
        void MainSystem()
        {
            while (1)
            {
                if (currentMode == "edit")
                    TextEditSystem();
                else if (currentMode == "command")
                    CommandsRouter();
                else if (currentMode == "visual")
                    VisualEdit();
            }
        }
};

void EditorSystem::TextEditSystem()
{
    char ch;
    bool somethingHappenTextView=false;
    #if (defined (_WIN32) || defined (_WIN64))
    switch (ch = getch())
    {
        case SPECIAL_KEYS_WINDOWS:
            switch(ch = getch())
            {
                case UP_ARROW_KEY_WINDOWS:
                    UP(currentLine, currentColumn, mainText);
                    somethingHappenTextView=true;
                    break;
                case LEFT_ARROW_KEY_WINDOWS:
                    LEFT(currentLine, currentColumn, mainText);
                    somethingHappenTextView=true;
                    break;
                case RIGHT_ARROW_KEY_WINDOWS:
                    RIGHT(currentLine,   currentColumn, mainText);
                    somethingHappenTextView=true;
                    break;
                case DOWN_ARROW_KEY_WINDOWS:
                    DOWN(currentLine, currentColumn, mainText);
                    somethingHappenTextView=true;
                    break;
                case DELETE_KEY_WINDOWS:
                    DELETE_(currentLine, currentColumn, mainText, false);
                    somethingHappenTextView=true;
                    break;
                default:
                    INSERT_CHARACTER(ch, currentLine, currentColumn, mainText, false);
                    somethingHappenTextView=true;
            }
            break;
        case BACKSPACE_KEY_WINDOWS:
            BACKSPACE(currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
            break;
        case ENTER_KEY_WINDOWS:
            ENTER(currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
            break;
        case TAB_KEY:
            TAB(currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
            break;
        case CTRL_B:
            currentMode = "visual";
            break;
        case CTRL_V:
            PASTE(currentLine, currentColumn, mainText);
            somethingHappenTextView=true;
            break;
        case CTRL_F:
            currentMode = "file";
            fileSystem("save", mainText);
            ClearTerminalScreen();
            displayLocationInfo();
            printTabs();
            displayPageOfText(mainText, -1, -1, currentLine, currentColumn);
            currentMode = "edit";
            break;
        case CTRL_X:
            DELETE_LINE(currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
            break;
        case CTRL_P:
            currentMode = "command";
            ClearTerminalScreen();
            break;
        case CTRL_U:
            UNDO(currentLine, currentColumn, mainText);
            somethingHappenTextView=true;
            break;
        case CTRL_R:
            REDO(currentLine, currentColumn, mainText);
            somethingHappenTextView=true;
            break;
        default:
            INSERT_CHARACTER(ch, currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
    }
    #endif
    #if (defined (LINUX) || defined (__linux__))
    switch (ch = getch())
    {
        case ESCAPE_KEY:
            switch(ch = getch())
            {
                case SPECIAL_KEYS_LINUX:
                    switch(ch = getch())
                    {
                        case UP_ARROW_KEY_LINUX:
                            UP(currentLine, currentColumn, mainText);
                            somethingHappenTextView=true;
                            break;
                        case DOWN_ARROW_KEY_LINUX:
                            DOWN(currentLine, currentColumn, mainText);
                            somethingHappenTextView=true;
                            break;
                        case RIGHT_ARROW_KEY_LINUX:
                            RIGHT(currentLine, currentColumn, mainText);
                            somethingHappenTextView=true;
                            break;
                        case LEFT_ARROW_KEY_LINUX:
                            LEFT(currentLine, currentColumn, mainText);
                            somethingHappenTextView=true;
                            break;
                        case 51:
                            switch(ch = getch())
                            {
                                case DELETE_KEY_LINUX:
                                    DELETE(currentLine, currentColumn, mainText, false);
                                    somethingHappenTextView=true;
                                    break;
                            }
                            break;
                        case 49:
                            switch(ch = getch())
                            {
                                case 59: 
                                    switch(ch = getch())
                                    {
                                        case 53:
                                            switch(ch = getch())
                                            {
                                                case UP_ARROW_KEY_LINUX:
                                                    QUICK_UP(currentLine, currentColumn, mainText);
                                                    somethingHappenTextView=true;
                                                    break;
                                                case DOWN_ARROW_KEY_LINUX:
                                                    QUICK_DOWN(currentLine, currentColumn, mainText);
                                                    somethingHappenTextView=true;
                                                    break;
                                                case RIGHT_ARROW_KEY_LINUX:
                                                    QUICK_RIGHT(currentLine, currentColumn, mainText);
                                                    somethingHappenTextView=true;
                                                    break;
                                                case LEFT_ARROW_KEY_LINUX:
                                                    QUICK_LEFT(currentLine, currentColumn, mainText);
                                                    somethingHappenTextView=true;
                                                    break;
                                            }
                                            break;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
            }
            break;
        case BACKSPACE_KEY_LINUX:
            BACKSPACE(currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
            break;
        case ENTER_KEY_LINUX:
            ENTER(currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
            break;
        case TAB_KEY:
            TAB(currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
            break;
        case CTRL_B:
            currentMode = "visual";
            break;
        case CTRL_V:
            PASTE(currentLine, currentColumn, mainText);
            somethingHappenTextView=true;
            break;
        case CTRL_F:
            currentMode = "file";
            fileSystem("save", mainText);
            ClearTerminalScreen();
            displayLocationInfo();
            printTabs();
            displayPageOfText(mainText, -1, -1, currentLine, currentColumn);
            currentMode = "edit";
            break;
        case CTRL_X:
            DELETE_LINE(currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
            break;
        case CTRL_P:
            currentMode = "command";
            ClearTerminalScreen();
            break;
        case CTRL_U:
            UNDO(currentLine, currentColumn, mainText);
            somethingHappenTextView=true;
            break;
        case CTRL_R:
            REDO(currentLine, currentColumn, mainText);
            somethingHappenTextView=true;
            break;
        default:
            INSERT_CHARACTER(ch, currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
    }
    #endif

    if (UpdateViewport())
        somethingHappenTextView=true;

    if (somethingHappenTextView)
    {
        displayLocationInfo();
        printTabs();
        displayPageOfText(mainText, -1, -1, currentLine, currentColumn);
    }
}

int main()
{
    ClearTerminalScreen();
    loadLogo();
	EditorSystem Ejib;
    std::thread ContorlViewportWithTerminalSize = std::thread (&InsertMode::AdjustingViewportWithSizeOfTerminal, Ejib);
	Ejib.MainSystem();
    ContorlViewportWithTerminalSize.join();
}
