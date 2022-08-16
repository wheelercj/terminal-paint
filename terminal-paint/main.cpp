#include "../ynot/ynot/ynot.h"
#include <fstream>
#include <map>
#include <string>
#include <vector>
using namespace std;
using ynot::CursorStyle;
using ynot::Coord;

HANDLE hStdin;
DWORD fdwSaveOldMode;

void validate_char_map();
map<string, vector<string>> get_char_map();
bool confirmed_dont_save(vector<vector<string>>& canvas);
void show_help();
bool open_canvas(vector<vector<string>>& canvas, Coord window_size, string& brush_character, int& brush_radius);
vector<vector<string>> init_canvas(Coord window_size);
void clear_canvas(vector<vector<string>>& canvas);
string choose_file_to_load();
string create_save_file();
string create_export_file();
bool load_canvas(vector<vector<string>>& canvas);
bool save_canvas(vector<vector<string>>& canvas);
bool export_canvas(vector<vector<string>>& canvas);
void print_entire_canvas(vector<vector<string>>& canvas, Coord window_size);
bool show_canvas_menu(string& brush_character);
void print_canvas_menu(const map<string, vector<string>>& char_map);
bool show_char_menu(vector<string> char_options, string& brush_character);
void draw(string output, COORD cursor_coord, int radius, vector<vector<string>>& canvas, Coord window_size);
void error_exit(string message);
void reset_terminal();

int main()
{
	validate_char_map();
	ynot::alternate_screen_buffer();
	ynot::reset_on_keyboard_interrupt();
	ynot::Menu main_menu("terminal paint", { "paint", "load", "save", "export", "clear", "help", "exit" });
	Coord window_size = ynot::get_window_size();
	vector<vector<string>> canvas = init_canvas(window_size);
	int brush_radius = 1;
	string brush_character = "█";

	string choice = "";
	bool saved = true;
	bool havent_shown_help_yet = true;
	while (choice != "exit" || (!saved && !confirmed_dont_save(canvas)))
	{
		choice = main_menu.run();
		ynot::alternate_screen_buffer();
		if (choice == "paint")
		{
			if (havent_shown_help_yet)
			{
				show_help();
				havent_shown_help_yet = false;
			}
			if (open_canvas(canvas, window_size, brush_character, brush_radius))
				saved = false;
		}
		else if (choice == "load" && (saved || confirmed_dont_save(canvas)))
		{
			if (!load_canvas(canvas))
				continue;
			saved = true;
			if (open_canvas(canvas, window_size, brush_character, brush_radius))
				saved = false;
			havent_shown_help_yet = false;
		}
		else if (choice == "save")
		{
			if (!save_canvas(canvas))
				continue;
			saved = true;
			ynot::notify("Canvas saved.");
		}
		else if (choice == "export")
		{
			if (!export_canvas(canvas))
				continue;
			ynot::notify("Canvas exported.");
		}
		else if (choice == "clear" && (saved || confirmed_dont_save(canvas)))
		{
			clear_canvas(canvas);
			saved = true;
			ynot::notify("Canvas cleared.");
		}
		else if (choice == "help")
		{
			show_help();
			havent_shown_help_yet = false;
		}
	}
	reset_terminal();
	return 0;
}

void validate_char_map()
{
	map<string, vector<string>> char_map = get_char_map();
	if (char_map.size() > 36)
		throw std::out_of_range("The char map must have <= 36 items.");
	for (const auto& row : char_map)
	{
		if (row.second.size() > 36)
			throw std::out_of_range("Each vector in the char map must have <= 36 elements.");
	}
}

