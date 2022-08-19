#include "canvas.h"
#include "files.h"
#include <fstream>
#include <windows.h>
using ynot::CursorStyle;
using namespace std;

vector<vector<string>> create_canvas(Coord window_size)
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

void print_entire_canvas(vector<vector<string>>& canvas, Coord window_size)
{
	for (int y = 0; y <= window_size.y && y < canvas.size(); y++)
	{
		for (int x = 0; x <= window_size.x && x < canvas[y].size(); x++)
			ynot::print_at(x, y, canvas[y][x]);
	}
}

void draw(
	string output,
	COORD cursor_coord,
	const int radius,
	vector<vector<string>>& canvas,
	Coord window_size)
{
	int y_start = cursor_coord.Y - radius + 1;
	if (y_start < 1)
	{
		y_start = cursor_coord.Y + radius;
		draw_reversed_y_x(y_start, output, cursor_coord, radius, canvas, window_size);
	}
	else
		draw_forward_y_x(y_start, output, cursor_coord, radius, canvas, window_size);
	
}

void draw_forward_y_x(
	const int y_start,
	string output,
	COORD cursor_coord,
	const int radius,
	vector<vector<string>> canvas,
	Coord window_size)
{
	for (int y = y_start;
		y <= cursor_coord.Y + radius - 1 && y < canvas.size() - 1;
		y++)
	{
		draw_x(y, output, cursor_coord, radius, canvas, window_size);
	}
}

void draw_reversed_y_x(
	const int y_start,
	string output,
	COORD cursor_coord,
	const int radius,
	vector<vector<string>> canvas,
	Coord window_size)
{
	for (int y = y_start; y > 0; y--)
		draw_x(y, output, cursor_coord, radius, canvas, window_size);
}

void draw_x(
	const int y,
	string output,
	COORD cursor_coord,
	int radius,
	vector<vector<string>> canvas,
	Coord window_size)
{
	int x_start = cursor_coord.X - radius + 1;
	if (x_start < 1)
	{
		x_start = cursor_coord.X + radius;
		draw_reversed_x(x_start, y, output, cursor_coord, radius, canvas, window_size);
	}
	else
		draw_forward_x(x_start, y, output, cursor_coord, radius, canvas, window_size);
}

void draw_forward_x(
	const int x_start,
	const int y,
	string output,
	COORD cursor_coord,
	const int radius,
	vector<vector<string>> canvas,
	Coord window_size)
{
	for (int x = x_start;
		x <= cursor_coord.X + radius - 1 && x < canvas[y].size() - 1;
		x++)
	{
		draw_pixel(x, y, output, canvas, window_size);
	}
}

void draw_reversed_x(
	const int x_start,
	const int y,
	string output,
	COORD cursor_coord,
	const int radius,
	vector<vector<string>> canvas,
	Coord window_size)
{
	for (int x = x_start; x > 0; x--)
		draw_pixel(x, y, output, canvas, window_size);
}

void draw_pixel(
	const int x,
	const int y,
	string output,
	vector<vector<string>>& canvas,
	Coord window_size)
{
	if ((window_size.y > y && window_size.x > x && y >= 0 && x >= 0)
		&& (output.size() == 1 || x < window_size.x - 3))
	{
		canvas[size_t(y) + 1][size_t(x) + 1] = output;
		ynot::print_at(x + 1, y + 1, output);
	}
}

bool load_canvas(vector<vector<string>>& canvas, Coord window_size)
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

void load_canvas(string file_content, vector<vector<string>>& canvas, Coord window_size)
{
	vector<string> lines = ynot::split(file_content, "\n");
	canvas.clear();
	canvas.resize(lines.size());
	for (size_t y = 0; y < canvas.size(); y++)
	{
		vector<string> pixels = ynot::split(lines[y], "␝");
		canvas[y].resize(pixels.size());
		for (size_t x = 0; x < canvas[y].size(); x++)
			canvas[y][x] = pixels[x];
	}
	enlarge_canvas(canvas, window_size);
}

bool enlarge_canvas(vector<vector<string>>& canvas, Coord window_size)
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

bool enlarge_rows(vector<vector<string>>& canvas, size_t target_row_size)
{
	bool size_changed = false;
	for (vector<string>& row : canvas)
	{
		size_t row_size = row.size();
		if (row_size < target_row_size)
		{
			row.resize(target_row_size);
			size_changed = true;
			for (size_t x = row_size; x < row.size(); x++)
				row[x] = " ";
		}
	}
	return size_changed;
}

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

void error_exit(string message, HANDLE handle, DWORD previous_input_mode)
{
	ynot::notify("Error: " + message);
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
