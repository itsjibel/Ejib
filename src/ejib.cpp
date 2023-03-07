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
    bool something_happen_in_text_view=false;
    #if (defined (_WIN32) || defined (_WIN64))
    switch (ch = getch())
    {
        case 0:
            case 0xE0:
                switch(ch = getch())
                {
                    case 72:
                        UP(lineSelected, columnSelected, input);
                        something_happen_in_text_view=true;
                        break;
                    case 75:
                        LEFT(lineSelected,    columnSelected, input);
                        something_happen_in_text_view=true;
                        break;
                    case 77:
                        RIGHT(lineSelected,   columnSelected, input);
                        something_happen_in_text_view=true;
                        break;
                    case 80:
                        DOWN(lineSelected, columnSelected, input);
                        something_happen_in_text_view=true;
                        break;
                    case 83:
                        DELETE(lineSelected, columnSelected, input);
                        something_happen_in_text_view=true;
                        break;
                    default:
                        getCharacter(ch, lineSelected, columnSelected, input);
                        something_happen_in_text_view=true;
                }
                break;
                case 8:
                    BACKSPACE(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                case 2:
                    mode = "visual";
                    break;
                case 13:
                    ENTER(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                case 9:
                    TAB(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                case 22:
                    PASTE(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
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
                    something_happen_in_text_view=true;
                    break;
                case 16:
                    mode = "command";
                    SetConsoleTextAttribute(hConsole, 7);
                    break;
                case 21:
                    UNDO(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                case 18:
                    REDO(lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
                    break;
                default:
                    INPUT_CHARACTER(ch, lineSelected, columnSelected, input);
                    something_happen_in_text_view=true;
    }
    #endif
    #if (defined (LINUX) || defined (__linux__))
    switch (ch = getch())
    {
        case 27:
            switch(ch = getch())
            {
                case 91:
                    switch(ch = getch())
                    {
                        case 65:
                            UP(currentLine, currentColumn, mainText);
                            something_happen_in_text_view=true;
                            break;
                        case 68:
                            LEFT(currentLine, currentColumn, mainText);
                            something_happen_in_text_view=true;
                            break;
                        case 67:
                            RIGHT(currentLine, currentColumn, mainText);
                            something_happen_in_text_view=true;
                            break;
                        case 66:
                            DOWN(currentLine, currentColumn, mainText);
                            something_happen_in_text_view=true;
                            break;
                        case 51:
                            switch(ch = getch())
                            {
                                case 126: 
                                    DELETE(currentLine, currentColumn, mainText, false);
                                    something_happen_in_text_view=true;
                                    break;
                                default: ;
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
                                                case 68:
                                                    QUICK_LEFT(currentLine, currentColumn, mainText);
                                                    something_happen_in_text_view=true;
                                                    break;
                                                case 67:
                                                    QUICK_RIGHT(currentLine, currentColumn, mainText);
                                                    something_happen_in_text_view=true;
                                                    break;
                                                case 65:
                                                    QUICK_UP(currentLine, currentColumn, mainText);
                                                    something_happen_in_text_view=true;
                                                    break;
                                                case 66:
                                                    QUICK_DOWN(currentLine, currentColumn, mainText);
                                                    something_happen_in_text_view=true;
                                                    break;
                                                default: ;
                                            }
                                            break;
                                        default: ;
                                    }
                                    break;
                                default: ;
                            }
                            break;
                        default: ;
                    }
                    break;
                default: ;
            }
            break;
        case 127:
            BACKSPACE(currentLine, currentColumn, mainText, false);
            something_happen_in_text_view=true;
            break;
        case 2:
            currentMode = "visual";
            break;
        case 10:
            ENTER(currentLine, currentColumn, mainText, false);
            something_happen_in_text_view=true;
            break;
        case 9:
            TAB(currentLine, currentColumn, mainText, false);
            something_happen_in_text_view=true;
            break;
        case 22:
            PASTE(currentLine, currentColumn, mainText);
            something_happen_in_text_view=true;
            break;
        case 6:
            currentMode = "file";
            fileSystem("save", mainText);
            ClearTerminalScreen();
            displayLocationInfo();
            printTabs();
            displayPageOfText(mainText, -1, -1, currentLine, currentColumn);
            currentMode = "edit";
            break;
        case 24:
            DELETE_LINE(currentLine, currentColumn, mainText, false);
            something_happen_in_text_view=true;
            break;
        case 16:
            currentMode = "command";
            ClearTerminalScreen();
            break;
        case 21:
            UNDO(currentLine, currentColumn, mainText);
            something_happen_in_text_view=true;
            break;
        case 18:
            REDO(currentLine, currentColumn, mainText);
            something_happen_in_text_view=true;
            break;
        default:
            INSERT_CHARACTER(ch, currentLine, currentColumn, mainText, false);
            something_happen_in_text_view=true;
    }
    #endif

    if (UpdateViewport())
        something_happen_in_text_view=true;

    if (something_happen_in_text_view)
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