map<string, vector<string>> get_char_map()
{
	// Some symbols might not render correctly here but still can in terminals.
	return {
		{ "0", { "choose with your keyboard" }},
		{ "1", { "█", "▓", "▒", "▌", "▐", "▄", "▀", "⬛", "■", "◾", "▬", "🔳", "🔲", "⬜", "🟥", "🟧", "🟨", "🟩", "🟦", "🟪", "🟫", "✅", "☑", "❌", "✖", "🛑", "☢", "☣", "🏴", "🏁", "🏳", "🚩" }},
		{ "2", { "…", "·", "•", "◘", "○", "◙", "°", "º", "☼", "ø" }},
		{ "3", { "│", "─", "┼", "┴", "├", "┬", "┤", "┘", "└", "┌", "┐", "║", "═", "╬", "╩", "╠", "╦", "╣", "╝", "╚", "╔", "╗" }},
		{ "4", { "╡", "╞", "╢", "╟", "╪", "╫", "╧", "╨", "╤", "╥", "╘", "╙", "╒", "╓", "╕", "╖", "╛", "╜" }},
		{ "5", { "↑", "↓", "↕", "↨", "←", "→", "↔", "◄", "►", "«", "»", "‹", "›", "⏮", "⏪", "◀", "⏹", "⏸", "⏯", "▶", "⏩", "⏭", "🔁", "🔼", "🔽" }},
		{ "6", { "¤", "$", "¢", "€", "£", "¥", "₧", "©", "®", "™" }},
		{ "7", { "±", "×", "≤", "≥", "≈", "≡", "√", "⌠", "⌡", "∟", "⌐", "¬" }},
		{ "8", { "☺", "☻", "♥", "♦", "♣", "♠", "♪", "♫", "⌂", "∩", "¦", "†", "‡" }},
		{ "9", { "α", "ß", "Γ", "π", "Σ", "σ", "µ", "τ", "Φ", "Θ", "Ω", "δ", "∞", "φ", "ε" }},
		{ "a", { "🔒", "🔑", "🗝", "🔧", "⛏", "🔨", "🪓", "🔪", "🗡", "✂", "🏹", "🔫", "🧨", "💣", "🛢", "💉", "🔮", "🧭", "💊", "🩹", "⛑", "🦺", "🧤", "🥾", "🛡", "🤿", "🥊", "🧲", "🔍", "💡", "🔦", "🕯", "🧰", "📱", "📞", "☎" }},
		{ "b", { "💾", "💿", "📀", "💽", "⚙", "🔍", "🔒", "🔑", "🔗", "🗑", "🔆", "⏰", "🔈", "🔉", "🔊", "🎧", "🔔", "🛒", "💻", "🧪", "⚗", "🔭", "⚖", "📿", "🧿", "📣", "🥁", "🎺", "🎸", "📚", "📓", "📕", "📗", "📘", "📙" }},
		{ "c", { "🎒", "🛄", "🛅", "💼", "👜", "👛", "👝", "💰", "🏧", "💲", "💸", "💵", "💶", "💴", "💷", "💳", "🧧", "🛍", "🎁", "📦", "🥇", "🥈", "🥉", "🏅", "🎖", "🏆", "💎", "💍", "♦", "🎰", "🎲" }},
		{ "d", { "♥", "💘", "💖", "💗", "💓", "💙", "💚", "💛", "💜", "🧡", "💝", "💞", "💟", "🖤", "❤", "💌", "❣", "🤎", "🤍", "💔", "💕" }},
		{ "e", { "💀", "☠", "🦴", "👀", "👁", "👣", "🐾", "옷", "🤺", "⛄", "☃", "🤖", "🗿", "👤", "👹", "👿", "👻", "🐜", "⚰", "⚱", "🏺", "⛓", "🕸", "⚔", "⚓", "🧩", "✉", "📦", "📪", "📮", "🪑" }},
		{ "f", { "🎉", "🎊", "🎆", "🎇", "💥", "🔥", "🩸", "💧", "💦", "🌊", "🧊", "❄", "⚡", "🌩", "🌪", "💨" }},
		{ "g", { "💐", "🌸", "🏵", "🌹", "🌺", "🌻", "🌼", "🌷", "🥀", "☘", "🌱", "🌲", "🌳", "🌴", "🌵", "🌾", "🌿", "🍀", "🍁", "🍂", "🍃" }},
		{ "h", { "🌡", "⌛", "⏳", "🕐", "🕑", "🕒", "🕓", "🕔", "🕕", "🕖", "🕗", "🕘", "🕙", "🕚", "🕛", "🕜", "🕝", "🕞", "🕟", "🕠", "🕡", "🕢", "🕣", "🕤", "🕥", "🕦", "🕧" }},
		{ "i", { "🌐", "🌍", "🌎", "🌏", "🪐", "☀", "🌞", "🌙", "🌑", "🌒", "🌓", "🌔", "🌕", "🌖", "🌗", "🌘", "🌚", "🌝", "🌛", "🌜", "🌠", "☄", "⭐", "🌟", "✨", "💫" }},
		{ "j", { "🐒", "🦍", "🦧", "🐕", "🐈", "🐅", "🐆", "🐎", "🦌", "🦏", "🐃", "🐄", "🐖", "🐏", "🐐", "🐪", "🦘", "🦨", "🐁", "🦔", "🐇", "🦆", "🐓", "🦎", "🐊", "🐢", "🐍", "🐉", "🦖", "🦀", "🐝", "🦂", "🕷", "🦇", "🦠" }},
		{ "k", { "🍽", "🍴", "🥄", "🥢", "🥝", "🥥", "🍇", "🍈", "🍉", "🍊", "🍋", "🍌", "🍍", "🥭", "🍎", "🍏", "🍐", "🍑", "🍒", "🍓", "🍅", "🍆", "🌽", "🌶", "🍄", "🥑", "🥒", "🥬", "🥦", "🥔", "🍠", "🧄", "🧅", "🥕", "🌰", "🥜" }},
		{ "l", { "🍞", "🥯", "🥨", "🥐", "🧇", "🥞", "🥗", "🥪", "🍜", "🥡", "🍟", "🥘", "🥣", "🥟", "🍚", "🥫" }},
		{ "m", { "🥧", "🍦", "🍧", "🍨", "🥮", "🥠", "🍩", "🍪", "🎂", "🍰", "🧁", "🍫", "🍬", "🍭", "🍡", "🍮", "🍯", "🧃", "☕", "🍵", "🧉", "🍶", "🍾", "🍷", "🍸", "🍹", "🍺", "🍻", "🥂", "🥃", "🥤" }},
	};
}

