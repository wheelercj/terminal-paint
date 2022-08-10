#include "../ynot/ynot/ynot.h"
#include <string>
#include <vector>
using namespace std;
using ynot::CursorStyle;
using ynot::Coord;

HANDLE hStdin;
DWORD fdwSaveOldMode;

bool confirmed_dont_save();
void notify(string message, Coord window_size);
void show_help(Coord window_size);
bool open_canvas(vector<vector<string>>& canvas, Coord window_size, string& drawing_character, int& drawing_radius);
vector<vector<string>> init_canvas(Coord window_size);
void clear_canvas(vector<vector<string>>& canvas);
void load_canvas(vector<vector<string>>& canvas, Coord window_size);
void save_canvas(vector<vector<string>>& canvas);
void print_entire_canvas(vector<vector<string>>& canvas, Coord window_size);
bool show_canvas_menu(string& drawing_character, int& drawing_radius);
void draw(string output, COORD cursor_coord, int radius, vector<vector<string>>& canvas, Coord window_size);
void error_exit(string message);
void reset_terminal();


int main()
{
	ynot::alternate_screen_buffer();
	ynot::reset_on_keyboard_interrupt();
	ynot::Menu main_menu("terminal paint", { "paint", "help", "exit" });
	//ynot::Menu main_menu("terminal paint", { "paint", "load", "save", "clear", "help", "exit" });
	Coord window_size = ynot::get_window_size();
	vector<vector<string>> canvas = init_canvas(window_size);
	int drawing_radius = 1;
	string drawing_character = "█";

	string choice = "";
	bool saved = true;
	while (choice != "exit" /*|| (!saved && !confirmed_dont_save())*/)
	{
		choice = main_menu.run();
		ynot::alternate_screen_buffer();
		if (choice == "paint")
		{
			if (open_canvas(canvas, window_size, drawing_character, drawing_radius))
				saved = false;
		}
		else if (choice == "load" && (saved || confirmed_dont_save()))
		{
			load_canvas(canvas, window_size);
			if (open_canvas(canvas, window_size, drawing_character, drawing_radius))
				saved = false;
		}
		else if (choice == "save")
		{
			save_canvas(canvas);
			saved = true;
			notify("Canvas saved.", window_size);
		}
		else if (choice == "clear" && (saved || confirmed_dont_save()))
		{
			clear_canvas(canvas);
			saved = true;
			notify("Canvas cleared.", window_size);
		}
		else if (choice == "help")
		{
			show_help(window_size);
		}
	}

	reset_terminal();
	return 0;
}

bool confirmed_dont_save()
{
	ynot::Menu confirmation_menu("Save changes?", { "save", "don't save", "cancel" });
	string choice = confirmation_menu.run();
	ynot::alternate_screen_buffer();
	if (choice == "don't save")
		return true;
	else
		return false;
}

void notify(string message, Coord window_size)
{
	ynot::clear_screen();
	ynot::set_cursor_coords(1, 3);
	ynot::print(ynot::dedent(message));
	ynot::print("\n\n     Press any key to continue.");
	ynot::pause();
}

void show_help(Coord window_size)
{
	notify(R"(
		In the paint canvas:
		• press tab to open the brush selection menu 
		  and any other key to close it
		• press escape to return to the main menu
		• left click to draw and right click to erase
		• use the number keys to control the brush radius
		• for more help, join the discussions at
		  https://github.com/wheelercj/terminal-paint/discussions
		)", window_size);
}

/* Returns true if the canvas changed, false otherwise. */
bool open_canvas(vector<vector<string>>& canvas, Coord window_size, string& drawing_character, int& drawing_radius)
{
	bool canvas_changed = false;
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

	bool just_started = true;
	while (true)
	{
		if (!ReadConsoleInput(
			hStdin,      // input buffer handle
			irInBuf,     // buffer to read into
			128,         // size of read buffer
			&cNumRead)) // number of records read
			error_exit("ReadConsoleInput");
		if (just_started)
		{
			// If this program was started with a terminal command, the enter keypress
			// to enter the terminal command must be cleared by ignoring irInBuf once.
			just_started = false;
			continue;
		}

		// Dispatch the events to the appropriate handler.
		for (i = 0; i < cNumRead; i++)
		{
			switch (irInBuf[i].EventType)
			{
			case MOUSE_EVENT:
				MOUSE_EVENT_RECORD mer = irInBuf[i].Event.MouseEvent;
				COORD coord = mer.dwMousePosition;
				switch (mer.dwButtonState)
				{
				case 1:
					draw(drawing_character, coord, drawing_radius, canvas, window_size);
					canvas_changed = true;
					break;
				case 2:
					draw(" ", coord, drawing_radius, canvas, window_size);
					canvas_changed = true;
					break;
				}
				break;
			case KEY_EVENT:
				WCHAR key = irInBuf[i].Event.KeyEvent.uChar.UnicodeChar;
				if (key == '\x1b')  // escape
					return canvas_changed;
				if (key >= '1' and key <= '9')
					drawing_radius = key - '0';
				else if (key == '\t')
				{
					if (!show_canvas_menu(drawing_character, drawing_radius))
						return canvas_changed;
					ynot::clear_screen();
					print_entire_canvas(canvas, window_size);
				}
			}
		}
	}
}

