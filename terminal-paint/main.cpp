#include "../ynot/ynot/ynot.h"
#include <string>
using namespace std;
using ynot::CursorStyle;

HANDLE hStdin;
DWORD fdwSaveOldMode;

void draw(string output, COORD cursor_coord, int radius=1);
void KeyEventProc(KEY_EVENT_RECORD);
void error_exit(string message);
void reset_terminal();

int main() {
    bool just_started = true;
    ynot::alternate_screen_buffer();
    ynot::set_cursor_style(CursorStyle::hidden);
    DWORD cNumRead, fdwMode, i;
    INPUT_RECORD irInBuf[128];

    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE)
        error_exit("GetStdHandle");

    // Save the current input mode, to be restored on exit.
    if (!GetConsoleMode(hStdin, &fdwSaveOldMode))
        error_exit("GetConsoleMode");

    // Enable the window and mouse input events.
    fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
    if (!SetConsoleMode(hStdin, fdwMode))
        error_exit("SetConsoleMode");

    int drawing_radius = 1;
    string drawing_character = "*";
    while (true) {
        if (!ReadConsoleInput(
            hStdin,      // input buffer handle
            irInBuf,     // buffer to read into
            128,         // size of read buffer
            &cNumRead)) // number of records read
            error_exit("ReadConsoleInput");
        if (just_started) {
            // If this program was started with a terminal command, the enter keypress
            // to enter the terminal command must be cleared by ignoring irInBuf once.
            just_started = false;
            continue;
        }

        // Dispatch the events to the appropriate handler.
        for (i = 0; i < cNumRead; i++) {
            switch (irInBuf[i].EventType) {
            case MOUSE_EVENT:
                MOUSE_EVENT_RECORD mer = irInBuf[i].Event.MouseEvent;
                COORD coord = mer.dwMousePosition;
                switch (mer.dwButtonState) {
                case 1:
                    draw(drawing_character, coord, drawing_radius);
                    break;
                case 2:
                    draw(" ", coord, drawing_radius);
                    break;
                }
                break;
            case KEY_EVENT:
                WCHAR key = irInBuf[i].Event.KeyEvent.uChar.UnicodeChar;
                if (key == '\x1b')  // escape
                {
                    reset_terminal();
                    return 0;
                }
                if (key >= '1' and key <= '9')
                    drawing_radius = key - '0';
                /*else if (key == 'a')
                    drawing_character = "*";
                else if (key == 'b')
                    drawing_character = "█";*/
            }
        }
    }

    reset_terminal();
    return 0;
}

void draw(string output, COORD cursor_coord, int radius) {
    ynot::Coord window_size = ynot::get_window_size();
    for (int x = cursor_coord.X - radius + 1; x <= cursor_coord.X + radius - 1; x++) {
        for (int y = cursor_coord.Y - radius + 1; y <= cursor_coord.Y + radius - 1; y++) {
            if (window_size.y > y && window_size.x > x && y >= 0 && x >= 0)
                ynot::print_at(x + 1, y + 1, output);
        }
    }
}

void error_exit(string message) {
    ynot::print("Error:" + message);
    ynot::sleep_(5000);
    reset_terminal();
    ExitProcess(0);
}

void reset_terminal() {
    SetConsoleMode(hStdin, fdwSaveOldMode);  // Restore input mode on exit.
    ynot::set_cursor_style(CursorStyle::not_hidden);
    ynot::restore_screen_buffer();
}