bool confirmed_dont_save(vector<vector<string>>& canvas)
{
	ynot::Menu confirmation_menu("Save changes?", { "save", "don't save", "cancel" });
	string choice = confirmation_menu.run();
	if (choice == "save")
	{
		if (!save_canvas(canvas))
			return false;
		ynot::notify("Saved.");
		reset_terminal();
		exit(0);
	}
	if (choice == "don't save")
		return true;
	return false;
}

void show_help()
{
	ynot::notify(R"(
		terminal paint
		
		In the paint canvas:
		• left click to draw and right click to erase
		• use the number keys to control the brush radius
		• press tab to open or close the brush selection menu
		• press escape to return to the main menu
		
		You can export using the export option in the main menu or
		with shift & drag to select part of the screen and ctrl+c
		to copy. Exporting emoji may have unpredictable results; it
		helps to not have 3 or more emoji on the same line next
		to each other, and redrawing emoji can fix some issues.

		For more help, join the discussions at
		https://github.com/wheelercj/terminal-paint/discussions
		)");
	ynot::alternate_screen_buffer();
}

/* Returns true if the canvas changed, false otherwise. */
bool open_canvas(vector<vector<string>>& canvas, Coord window_size, string& brush_character, int& brush_radius)
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
					draw(brush_character, coord, brush_radius, canvas, window_size);
					canvas_changed = true;
					break;
				case 2:
					draw(" ", coord, brush_radius, canvas, window_size);
					canvas_changed = true;
					break;
				}
				break;
			case KEY_EVENT:
				WCHAR key = irInBuf[i].Event.KeyEvent.uChar.UnicodeChar;
				if (key == '\x1b')  // escape
					return canvas_changed;
				if (key >= '1' and key <= '9')
					brush_radius = key - '0';
				else if (key == '\t')
				{
					if (!show_canvas_menu(brush_character))
						return canvas_changed;
					ynot::clear_screen();
					print_entire_canvas(canvas, window_size);
					just_started = true;
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

/* Returns the file path, or an empty string if canceled. */
string choose_file_to_load()
{
	while (true)
	{
		OPENFILENAMEA ofna;
		char sz_file[100]{};
		ZeroMemory(&ofna, sizeof(ofna));
		ofna.lStructSize = sizeof(ofna);
		ofna.hwndOwner = NULL;
		ofna.lpstrFile = sz_file;
		ofna.lpstrFile[0] = '\0';
		ofna.nMaxFile = sizeof(sz_file);
		ofna.lpstrFilter = ".tpaint\0\0";
		ofna.lpstrDefExt = "tpaint\0";
		ofna.nFilterIndex = 1;
		ofna.lpstrFileTitle = NULL;
		ofna.nMaxFileTitle = 0;
		ofna.lpstrInitialDir = NULL;
		ofna.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
		BOOL ok = GetOpenFileNameA(&ofna);
		string file_path = ofna.lpstrFile;
		if (!ok)
			return "";
		else if (!ynot::endswith(file_path, ".tpaint"))
		{
			ynot::notify("Error: the file extension must be .tpaint");
			ynot::notify("Choose a .tpaint file to load.", false);
		}
		else
			return file_path;
	}
}

/* Returns the file path, or an empty string if canceled. */
string create_save_file()
{
	while (true)
	{
		OPENFILENAMEA ofna;
		char sz_file[100]{};
		ZeroMemory(&ofna, sizeof(ofna));
		ofna.lStructSize = sizeof(ofna);
		ofna.hwndOwner = NULL;
		ofna.lpstrFile = sz_file;
		ofna.lpstrFile[0] = '\0';
		ofna.nMaxFile = sizeof(sz_file);
		ofna.lpstrFilter = ".tpaint\0\0";
		ofna.lpstrDefExt = "tpaint\0";
		ofna.nFilterIndex = 1;
		ofna.lpstrFileTitle = NULL;
		ofna.nMaxFileTitle = 0;
		ofna.lpstrInitialDir = NULL;
		BOOL ok = GetSaveFileNameA(&ofna);
		string file_path = ofna.lpstrFile;
		if (!ok)
		{
			ynot::notify("Canceled saving.");
			return "";
		}
		else if (!ynot::endswith(file_path, ".tpaint"))
		{
			ynot::notify("Error: the file extension must be .tpaint");
			ynot::notify("Choose a file name.", false);
		}
		else
			return file_path;
	}
}

/* Returns the file path, or an empty string if canceled. */
string create_export_file()
{
	OPENFILENAMEA ofna;
	char sz_file[100]{};
	ZeroMemory(&ofna, sizeof(ofna));
	ofna.lStructSize = sizeof(ofna);
	ofna.hwndOwner = NULL;
	ofna.lpstrFile = sz_file;
	ofna.lpstrFile[0] = '\0';
	ofna.nMaxFile = sizeof(sz_file);
	ofna.lpstrFilter = "*.txt\0\0";
	ofna.lpstrDefExt = "txt\0";
	ofna.nFilterIndex = 1;
	ofna.lpstrFileTitle = NULL;
	ofna.nMaxFileTitle = 0;
	ofna.lpstrInitialDir = NULL;
	BOOL ok = GetSaveFileNameA(&ofna);
	if (ok)
		return ofna.lpstrFile;
	ynot::notify("Canceled exporting.");
	return "";
}

/* Returns true if successful, false otherwise. */
bool load_canvas(vector<vector<string>>& canvas)
{
	ynot::notify("Choose a .tpaint file to load.", false);
	string file_path = choose_file_to_load();
	ynot::notify("Loading . . .", false);
	if (file_path.empty())
		return false;
	HANDLE file_handle = CreateFileA(  // opens a file, doesn't create one
		file_path.c_str(),
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
	);
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		ynot::notify("Error: failed to open the file.");
		return false;
	}
	DWORD file_size = GetFileSize(file_handle, 0);
	DWORD bytes_read = 0;
	string content;
	content.resize(size_t(file_size + 1));
	BOOL ok = ReadFile(file_handle, content.data(), file_size, &bytes_read, 0);
	CloseHandle(file_handle);
	if (!ok || bytes_read == 0 || !ynot::contains(content, "␝"))
	{
		ynot::notify("Error: opened but failed to read the file.");
		return false;
	}
	vector<string> lines = ynot::split(content, "\n");
	canvas.clear();
	canvas.resize(lines.size());
	for (size_t y = 0; y < canvas.size(); y++)
	{
		vector<string> pixels = ynot::split(lines[y], "␝");
		canvas[y].resize(pixels.size());
		for (size_t x = 0; x < canvas[y].size(); x++)
			canvas[y][x] = pixels[x];
	}
	return true;
}

