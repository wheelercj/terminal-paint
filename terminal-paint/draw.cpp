#include "draw.h"
using namespace std;

void draw(
	Brush brush,
	COORD cursor_coord,
	vector<vector<Pixel>>& canvas,
	Coord window_size)
{
	int y_start = cursor_coord.Y - brush.radius + 1;
	if (y_start < 1)
	{
		y_start = cursor_coord.Y + brush.radius;
		draw_reversed_y_x(y_start, brush, cursor_coord, canvas, window_size);
	}
	else
		draw_forward_y_x(y_start, brush, cursor_coord, canvas, window_size);

}

void draw_forward_y_x(
	const int y_start,
	Brush brush,
	COORD cursor_coord,
	vector<vector<Pixel>>& canvas,
	Coord window_size)
{
	for (int y = y_start;
		y <= cursor_coord.Y + brush.radius - 1 && y < canvas.size() - 1;
		y++)
	{
		draw_x(y, brush, cursor_coord, canvas, window_size);
	}
}

void draw_reversed_y_x(
	const int y_start,
	Brush brush,
	COORD cursor_coord,
	vector<vector<Pixel>>& canvas,
	Coord window_size)
{
	for (int y = y_start; y > 0; y--)
		draw_x(y, brush, cursor_coord, canvas, window_size);
}

void draw_x(
	int y,
	Brush brush,
	COORD cursor_coord,
	vector<vector<Pixel>>& canvas,
	Coord window_size)
{
	int x_start = cursor_coord.X - brush.radius + 1;
	if (x_start < 1)
	{
		x_start = cursor_coord.X + brush.radius;
		draw_reversed_x(x_start, y, brush, cursor_coord, canvas, window_size);
	}
	else
		draw_forward_x(x_start, y, brush, cursor_coord, canvas, window_size);
}

void draw_forward_x(
	const int x_start,
	int y,
	Brush brush,
	COORD cursor_coord,
	vector<vector<Pixel>>& canvas,
	Coord window_size)
{
	for (int x = x_start;
		x <= cursor_coord.X + brush.radius - 1 && x < canvas[y].size() - 1;
		x++)
	{
		draw_pixel(x, y, brush, canvas, window_size);
	}
}

void draw_reversed_x(
	const int x_start,
	int y,
	Brush brush,
	COORD cursor_coord,
	vector<vector<Pixel>>& canvas,
	Coord window_size)
{
	for (int x = x_start; x > 0; x--)
		draw_pixel(x, y, brush, canvas, window_size);
}

void draw_pixel(
	int x,
	int y,
	Brush brush,
	vector<vector<Pixel>>& canvas,
	Coord window_size)
{
	x++;
	y++;
	if (x > window_size.x - 4)
		x = window_size.x - 4;
	if (y > window_size.y - 1)
		y = window_size.y - 1;
	if (ynot::contains(" ┼╬", brush.symbol))
		draw_smart_line(x, y, brush, canvas);
	else
	{
		Pixel& pixel = canvas[y][x];
		pixel.set(brush.symbol, brush.color);
		ynot::print_at(x, y, brush.get());
	}
}

