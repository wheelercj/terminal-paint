#include "App.h"
#include <fstream>
using namespace std;
using ynot::CursorStyle;
using ynot::Coord;

App::App(string version, map<string, vector<string>> brush_map)
{
	ynot::reset_on_keyboard_interrupt();
	ynot::set_window_title("terminal paint");
	this->version = version;
	this->brush_map = brush_map;
	this->window_size = ynot::get_window_size();
	this->canvas = create_canvas(this->window_size);
}

App::~App()
{
	reset_terminal();
}

void App::run()
{
	bool saved = true;
	bool havent_shown_help_yet = true;
	ynot::Menu main_menu(
		"terminal paint",
		{ "paint", "load", "save", "export", "clear", "help", "exit" });
	string choice = "";
	while (choice != "exit" || (!saved && !this->confirmed_dont_save()))
	{
		choice = main_menu.run();
		update_window_size();
		if (choice == "paint")
		{
			if (havent_shown_help_yet)
			{
				this->show_help();
				havent_shown_help_yet = false;
			}
			bool canvas_changed = this->run_canvas();
			if (canvas_changed)
				saved = false;
		}
		else if (choice == "load" && (saved || this->confirmed_dont_save()))
		{
			if (!load_canvas(this->canvas, this->window_size))
				continue;
			saved = true;
			bool canvas_changed = this->run_canvas();
			if (canvas_changed)
				saved = false;
			havent_shown_help_yet = false;
		}
		else if (choice == "save")
		{
			if (!save_canvas(this->canvas))
				continue;
			saved = true;
			ynot::notify("Canvas saved.");
		}
		else if (choice == "export")
		{
			if (!export_canvas(this->canvas))
				continue;
			ynot::notify("Canvas exported.");
		}
		else if (choice == "clear" && (saved || this->confirmed_dont_save()))
		{
			clear_canvas(this->canvas);
			saved = true;
			ynot::notify("Canvas cleared.");
		}
		else if (choice == "help")
		{
			this->show_help();
			havent_shown_help_yet = false;
		}
	}
	reset_terminal();
}

bool App::confirmed_dont_save()
{
	ynot::Menu confirmation_menu("Save changes?", { "save", "don't save", "cancel" });
	string choice = confirmation_menu.run();
	if (choice == "save")
	{
		if (!save_canvas(this->canvas))
			return false;
		ynot::notify("Saved.");
		reset_terminal();
		exit(0);
	}
	return choice == "don't save";
}

bool App::update_window_size()
{
	Coord temp_window_size = ynot::get_window_size();
	if (temp_window_size != this->window_size)
	{
		this->window_size = temp_window_size;
		return true;
	}
	return false;
}

void App::show_help()
{
	ynot::notify(
		"\t\tterminal paint\n\t\t" + this->version + R"help_menu(
		
		In the paint canvas:
		• left click to draw and right click to erase
		• use the number keys to control the brush radius
		• press tab to open the brush symbol menu
		• press space to open the color menu
		• press escape to return to the main menu
		
		You can export using the export option in the main menu or
		with shift & drag to select part of the screen and ctrl+c
		to copy. Exporting emoji may have unpredictable results; it
		helps to not have 3 or more emoji on the same line next
		to each other, and redrawing emoji can fix some issues.

		For more help, join the discussions at
		https://github.com/wheelercj/terminal-paint/discussions
		)help_menu");
}

bool App::run_canvas()
{
	ynot::alternate_screen_buffer();
	print_entire_canvas(this->canvas, this->brush, this->window_size);
	HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
	DWORD previous_input_mode{};
	if (handle == INVALID_HANDLE_VALUE)
		error_exit("GetStdHandle", handle, previous_input_mode);

	// Save the current input mode, to be restored on exit.
	if (!GetConsoleMode(handle, &previous_input_mode))
		error_exit("GetConsoleMode", handle, previous_input_mode);

	// Enable the window and mouse input events.
	DWORD new_input_mode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	if (!SetConsoleMode(handle, new_input_mode))
		error_exit("SetConsoleMode", handle, previous_input_mode);

	return this->run_canvas_loop(handle, previous_input_mode);
}