/* Returns true if successful, false otherwise. */
bool save_canvas(vector<vector<string>>& canvas)
{
	ynot::notify("Choose a file name.", false);
	string file_path = create_save_file();
	if (file_path.empty())
		return false;
	ynot::notify("Saving . . .", false);
	ofstream file(file_path);
	if (!file.is_open())
	{
		ynot::notify("Error: failed to open " + file_path);
		return false;
	}
	for (const vector<string>& line : canvas)
	{
		for (size_t i = 0; i < line.size(); i++)
		{
			file << line[i];
			if (i < line.size() - 1)
				file << "␝";
		}
		file << "\n";
	}
	file.close();
	return true;
}

/* Returns true if successful, false otherwise. */
bool export_canvas(vector<vector<string>>& canvas)
{
	ynot::notify("Choose a file name.", false);
	string file_path = create_export_file();
	if (file_path.empty())
		return false;
	ynot::notify("Exporting . . .", false);
	ofstream file(file_path);
	if (!file.is_open())
	{
		ynot::notify("Error: failed to open " + file_path);
		return false;
	}
	for (const vector<string>& line : canvas)
	{
		for (const string& ch_str : line)
			file << ch_str;
		file << "\n";
	}
	file.close();
	return true;
}

void print_entire_canvas(vector<vector<string>>& canvas, Coord window_size)
{
	for (int y = 0; y < window_size.y && y < canvas.size(); y++)
	{
		for (int x = 0; x < window_size.x && x < canvas[y].size(); x++)
			ynot::print_at(x, y, canvas[y][x]);
	}
}