vector<vector<string>> init_canvas(Coord window_size)
{
	vector<vector<string>> canvas;
	canvas.resize(size_t(window_size.y + 10));
	for (size_t y = 0; y < canvas.size(); y++)
	{
		canvas[y].resize(size_t(window_size.x + 10));
		for (size_t x = 0; x < canvas[y].size(); x++)
			canvas[y][x] = " ";
	}
	return canvas;
}

void clear_canvas(vector<vector<string>>& canvas)
{
	for (size_t y = 0; y < canvas.size(); y++)
	{
		for (size_t x = 0; x < canvas[y].size(); x++)
			canvas[y][x] = " ";
	}
}

void load_canvas(vector<vector<string>>& canvas, Coord window_size)
{
	// TODO
}

void save_canvas(vector<vector<string>>& canvas)
{
	// TODO
}

void print_entire_canvas(vector<vector<string>>& canvas, Coord window_size)
{
	for (int y = 0; y < window_size.y; y++)
	{
		for (int x = 0; x < window_size.x; x++)
			ynot::print_at(x, y, canvas[y][x]);
	}
}

/* Returns true if returning to the canvas, false if returning to the main menu. */
bool show_canvas_menu(string& drawing_character, int& drawing_radius)
{
	ynot::clear_screen();
	ynot::print_at(1, 1, ynot::dedent(R"canvas_menu(
        Press tab to open this menu and any other key to close it.
        Press escape to return to the main menu.
        In the canvas, left click to draw and right click to erase.
        The number keys control the brush radius.

        Press a key to choose a symbol to draw with:

        │ ─ ┼ ┴ ├ ┬ ┤ ┘ └ ┌ ┐ *
        q w e r t y u i o p [ ]

        ║ ═ ╬ ╩ ╠ ╦ ╣ ╝ ╚ ╔ ╗
        a s d f g h j k l ; '

        █ ▓ ▒ ▄ ▀ ■ ≡ . ·
        z x c v b n m , .
        )canvas_menu"));
	string key = "tab";
	while (key == "tab")
	{
		key = ynot::get_key();
		if (key == "escape")
			return false;
		try
		{
			drawing_radius = stoi(key);
		}
		catch (std::invalid_argument) {}
		catch (std::out_of_range) {}
		if (key == "q")
			drawing_character = "│";
		else if (key == "w")
			drawing_character = "─";
		else if (key == "e")
			drawing_character = "┼";
		else if (key == "r")
			drawing_character = "┴";
		else if (key == "t")
			drawing_character = "├";
		else if (key == "y")
			drawing_character = "┬";
		else if (key == "u")
			drawing_character = "┤";
		else if (key == "i")
			drawing_character = "┘";
		else if (key == "o")
			drawing_character = "└";
		else if (key == "p")
			drawing_character = "┌";
		else if (key == "[")
			drawing_character = "┐";
		else if (key == "]")
			drawing_character = "*";
		else if (key == "a")
			drawing_character = "║";
		else if (key == "s")
			drawing_character = "═";
		else if (key == "d")
			drawing_character = "╬";
		else if (key == "f")
			drawing_character = "╩";
		else if (key == "g")
			drawing_character = "╠";
		else if (key == "h")
			drawing_character = "╦";
		else if (key == "j")
			drawing_character = "╣";
		else if (key == "k")
			drawing_character = "╝";
		else if (key == "l")
			drawing_character = "╚";
		else if (key == ";")
			drawing_character = "╔";
		else if (key == "'")
			drawing_character = "╗";
		else if (key == "z")
			drawing_character = "█";
		else if (key == "x")
			drawing_character = "▓";
		else if (key == "c")
			drawing_character = "▒";
		else if (key == "v")
			drawing_character = "▄";
		else if (key == "b")
			drawing_character = "▀";
		else if (key == "n")
			drawing_character = "■";
		else if (key == "m")
			drawing_character = "≡";
		else if (key == ",")
			drawing_character = ".";
		else if (key == ".")
			drawing_character = "·";
	}

	return true;
}

void draw(string output, COORD cursor_coord, int radius, vector<vector<string>>& canvas, Coord window_size)
{
	for (int x = cursor_coord.X - radius + 1; x <= cursor_coord.X + radius - 1; x++) {
		for (int y = cursor_coord.Y - radius + 1; y <= cursor_coord.Y + radius - 1; y++) {
			if (window_size.y > y && window_size.x > x && y >= 0 && x >= 0)
			{
				canvas[size_t(y) + 1][size_t(x) + 1] = output;
				ynot::print_at(x + 1, y + 1, output);
			}
		}
	}
}

void error_exit(string message)
{
	ynot::print("Error: " + message);
	ynot::sleep_(5000);
	reset_terminal();
	ExitProcess(0);
}

void reset_terminal()
{
	SetConsoleMode(hStdin, fdwSaveOldMode);  // Restore input mode on exit.
	ynot::set_cursor_style(CursorStyle::not_hidden);
	ynot::restore_screen_buffer();
}