bool App::run_canvas_loop(HANDLE handle, DWORD previous_input_mode)
{
	bool canvas_changed = false;
	bool clear_input_buffer = true;
	const int buffer_size = 128;
	INPUT_RECORD input_record_buffer[buffer_size];
	DWORD read_record_count;
	while (true)
	{
		BOOL ok = ReadConsoleInputW(
			handle,
			input_record_buffer,
			buffer_size,
			&read_record_count);
		if (!ok)
			error_exit("ReadConsoleInput", handle, previous_input_mode);
		if (clear_input_buffer)
		{
			clear_input_buffer = false;
			continue;
		}
		for (DWORD i = 0; i < read_record_count; i++)
		{
			switch (input_record_buffer[i].EventType)
			{
				case MOUSE_EVENT:
				{
					MOUSE_EVENT_RECORD mer = input_record_buffer[i].Event.MouseEvent;
					this->on_canvas_mouse_event(mer, canvas_changed);
					break;
				}
				case KEY_EVENT:
				{
					WCHAR key = input_record_buffer[i].Event.KeyEvent.uChar.UnicodeChar;
					bool resume = this->on_canvas_key_event(key, clear_input_buffer);
					if (!resume)
						return canvas_changed;
					break;
				}
				case WINDOW_BUFFER_SIZE_EVENT:
				{
					COORD new_window_size = input_record_buffer[i].Event.WindowBufferSizeEvent.dwSize;
					this->window_size.x = new_window_size.X;
					this->window_size.y = new_window_size.Y;
					enlarge_canvas(this->canvas, this->window_size);
					print_entire_canvas(this->canvas, this->brush, this->window_size);
				}
			}
		}
	}
}

void App::on_canvas_mouse_event(MOUSE_EVENT_RECORD mer, bool& canvas_changed)
{
	COORD coord = mer.dwMousePosition;
	switch (mer.dwButtonState)
	{
	case 1:  // left click
		draw(this->brush, coord, this->canvas, this->window_size);
		canvas_changed = true;
		break;
	case 2:  // right click
		Brush eraser(" ", "", this->brush.radius);
		draw(eraser, coord, this->canvas, this->window_size);
		canvas_changed = true;
	}
}

bool App::on_canvas_key_event(WCHAR key, bool& clear_input_buffer)
{
	if (key == '\x1b')  // escape
		return false;
	if (key >= '1' && key <= '9')
	{
		this->brush.radius = key - '0';
		ynot::print_at(17, 1, "\x1b[48;5;28m" + to_string(this->brush.radius) + "\x1b[49m");
	}
	else if (key == '\t')
	{
		update_window_size();
		bool resume = this->run_brush_menu();
		ynot::alternate_screen_buffer();
		if (!resume)
			return false;
		update_window_size();
		print_entire_canvas(this->canvas, this->brush, this->window_size);
		clear_input_buffer = true;
	}
	else if (key == ' ')
	{
		update_window_size();
		this->run_color_menu();
		update_window_size();
		print_entire_canvas(this->canvas, this->brush, this->window_size);
		clear_input_buffer = true;
	}
	return true;
}

bool App::run_brush_menu()
{
	this->print_brush_menu();
	return this->run_brush_menu_loop();
}

void App::print_brush_menu()
{
	ynot::clear_screen();
	string canvas_menu_str = "Press tab to return or choose a category to change the brush:\n";
	int i = 0;
	for (const auto& row : this->brush_map)
	{
		if (i >= this->window_size.y - 5)
			break;
		string row_str = "\x1b[42m" + row.first + "│\x1b[0m";
		for (const string& cell : row.second)
			row_str += cell;
		canvas_menu_str += row_str + "\n";
		i++;
	}
	ynot::print_at(1, 1, canvas_menu_str);
}

