/* <<The mode controller of ejib>>
 * A summary of what this main file does:
 * The file <<ejib.cpp>> controls all the switching between modes such as
 * insert mode, visual mode, etc.
 * Also, all keyboard inputs for insert mode are taken in this file.
 */
#include "headers/visual_mode.hpp"

class EditorSystem: public VisualMode
{
    private:
        void TextEditSystem();

    public:
        void MainSystem()
        {
            /// Controlling executive functions for each mode
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
    /* In this section, we identify keyboard inputs using <getch()> and then
     * we call the function related to this key or keys.
     * Note two things:
     *      1.We separated the Windows and Linux input sections because inputs such as
     *        Enter, Backspace, etc. have different ASCII codes in the two operating systems,
     *        and this has caused us to separate the input of these two operating systems.
     *      2.The reason for the many switch cases is that when you press a special button such as
     *        Backspace, the Down, Left, Right and up Arrow keys, multiple ASCII codes are passed to <getch()>.
     *        To control this issue we need <getch()> stitch cases multiple times.
     */
    char ch;
    bool somethingHappenTextView=false;
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
                    DELETE_(currentLine, currentColumn, mainText, false);
                    somethingHappenTextView=true;
                    break;

                    case 59:
                    switch(ch = getch())
                    {
                        case 53:
                        switch(ch = getch())
                        {
                            case DELETE_KEY_LINUX:
                            QUICK_DELETE(currentLine, currentColumn, mainText, false);
                            somethingHappenTextView=true;
                            break;
                        }
                        break;
                    }
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

        case CTRL_BACKSPACE_KEY_LINUX:
        QUICK_BACKSPACE(currentLine, currentColumn, mainText, false);
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
        displayPageOfText(mainText, -1, -1);
        currentMode = "edit";
        break;

        case CTRL_X:
        DELETE_LINE(currentLine, currentColumn, mainText, false);
        somethingHappenTextView=true;
        break;

        case CTRL_P:
        gotoxy (10, numberOfTerminalLine - 1);
        ColorPrint ("[Warning]: Save the current file? [y/*] ", YELLOW);
        if (getch() == 'y')
        {
            currentMode = "file";
            fileSystem("save", mainText);
            ClearTerminalScreen();
            ShowConsoleCursor(true);
            currentMode = "command";
            ClearTerminalScreen();
        } else {
            ClearTerminalScreen();
            ShowConsoleCursor(true);
            currentMode = "command";
            ClearTerminalScreen();
        }
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
    /* The user may need to move the page down or up or right or left with the
     * change in the text user has made or movement the user did,
     * so we call this function so that if the user is a downer or upper or more right or more left than usual.
     * It will update the screen depending on it.
     */
    if (UpdateViewport())
        somethingHappenTextView=true;
    /* This <if(somethingHappenTextView)> means that if the user makes any changes to the location or text,
     * the page and other information should be printed.
     */
    if (somethingHappenTextView)
    {
        displayLocationInfo();
        printTabs();
        displayPageOfText(mainText, -1, -1);
    }
}

int main()
{
    ClearTerminalScreen();
    loadLogo();
	EditorSystem Ejib;
    std::thread ContorlViewportWithTerminalSize = std::thread (&VisualMode::AdjustingViewportWithSizeOfTerminal, Ejib);
    std::thread SetCurrentLineValueWithMouseWheelAction = std::thread (&InsertMode::setLineWithMouseWheelAction, Ejib);
	Ejib.MainSystem();
    ContorlViewportWithTerminalSize.join();
}