void draw_smart_line(
	int x,
	int y,
	Brush brush,
	vector<vector<Pixel>>& canvas,
	bool recurse)
{
	const string single_lines = "│─┼┴├┬┤┘└┌┐";
	const string double_lines = "║═╬╩╠╦╣╝╚╔╗";
	bool single_above = false;
	bool single_right = false;
	bool single_below = false;
	bool single_left = false;
	bool double_above = false;
	bool double_right = false;
	bool double_below = false;
	bool double_left = false;
	if (y - 1 >= 0 && ynot::contains(single_lines, canvas[y - 1][x].symbol))
		single_above = true;
	else if (y - 1 >= 0 && ynot::contains(double_lines, canvas[y - 1][x].symbol))
		double_above = true;
	if (size_t(x) + 1 < canvas[y].size() && ynot::contains(single_lines, canvas[y][x + 1].symbol))
		single_right = true;
	else if (size_t(x) + 1 < canvas[y].size() && ynot::contains(double_lines, canvas[y][x + 1].symbol))
		double_right = true;
	if (size_t(y) + 1 < canvas.size() && ynot::contains(single_lines, canvas[y + 1][x].symbol))
		single_below = true;
	else if (size_t(y) + 1 < canvas.size() && ynot::contains(double_lines, canvas[y + 1][x].symbol))
		double_below = true;
	if (x - 1 >= 0 && ynot::contains(single_lines, canvas[y][x - 1].symbol))
		single_left = true;
	else if (x - 1 >= 0 && ynot::contains(double_lines, canvas[y][x - 1].symbol))
		double_left = true;
	if (brush.symbol == " " && recurse)
	{
		Pixel& pixel = canvas[y][x];
		pixel.symbol = " ";
		pixel.color = "";
		ynot::print_at(x, y, " ");
	}
	else
	{
		Pixel& pixel = canvas[y][x];
		if (single_above && single_right && single_below && single_left)
			pixel.symbol = "┼";
		else if (double_above && double_right && double_below && double_left)
			pixel.symbol = "╬";
		else if (single_above && single_below && double_left && double_right)
			pixel.symbol = "╪";
		else if (double_above && double_below && single_left && single_right)
			pixel.symbol = "╫";
		else if (single_above && single_right && single_below)
			pixel.symbol = "├";
		else if (double_above && double_right && double_below)
			pixel.symbol = "╠";
		else if (single_right && single_below && single_left)
			pixel.symbol = "┬";
		else if (double_right && double_below && double_left)
			pixel.symbol = "╦";
		else if (single_above && single_below && single_left)
			pixel.symbol = "┤";
		else if (double_above && double_below && double_left)
			pixel.symbol = "╣";
		else if (single_above && single_right && single_left)
			pixel.symbol = "┴";
		else if (double_above && double_right && double_left)
			pixel.symbol = "╩";
		else if (single_above && single_below && double_left)
			pixel.symbol = "╡";
		else if (single_above && single_below && double_right)
			pixel.symbol = "╞";
		else if (double_above && double_below && single_left)
			pixel.symbol = "╢";
		else if (double_above && double_below && single_right)
			pixel.symbol = "╟";
		else if (single_left && single_right && double_above)
			pixel.symbol = "╨";
		else if (double_left && double_right && single_above)
			pixel.symbol = "╧";
		else if (single_left && single_right && double_below)
			pixel.symbol = "╥";
		else if (double_left && double_right && single_below)
			pixel.symbol = "╤";
		else if (single_above && single_below)
			pixel.symbol = "│";
		else if (double_above && double_below)
			pixel.symbol = "║";
		else if (single_left && single_right)
			pixel.symbol = "─";
		else if (double_left && double_right)
			pixel.symbol = "═";
		else if (single_left && single_above)
			pixel.symbol = "┘";
		else if (double_left && double_above)
			pixel.symbol = "╝";
		else if (single_right && single_below)
			pixel.symbol = "┌";
		else if (double_right && double_below)
			pixel.symbol = "╔";
		else if (single_right && single_above)
			pixel.symbol = "└";
		else if (double_right && double_above)
			pixel.symbol = "╚";
		else if (single_left && single_below)
			pixel.symbol = "┐";
		else if (double_left && double_below)
			pixel.symbol = "╗";
		else if (single_above && double_right)
			pixel.symbol = "╘";
		else if (double_above && single_right)
			pixel.symbol = "╙";
		else if (single_right && double_below)
			pixel.symbol = "╓";
		else if (double_right && single_below)
			pixel.symbol = "╒";
		else if (single_below && double_left)
			pixel.symbol = "╕";
		else if (double_below && single_left)
			pixel.symbol = "╖";
		else if (single_left && double_above)
			pixel.symbol = "╜";
		else if (double_left && single_above)
			pixel.symbol = "╛";
		else if (single_above || single_below)
			pixel.symbol = "│";
		else if (single_left || single_right)
			pixel.symbol = "─";
		else if (double_above || double_below)
			pixel.symbol = "║";
		else if (double_left || double_right)
			pixel.symbol = "═";
		else if (brush.symbol == "┼")
			pixel.symbol = "│";
		else if (brush.symbol == "╬")
			pixel.symbol = "║";
		if (recurse)
			pixel.color = brush.color;
		ynot::print_at(x, y, pixel.get());
	}
	if (recurse)
	{
		if (single_above || double_above)
			draw_smart_line(x, y - 1, brush, canvas, false);
		if (single_right || double_right)
			draw_smart_line(x + 1, y, brush, canvas, false);
		if (single_below || double_below)
			draw_smart_line(x, y + 1, brush, canvas, false);
		if (single_left || double_left)
			draw_smart_line(x - 1, y, brush, canvas, false);
	}
}