bool App::run_brush_menu_loop()
{
	while (true)
	{
		string key = ynot::get_key();
		if (key == "escape")
			return false;
		if (key == "tab")
			return true;
		if (this->brush_map.count(key) == 0)
			continue;
		if (key == "0")
		{
			ynot::notify("Press a key to draw with.", false);
			this->brush.symbol = ynot::get_key();
			ynot::alternate_screen_buffer();
			return true;
		}
		if (key == "1")
		{
			this->brush.symbol = "┼";
			return true;
		}
		if (key == "2")
		{
			this->brush.symbol = "╬";
			return true;
		}
		bool brush_character_changed = this->run_brush_menu(this->brush_map[key]);
		if (brush_character_changed)
			return true;
	}
}

bool App::run_brush_menu(const vector<string>& brush_options)
{
	this->print_brush_menu(brush_options);
	return this->run_brush_menu_loop(brush_options);
}

void App::print_brush_menu(const vector<string>& brush_options)
{
	ynot::clear_screen();
	string text = "";
	size_t i = 0;
	for (; i < brush_options.size() && i < 10; i++)
		text += " \x1b[4;42m" + to_string(i) + ":\x1b[0m " + brush_options[i];
	for (; i < brush_options.size(); i++)
	{
		if (i % 10 == 0)
			text += "\n";
		text += " \x1b[4;42m";
		text.push_back(char('a' + i - 10));
		text += ":\x1b[0m " + brush_options[i];
	}
	ynot::print_at(1, 1, text);
}

bool App::run_brush_menu_loop(const vector<string>& brush_options)
{
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
			if (index >= brush_options.size())
				continue;
			this->brush.symbol = brush_options[index];
		}
		else
		{
			size_t index = size_t(key[0] - 'a' + 10);
			if (index >= brush_options.size())
				continue;
			this->brush.symbol = brush_options[index];
		}
		return true;
	}
}

void App::run_color_menu()
{
	ynot::Menu color_menu("choose a color category", { "foreground", "background", "return" });
	string category = color_menu.run();
	ynot::alternate_screen_buffer();
	if (category == "return")
		return;
	this->print_color_options(category);
	ynot::print("Enter a color's number, or enter zero for the default color:\n");
	ynot::set_cursor_style(CursorStyle::not_hidden);
	string color_number_s = this->get_color_number_input();
	ynot::set_cursor_style(CursorStyle::hidden);
	if (color_number_s.empty())
		return;
	if (category == "foreground")
	{
		regex fg_pattern("\x1b\\[38;5;\\d{1,3}m");
		this->brush.color = regex_replace(this->brush.color, fg_pattern, "");
		if (color_number_s == "0")
			return;
		this->brush.color += "\x1b[38;5;" + color_number_s + "m";
		this->brush.fg_color = atoi(color_number_s.c_str());
	}
	else
	{
		regex bg_pattern("\x1b\\[48;5;\\d{1,3}m");
		this->brush.color = regex_replace(this->brush.color, bg_pattern, "");
		if (color_number_s == "0")
			return;
		this->brush.color.append("\x1b[48;5;" + color_number_s + "m");
		this->brush.bg_color = atoi(color_number_s.c_str());
	}
}

string App::get_color_number_input()
{
	string color_number_s = "";
	while (true)
	{
		string key = ynot::get_key();
		if (key == "escape")
			return "";
		if (key == "enter"
			&& color_number_s.size() >= 1
			&& stoi(color_number_s) <= 255
		)
			return color_number_s;
		if (key == "backspace")
		{
			color_number_s.pop_back();
			ynot::backspace_chars(1);
			continue;
		}
		if (!this->is_number(key) || color_number_s.size() == 3)
			continue;
		color_number_s += key;
		ynot::print(key);
	}
}

void App::print_color_options(string category)
{
	ynot::set_cursor_coords(1, 1);
	string options = "";
	for (int i = 0; i <= 255; i++)
	{
		string s = to_string(i);
		if (category == "foreground")
			options += " \x1b[38;5;" + s + "m" + s;
		else
			options += " \x1b[48;5;" + s + "m" + s;
	}
	ynot::print(ynot::wrap(options + "\x1b[0m", this->window_size.x * 3) + "\n\n");
}

bool App::is_number(string str)
{
	return !str.empty() && std::find_if(str.begin(),
		str.end(), [](unsigned char c) { return !std::isdigit(c); }) == str.end();
}
