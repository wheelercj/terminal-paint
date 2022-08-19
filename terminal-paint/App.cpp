#include "App.h"
#include "draw.h"
#include <fstream>
using namespace std;
using ynot::CursorStyle;
using ynot::Coord;

App::App(map<string, vector<string>> char_map)
{
	ynot::reset_on_keyboard_interrupt();
	ynot::set_window_title("terminal paint");
	this->char_map = char_map;
	this->window_size = ynot::get_window_size();
	this->canvas = create_canvas(this->window_size);
	this->brush_character = "┼";
	this->brush_radius = 1;
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

void App::show_help()
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
}

bool App::run_canvas()
{
	ynot::alternate_screen_buffer();
	print_entire_canvas(this->canvas, this->window_size);
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
					enlarge_canvas(canvas, window_size);
					print_entire_canvas(canvas, window_size);
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
		draw(this->brush_character, coord, this->brush_radius, this->canvas, this->window_size);
		canvas_changed = true;
		break;
	case 2:  // right click
		draw(" ", coord, this->brush_radius, this->canvas, this->window_size);
		canvas_changed = true;
	}
}

bool App::on_canvas_key_event(WCHAR key, bool& clear_input_buffer)
{
	if (key == '\x1b')  // escape
		return false;
	if (key >= '1' && key <= '9')
		this->brush_radius = key - '0';
	else if (key == '\t')
	{
		bool resume = this->run_canvas_menu();
		ynot::alternate_screen_buffer();
		if (!resume)
			return false;
		print_entire_canvas(this->canvas, this->window_size);
		clear_input_buffer = true;
	}
	return true;
}

bool App::run_canvas_menu()
{
	this->print_canvas_menu();
	return this->run_canvas_menu_loop();
}

void App::print_canvas_menu()
{
	ynot::clear_screen();
	string canvas_menu_str = "To change the drawing character, choose a category:\n";
	for (const auto& row : this->char_map)
	{
		string row_str = "\x1b[42m" + row.first + "│\x1b[0m";
		for (const string& cell : row.second)
			row_str += cell;
		canvas_menu_str += row_str + "\n";
	}
	ynot::print_at(1, 1, canvas_menu_str);
}

bool App::run_canvas_menu_loop()
{
	while (true)
	{
		string key = ynot::get_key();
		if (key == "escape")
			return false;
		if (key == "tab")
			return true;
		if (this->char_map.count(key) == 0)
			continue;
		if (key == "0")
		{
			ynot::notify("Press a key to draw with.", false);
			this->brush_character = ynot::get_key();
			ynot::alternate_screen_buffer();
			return true;
		}
		if (key == "1")
		{
			this->brush_character = "┼";
			return true;
		}
		if (key == "2")
		{
			this->brush_character = "╬";
			return true;
		}
		bool brush_character_changed = this->run_char_menu(this->char_map[key]);
		if (brush_character_changed)
			return true;
	}
}

bool App::run_char_menu(const vector<string>& char_options)
{
	this->print_char_menu(char_options);
	return this->run_char_menu_loop(char_options);
}

void App::print_char_menu(const vector<string>& char_options)
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
}

bool App::run_char_menu_loop(const vector<string>& char_options)
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
			if (index >= char_options.size())
				continue;
			this->brush_character = char_options[index];
		}
		else
		{
			size_t index = size_t(key[0] - 'a' + 10);
			if (index >= char_options.size())
				continue;
			this->brush_character = char_options[index];
		}
		return true;
	}
}
