#include "headers/visual_commands.hpp"

#if (defined (_WIN32) || defined (_WIN64))
#include "headers/mingw.thread.hpp"
#endif


class EditorSystem: public VisualCommand
{
    private:
        void EDIT_SYSTEM();

    public:
        void SYSTEM() {
            while (1)
            {
                if (mode == "edit")
                    EDIT_SYSTEM();

                if (mode == "command")
                    COMMAND_LINE();

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

void EditorSystem::EDIT_SYSTEM()
{
    int ch;
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
                            UP(lineSelected, columnSelected, input);
                            something_happen_in_text_view=true;
                            break;
                        case 68:
                            LEFT(lineSelected, columnSelected, input);
                            something_happen_in_text_view=true;
                            break;
                        case 67:
                            RIGHT(lineSelected, columnSelected, input);
                            something_happen_in_text_view=true;
                            break;
                        case 66:
                            DOWN(lineSelected, columnSelected, input);
                            something_happen_in_text_view=true;
                            break;
                        case 51:
                            switch(ch = getch())
                            {
                                case 126: 
                                    DELETE(lineSelected, columnSelected, input, false);
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
                                                    QUICK_LEFT(lineSelected, columnSelected, input);
                                                    something_happen_in_text_view=true;
                                                    break;
                                                case 67:
                                                    QUICK_RIGHT(lineSelected, columnSelected, input);
                                                    something_happen_in_text_view=true;
                                                    break;
                                                case 65:
                                                    QUICK_UP(lineSelected, columnSelected, input);
                                                    something_happen_in_text_view=true;
                                                    break;
                                                case 66:
                                                    QUICK_DOWN(lineSelected, columnSelected, input);
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
            BACKSPACE(lineSelected, columnSelected, input, false);
            something_happen_in_text_view=true;
            break;
        case 2:
            mode = "visual";
            break;
        case 10:
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
            printText(input, -1, -1, lineSelected, columnSelected);
            mode = "edit";
            break;
        case 24:
            DELETE_LINE(lineSelected, columnSelected, input, false);
            something_happen_in_text_view=true;
            break;
        case 16:
            mode = "command";
            clearTerminal();
            break;
        case 21:
            UNDO(1, lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        case 18:
            REDO(1, lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        case 25:
            UNDO(5, lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        case 5:
            REDO(5, lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
            break;
        default:
            INPUT_CHARACTER(ch, lineSelected, columnSelected, input);
            something_happen_in_text_view=true;
    }
    #endif

    if (updateViewport())
        something_happen_in_text_view=true;

    if (something_happen_in_text_view)
    {
        open=0;
        NumberOfOpenScopes=0;
        NumberOfCloseScopes=0;
        scopesAreScaned=false;
        printInfo();
        printText(input, -1, -1, lineSelected, columnSelected);
        scopesAreScaned=true;

        if (ScopeCharacterIsClose != tempScopeCharacterIsClose ||
            ScopeCharacterIsOpen != tempScopeCharacterIsOpen)
        {
            open=0;
            printText(input, -1, -1, lineSelected, columnSelected);
            open=0;
            printText(input, -1, -1, lineSelected, columnSelected);
        }

        tempScopeCharacterIsClose = ScopeCharacterIsClose;
        tempScopeCharacterIsOpen = ScopeCharacterIsOpen;
    }
}

int main()
{
    clearTerminal();
    loadLogo();
	EditorSystem Ejib;
    std::thread ContorlViewportWithTerminalSize = std::thread(&Editor::AdjustingViewportWithSizeOfTerminal, Ejib);
    std::thread DisplayConsumption = std::thread(&display_MEM_USAGE, std::ref(mode),
                                                 std::ref(TerminalColumn), std::ref(TerminalLine));
	Ejib.SYSTEM();
    ContorlViewportWithTerminalSize.join();
    DisplayConsumption.join();
}