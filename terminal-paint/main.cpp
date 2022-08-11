#include "../ynot/ynot/ynot.h"
#include <map>
#include <string>
#include <vector>
using namespace std;
using ynot::CursorStyle;
using ynot::Coord;

HANDLE hStdin;
DWORD fdwSaveOldMode;

bool confirmed_dont_save();
void show_help();
bool open_canvas(vector<vector<string>>& canvas, Coord window_size, string& drawing_character, int& drawing_radius);
vector<vector<string>> init_canvas(Coord window_size);
void clear_canvas(vector<vector<string>>& canvas);
void load_canvas(vector<vector<string>>& canvas, Coord window_size);
void save_canvas(vector<vector<string>>& canvas);
void print_entire_canvas(vector<vector<string>>& canvas, Coord window_size);
bool show_canvas_menu(string& drawing_character);
void print_canvas_menu(const map<string, vector<string>>& char_map);
bool show_char_menu(vector<string> char_options, string& drawing_character);
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
			ynot::Notification("Canvas saved.").run();
		}
		else if (choice == "clear" && (saved || confirmed_dont_save()))
		{
			clear_canvas(canvas);
			saved = true;
			ynot::Notification("Canvas cleared.").run();
		}
		else if (choice == "help")
		{
			show_help();
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

void show_help()
{
	ynot::Notification(R"(
		terminal paint
		
		In the paint canvas:
		• press tab to open the brush selection menu 
		  and any other key to close it
		• press escape to return to the main menu
		• left click to draw and right click to erase
		• use the number keys to control the brush radius
		• for more help, join the discussions at
		  https://github.com/wheelercj/terminal-paint/discussions
		)").run();
	ynot::alternate_screen_buffer();
}

/* Returns true if the canvas changed, false otherwise. */
bool open_canvas(vector<vector<string>>& canvas, Coord window_size, string& drawing_character, int& drawing_radius)
{
	print_entire_canvas(canvas, window_size);
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
					if (!show_canvas_menu(drawing_character))
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
bool show_canvas_menu(string& drawing_character)
{
	map<string, vector<string>> char_map = {
		{ "0", { "choose with your keyboard" }},
		{ "1", { "█", "▓", "▒", "▐", "▄", "▌", "▀", "■", "▬" }},
		{ "2", { "…", "·", "•", "◘", "○", "◙", "°", "º", "☼", "ø" }},
		{ "3", { "│", "─", "┼", "┴", "├", "┬", "┤", "┘", "└", "┌", "┐" }},
		{ "4", { "║", "═", "╬", "╩", "╠", "╦", "╣", "╝", "╚", "╔", "╗" }},
		{ "5", { "╡", "╞", "╢", "╟", "╪", "╫", "╧", "╨", "╤", "╥", "╘", "╙", "╒", "╓", "╕", "╖", "╛", "╜" }},
		{ "6", { "↑", "↓", "↕", "↨", "←", "→", "↔", "◄", "►", "«", "»", "‹", "›" }},
		{ "7", { "¤", "¢", "€", "£", "¥", "₧", "©", "®", "™" }},
		{ "8", { "±", "×", "≤", "≥", "≈", "≡", "√", "⌠", "⌡", "∟", "⌐", "¬" }},
		{ "9", { "☺", "☻", "♥", "♦", "♣", "♠", "♪", "♫", "⌂", "∩", "¦", "†", "‡" }},
		{ "a", { "α", "ß", "Γ", "π", "Σ", "σ", "µ", "τ", "Φ", "Θ", "Ω", "δ", "∞", "φ", "ε" }}
	};
	print_canvas_menu(char_map);
	while (true)
	{
		string key = ynot::get_key();
		if (key == "escape")
			return false;
		if (char_map.count(key) == 0)
			continue;
		else
			if (show_char_menu(char_map[key], drawing_character))
				return true;
			else
				print_canvas_menu(char_map);
	}
}

void print_canvas_menu(const map<string, vector<string>>& char_map)
{
	ynot::clear_screen();
	string canvas_menu_str = ynot::dedent(R"canvas_menu(
		Press tab to open this menu and any other key to close it.
		Press escape to return to the main menu.
		In the canvas:
		• left click to draw and right click to erase
		• the number keys control the brush radius
		
		To change the drawing character, choose a category:
		
		)canvas_menu");
	for (const auto& row : char_map)
	{
		string row_str = "\x1b[42m" + row.first + "│\x1b[0m";
		for (const string& cell : row.second)
			row_str += " " + cell;
		canvas_menu_str += row_str + "\n";
	}
	ynot::print_at(1, 1, canvas_menu_str);
}

/* Returns true if a new drawing character was chosen, false otherwise. */
bool show_char_menu(vector<string> char_options, string& drawing_character)
{
	ynot::clear_screen();
	string selectors = "\x1b[4;42m";
	int i = 0;
	for (; i < char_options.size() && i < 9; i++)
		selectors += " " + to_string(i + 1);
	for (; i < char_options.size(); i++)
	{
		selectors += " ";
		selectors.push_back(char('a' + i - 9));
	}
	ynot::print_at(1, 1, selectors + " \x1b[0m\n");
	for (const string& ch : char_options)
		ynot::print(" " + ch);
	while (true)
	{
		string key = ynot::get_key();
		if (key == "escape")
			return false;
		if (key == "")
			continue;
		if (key == "0")
			drawing_character = ynot::get_key();
		else if (key[0] > '0' && key[0] <= '9')
		{
			size_t index = size_t(key[0] - '1');
			if (index >= char_options.size())
				continue;
			drawing_character = char_options[index];
		}
		else
		{
			size_t index = size_t(key[0] - 'a' + 9);
			if (index >= char_options.size())
				continue;
			drawing_character = char_options[index];
		}
		return true;
	}
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