/* Returns true if returning to the canvas, false if returning to the main menu. */
bool show_canvas_menu(string& brush_character)
{
	map<string, vector<string>> char_map = get_char_map();
	print_canvas_menu(char_map);
	while (true)
	{
		string key = ynot::get_key();
		if (key == "escape")
			return false;
		if (key == "tab")
			return true;
		if (char_map.count(key) == 0)
			continue;
		else
		{
			if (key == "0")
			{
				ynot::clear_screen();
				ynot::notify("Press a key to draw with.", false);
				brush_character = ynot::get_key();
				return true;
			}
			else if (show_char_menu(char_map[key], brush_character))
				return true;
			else
				print_canvas_menu(char_map);
		}
	}
}

void print_canvas_menu(const map<string, vector<string>>& char_map)
{
	ynot::clear_screen();
	string canvas_menu_str = "To change the drawing character, choose a category:\n";
	for (const auto& row : char_map)
	{
		string row_str = "\x1b[42m" + row.first + "│\x1b[0m";
		for (const string& cell : row.second)
			row_str += cell;
		canvas_menu_str += row_str + "\n";
	}
	ynot::print_at(1, 1, canvas_menu_str);
}

/* Returns true if a new drawing character was chosen, false otherwise. */
bool show_char_menu(vector<string> char_options, string& brush_character)
{
	ynot::clear_screen();
	string text = "";
	size_t i = 0;
	for (; i < char_options.size() && i < 10; i++)
		text += " \x1b[4;42m" + to_string(i) + ":\x1b[0m " + char_options[i];
	for (; i < char_options.size(); i++)
	{
		if (i % 10 == 0)
			text += "\n";
		text += " \x1b[4;42m";
		text.push_back(char('a' + i - 10));
		text += ":\x1b[0m " + char_options[i];
	}
	ynot::print_at(1, 1, text);
	while (true)
	{
		string key = ynot::get_key();
		if (key == "escape")
			return false;
		if (key == "")
			continue;
		else if (key[0] >= '0' && key[0] <= '9')
		{
			size_t index = size_t(key[0] - '0');
			if (index >= char_options.size())
				continue;
			brush_character = char_options[index];
		}
		else
		{
			size_t index = size_t(key[0] - 'a' + 10);
			if (index >= char_options.size())
				continue;
			brush_character = char_options[index];
		}
		return true;
	}
}

void draw(string output, COORD cursor_coord, int radius, vector<vector<string>>& canvas, Coord window_size)
{
	for (int y = cursor_coord.Y - radius + 1; y <= cursor_coord.Y + radius - 1 && y < canvas.size(); y++)
	{
		for (int x = cursor_coord.X - radius + 1; x <= cursor_coord.X + radius - 1 && x < canvas[y].size(); x++)
		{
			if (window_size.y > y && window_size.x > x && y >= 0 && x >= 0)
			{
				if (output.size() <= 3 || x < window_size.x - 3)
				{
					canvas[size_t(y) + 1][size_t(x) + 1] = output;
					ynot::print_at(x + 1, y + 1, output);
				}
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
