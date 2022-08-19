#include "draw.h"
using namespace std;

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
