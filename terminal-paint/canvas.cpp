#include "canvas.h"
#include "files.h"
#include <fstream>
#include <windows.h>
using ynot::CursorStyle;
using namespace std;

vector<vector<Pixel>> create_canvas(Coord window_size)
{
	vector<vector<Pixel>> canvas;
	canvas.resize(size_t(window_size.y + 10));
	for (size_t y = 0; y < canvas.size(); y++)
		canvas[y].resize(size_t(window_size.x + 10));
	return canvas;
}

void clear_canvas(vector<vector<Pixel>>& canvas)
{
	for (size_t y = 0; y < canvas.size(); y++)
	{
		for (size_t x = 0; x < canvas[y].size(); x++)
			canvas[y][x].set(" ", "");
	}
}

void print_entire_canvas(vector<vector<Pixel>>& canvas, Brush brush, Coord window_size)
{
	ynot::clear_screen();
	print_brush_info_line(brush, window_size);
	for (int y = 2; y < window_size.y && y < canvas.size(); y++)
	{
		for (int x = 2; x < window_size.x && x < canvas[y].size(); x++)
			ynot::print_at(x, y, canvas[y][x].get());
	}
}

void print_brush_info_line(Brush brush, Coord window_size)
{
	ynot::set_cursor_coords(1, 1);
	ynot::print(" \x1b[48;5;22m");
	string r = to_string(brush.radius);
	ynot::print(" brush radius:\x1b[48;5;28m " + r + " \x1b[48;5;22m");
	ynot::print(" symbol:\x1b[48;5;28m " + brush.symbol + " \x1b[48;5;22m");
	string fgc = to_string(brush.fg_color);
	ynot::print(" fg color:\x1b[48;5;" + fgc + "m " + fgc + " \x1b[48;5;22m");
	string bgc = to_string(brush.bg_color);
	ynot::print(" bg color:\x1b[48;5;" + bgc + "m " + bgc + " \x1b[48;5;22m");
	Coord c = ynot::get_cursor_coords();
	for (int i = c.x; i < window_size.x - 3; i++)
		ynot::print(" ");
	ynot::print("\x1b[49m");
}

bool load_canvas(vector<vector<Pixel>>& canvas, Coord window_size)
{
	ynot::notify("Choose a .tpaint file to load.", false);
	string file_path = choose_file_to_load();
	ynot::notify("Loading . . .", false);
	if (file_path.empty())
		return false;
	string content = load_file(file_path);
	if (content.empty())
		return false;
	load_canvas(content, canvas, window_size);
	return true;
}

void load_canvas(string file_content, vector<vector<Pixel>>& canvas, Coord window_size)
{
	vector<string> lines;
	if (ynot::contains(file_content, "\r\n"))
		lines = ynot::split(file_content, "\r\n");
	else
		lines = ynot::split(file_content, "\n");
	canvas.clear();
	canvas.resize(lines.size());
	for (size_t y = 0; y < canvas.size(); y++)
	{
		vector<string> pixel_strs = ynot::split(lines[y], "␝");
		canvas[y].resize(pixel_strs.size());
		for (size_t x = 0; x < canvas[y].size(); x++)
		{
			const string& pixel_str = pixel_strs[x];
			Pixel& pixel = canvas[y][x];
			if (ynot::contains(pixel_str, "␞"))
			{
				vector<string> color_and_symbol = ynot::split(pixel_str, "␞");
				pixel.set(color_and_symbol[1], color_and_symbol[0]);
			}
			else
				pixel.set(pixel_str, "");
		}
	}
	enlarge_canvas(canvas, window_size);
}

bool enlarge_canvas(vector<vector<Pixel>>& canvas, Coord window_size)
{
	bool size_changed = false;
	if (canvas.size() < window_size.y)
	{
		canvas.resize(window_size.y);
		size_changed = true;
	}
	if (enlarge_rows(canvas, window_size.x))
		size_changed = true;
	return size_changed;
}

bool enlarge_rows(vector<vector<Pixel>>& canvas, size_t target_row_size)
{
	bool size_changed = false;
	for (vector<Pixel>& row : canvas)
	{
		size_t row_size = row.size();
		if (row_size < target_row_size)
		{
			row.resize(target_row_size);
			size_changed = true;
		}
	}
	return size_changed;
}

bool save_canvas(vector<vector<Pixel>>& canvas)
{
	ynot::notify("Choose a file name.", false);
	string file_path = create_save_file();
	if (file_path.empty())
		return false;
	ynot::notify("Saving . . .", false);
	ofstream file(file_path);
	if (!file.is_open())
	{
		ynot::notify("\aError: failed to open " + file_path);
		return false;
	}
	for (vector<Pixel>& line : canvas)
	{
		for (size_t i = 0; i < line.size(); i++)
		{
			Pixel& pixel = line[i];
			if (pixel.color.size())
				file << pixel.color << "␞";
			file << pixel.symbol;
			if (i < line.size() - 1)
				file << "␝";
		}
		file << "\n";
	}
	file.close();
	return true;
}

bool export_canvas(vector<vector<Pixel>>& canvas)
{
	bool using_colors = detect_color(canvas);
	bool export_colors = false;
	if (using_colors && "yes" == ynot::Menu("Export color codes?", { "yes", "no" }).run())
		export_colors = true;
	ynot::notify("Choose a file name.", false);
	string file_path = create_export_file();
	if (file_path.empty())
		return false;
	ynot::notify("Exporting . . .", false);
	ofstream file(file_path);
	if (!file.is_open())
	{
		ynot::notify("\aError: failed to open " + file_path);
		return false;
	}
	for (vector<Pixel>& line : canvas)
	{
		for (Pixel& pixel : line)
		{
			if (export_colors)
				file << pixel.get();
			else
				file << pixel.symbol;
		}
		file << "\n";
	}
	file.close();
	return true;
}

bool detect_color(vector<vector<Pixel>>& canvas)
{
	for (vector<Pixel>& line : canvas)
	{
		for (Pixel& pixel : line)
		{
			if (pixel.color.size())
				return true;
		}
	}
	return false;
}

void error_exit(string message, HANDLE handle, DWORD previous_input_mode)
{
	ynot::notify("\aError: " + message);
	reset_terminal(handle, previous_input_mode);
	ExitProcess(0);
}

void reset_terminal(HANDLE handle, DWORD previous_input_mode)
{
	SetConsoleMode(handle, previous_input_mode);  // Restore input mode on exit.
	reset_terminal();
}

void reset_terminal()
{
	ynot::set_cursor_style(CursorStyle::not_hidden);
	ynot::restore_screen_buffer();
}
