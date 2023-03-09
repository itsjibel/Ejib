#include "headers/visual_mode.hpp"

#if (defined (_WIN32) || defined (_WIN64))
#include "headers/mingw.thread.hpp"
#endif

class EditorSystem: public VisualMode
{
    private:
        void TextEditSystem();
        enum keyASCII {
            ESCAPE_KEY      = 27,
            SPECIAL_KEYS    = 91,
            UP_ARROW_KEY    = 65,
            DOWN_ARROW_KEY  = 66,
            RIGHT_ARROW_KEY = 67,
            LEFT_ARROW_KEY  = 68,
            DELETE_KEY      = 126,
            BACKSPACE_KEY   = 127,
            ENTER_KEY       = 10,
            TAB_KEY         = 9,
            CTRL_B          = 2,
            CTRL_V          = 22,
            CTRL_F          = 6,
            CTRL_X          = 24,
            CTRL_P          = 16,
            CTRL_U          = 21,
            CTRL_R          = 18
            };

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
        case 0:
            case 0xE0:
                switch(ch = getch())
                {
                    case 72:
                        UP(lineSelected, columnSelected, input);
                        somethingHappenTextView=true;
                        break;
                    case 75:
                        LEFT(lineSelected,    columnSelected, input);
                        somethingHappenTextView=true;
                        break;
                    case 77:
                        RIGHT(lineSelected,   columnSelected, input);
                        somethingHappenTextView=true;
                        break;
                    case 80:
                        DOWN(lineSelected, columnSelected, input);
                        somethingHappenTextView=true;
                        break;
                    case 83:
                        DELETE(lineSelected, columnSelected, input);
                        somethingHappenTextView=true;
                        break;
                    default:
                        getCharacter(ch, lineSelected, columnSelected, input);
                        somethingHappenTextView=true;
                }
                break;
                case 8:
                    BACKSPACE(lineSelected, columnSelected, input);
                    somethingHappenTextView=true;
                    break;
                case 2:
                    mode = "visual";
                    break;
                case 13:
                    ENTER(lineSelected, columnSelected, input);
                    somethingHappenTextView=true;
                    break;
                case 9:
                    TAB(lineSelected, columnSelected, input);
                    somethingHappenTextView=true;
                    break;
                case 22:
                    PASTE(lineSelected, columnSelected, input);
                    somethingHappenTextView=true;
                    break;
                case 6:
                    mode = "file";
                    fileSystem("save", input);
                    clearTerminal();
                    printInfo();
                    printTabs();
                    printText(input, -1, -1, lineSelected);
                    mode = "visual";
                    break;
                case 24:
                    DELETE_LINE(lineSelected, columnSelected, input);
                    somethingHappenTextView=true;
                    break;
                case 16:
                    mode = "command";
                    SetConsoleTextAttribute(hConsole, 7);
                    break;
                case 21:
                    UNDO(lineSelected, columnSelected, input);
                    somethingHappenTextView=true;
                    break;
                case 18:
                    REDO(lineSelected, columnSelected, input);
                    somethingHappenTextView=true;
                    break;
                default:
                    INPUT_CHARACTER(ch, lineSelected, columnSelected, input);
                    somethingHappenTextView=true;
    }
    #endif
    #if (defined (LINUX) || defined (__linux__))
    switch (ch = getch())
    {
        case ESCAPE_KEY:
            switch(ch = getch())
            {
                case SPECIAL_KEYS:
                    switch(ch = getch())
                    {
                        case UP_ARROW_KEY:
                            UP(currentLine, currentColumn, mainText);
                            somethingHappenTextView=true;
                            break;
                        case DOWN_ARROW_KEY:
                            DOWN(currentLine, currentColumn, mainText);
                            somethingHappenTextView=true;
                            break;
                        case RIGHT_ARROW_KEY:
                            RIGHT(currentLine, currentColumn, mainText);
                            somethingHappenTextView=true;
                            break;
                        case LEFT_ARROW_KEY:
                            LEFT(currentLine, currentColumn, mainText);
                            somethingHappenTextView=true;
                            break;
                        case 51:
                            switch(ch = getch())
                            {
                                case DELETE_KEY:
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
                                                case UP_ARROW_KEY:
                                                    QUICK_UP(currentLine, currentColumn, mainText);
                                                    somethingHappenTextView=true;
                                                    break;
                                                case DOWN_ARROW_KEY:
                                                    QUICK_DOWN(currentLine, currentColumn, mainText);
                                                    somethingHappenTextView=true;
                                                    break;
                                                case RIGHT_ARROW_KEY:
                                                    QUICK_RIGHT(currentLine, currentColumn, mainText);
                                                    somethingHappenTextView=true;
                                                    break;
                                                case LEFT_ARROW_KEY:
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
        case BACKSPACE_KEY:
            BACKSPACE(currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
            break;
        case CTRL_B:
            currentMode = "visual";
            break;
        case ENTER_KEY:
            ENTER(currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
            break;
        case TAB_KEY:
            TAB(currentLine, currentColumn, mainText, false);
            somethingHappenTextView=true;
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