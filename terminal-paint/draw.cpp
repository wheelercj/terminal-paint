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
	vector<vector<string>>& canvas,
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
	vector<vector<string>>& canvas,
	Coord window_size)
{
	for (int y = y_start; y > 0; y--)
		draw_x(y, output, cursor_coord, radius, canvas, window_size);
}

void draw_x(
	int y,
	string output,
	COORD cursor_coord,
	int radius,
	vector<vector<string>>& canvas,
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
	int y,
	string output,
	COORD cursor_coord,
	const int radius,
	vector<vector<string>>& canvas,
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
	int y,
	string output,
	COORD cursor_coord,
	const int radius,
	vector<vector<string>>& canvas,
	Coord window_size)
{
	for (int x = x_start; x > 0; x--)
		draw_pixel(x, y, output, canvas, window_size);
}

void draw_pixel(
	int x,
	int y,
	string output,
	vector<vector<string>>& canvas,
	Coord window_size)
{
	if ((window_size.y > y && window_size.x > x && y >= 0 && x >= 0)
		&& (output.size() == 1 || x < window_size.x - 3))
	{
		x++;
		y++;
		if (ynot::contains(" ┼╬", output))
			draw_smart_line(x, y, output, canvas);
		else
		{
			canvas[y][x] = output;
			ynot::print_at(x, y, output);
		}
	}
}

void draw_smart_line(
	int x,
	int y,
	string output,
	vector<vector<string>>& canvas,
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
	if (y - 1 >= 0 && ynot::contains(single_lines, canvas[y - 1][x]))
		single_above = true;
	else if (y - 1 >= 0 && ynot::contains(double_lines, canvas[y - 1][x]))
		double_above = true;
	if (size_t(x) + 1 < canvas[y].size() && ynot::contains(single_lines, canvas[y][x + 1]))
		single_right = true;
	else if (size_t(x) + 1 < canvas[y].size() && ynot::contains(double_lines, canvas[y][x + 1]))
		double_right = true;
	if (size_t(y) + 1 < canvas.size() && ynot::contains(single_lines, canvas[y + 1][x]))
		single_below = true;
	else if (size_t(y) + 1 < canvas.size() && ynot::contains(double_lines, canvas[y + 1][x]))
		double_below = true;
	if (x - 1 >= 0 && ynot::contains(single_lines, canvas[y][x - 1]))
		single_left = true;
	else if (x - 1 >= 0 && ynot::contains(double_lines, canvas[y][x - 1]))
		double_left = true;
	if (output == " " && recurse)
	{
		canvas[y][x] = " ";
		ynot::print_at(x, y, " ");
	}
	else
	{
		if (single_above && single_right && single_below && single_left)
		{
			canvas[y][x] = "┼";
			ynot::print_at(x, y, "┼");
		}
		else if (double_above && double_right && double_below && double_left)
		{
			canvas[y][x] = "╬";
			ynot::print_at(x, y, "╬");
		}
		else if (single_above && single_below && double_left && double_right)
		{
			canvas[y][x] = "╪";
			ynot::print_at(x, y, "╪");
		}
		else if (double_above && double_below && single_left && single_right)
		{
			canvas[y][x] = "╫";
			ynot::print_at(x, y, "╫");
		}
		else if (single_above && single_right && single_below)
		{
			canvas[y][x] = "├";
			ynot::print_at(x, y, "├");
		}
		else if (double_above && double_right && double_below)
		{
			canvas[y][x] = "╠";
			ynot::print_at(x, y, "╠");
		}
		else if (single_right && single_below && single_left)
		{
			canvas[y][x] = "┬";
			ynot::print_at(x, y, "┬");
		}
		else if (double_right && double_below && double_left)
		{
			canvas[y][x] = "╦";
			ynot::print_at(x, y, "╦");
		}
		else if (single_above && single_below && single_left)
		{
			canvas[y][x] = "┤";
			ynot::print_at(x, y, "┤");
		}
		else if (double_above && double_below && double_left)
		{
			canvas[y][x] = "╣";
			ynot::print_at(x, y, "╣");
		}
		else if (single_above && single_right && single_left)
		{
			canvas[y][x] = "┴";
			ynot::print_at(x, y, "┴");
		}
		else if (double_above && double_right && double_left)
		{
			canvas[y][x] = "╩";
			ynot::print_at(x, y, "╩");
		}
		else if (single_above && single_below && double_left)
		{
			canvas[y][x] = "╡";
			ynot::print_at(x, y, "╡");
		}
		else if (single_above && single_below && double_right)
		{
			canvas[y][x] = "╞";
			ynot::print_at(x, y, "╞");
		}
		else if (double_above && double_below && single_left)
		{
			canvas[y][x] = "╢";
			ynot::print_at(x, y, "╢");
		}
		else if (double_above && double_below && single_right)
		{
			canvas[y][x] = "╟";
			ynot::print_at(x, y, "╟");
		}
		else if (single_left && single_right && double_above)
		{
			canvas[y][x] = "╨";
			ynot::print_at(x, y, "╨");
		}
		else if (double_left && double_right && single_above)
		{
			canvas[y][x] = "╧";
			ynot::print_at(x, y, "╧");
		}
		else if (single_left && single_right && double_below)
		{
			canvas[y][x] = "╥";
			ynot::print_at(x, y, "╥");
		}
		else if (double_left && double_right && single_below)
		{
			canvas[y][x] = "╤";
			ynot::print_at(x, y, "╤");
		}
		else if (single_above && single_below)
		{
			canvas[y][x] = "│";
			ynot::print_at(x, y, "│");
		}
		else if (double_above && double_below)
		{
			canvas[y][x] = "║";
			ynot::print_at(x, y, "║");
		}
		else if (single_left && single_right)
		{
			canvas[y][x] = "─";
			ynot::print_at(x, y, "─");
		}
		else if (double_left && double_right)
		{
			canvas[y][x] = "═";
			ynot::print_at(x, y, "═");
		}
		else if (single_left && single_above)
		{
			canvas[y][x] = "┘";
			ynot::print_at(x, y, "┘");
		}
		else if (double_left && double_above)
		{
			canvas[y][x] = "╝";
			ynot::print_at(x, y, "╝");
		}
		else if (single_right && single_below)
		{
			canvas[y][x] = "┌";
			ynot::print_at(x, y, "┌");
		}
		else if (double_right && double_below)
		{
			canvas[y][x] = "╔";
			ynot::print_at(x, y, "╔");
		}
		else if (single_right && single_above)
		{
			canvas[y][x] = "└";
			ynot::print_at(x, y, "└");
		}
		else if (double_right && double_above)
		{
			canvas[y][x] = "╚";
			ynot::print_at(x, y, "╚");
		}
		else if (single_left && single_below)
		{
			canvas[y][x] = "┐";
			ynot::print_at(x, y, "┐");
		}
		else if (double_left && double_below)
		{
			canvas[y][x] = "╗";
			ynot::print_at(x, y, "╗");
		}
		else if (single_above && double_right)
		{
			canvas[y][x] = "╘";
			ynot::print_at(x, y, "╘");
		}
		else if (double_above && single_right)
		{
			canvas[y][x] = "╙";
			ynot::print_at(x, y, "╙");
		}
		else if (single_right && double_below)
		{
			canvas[y][x] = "╓";
			ynot::print_at(x, y, "╓");
		}
		else if (double_right && single_below)
		{
			canvas[y][x] = "╒";
			ynot::print_at(x, y, "╒");
		}
		else if (single_below && double_left)
		{
			canvas[y][x] = "╕";
			ynot::print_at(x, y, "╕");
		}
		else if (double_below && single_left)
		{
			canvas[y][x] = "╖";
			ynot::print_at(x, y, "╖");
		}
		else if (single_left && double_above)
		{
			canvas[y][x] = "╜";
			ynot::print_at(x, y, "╜");
		}
		else if (double_left && single_above)
		{
			canvas[y][x] = "╛";
			ynot::print_at(x, y, "╛");
		}
		else if (single_above || single_below)
		{
			canvas[y][x] = "│";
			ynot::print_at(x, y, "│");
		}
		else if (single_left || single_right)
		{
			canvas[y][x] = "─";
			ynot::print_at(x, y, "─");
		}
		else if (double_above || double_below)
		{
			canvas[y][x] = "║";
			ynot::print_at(x, y, "║");
		}
		else if (double_left || double_right)
		{
			canvas[y][x] = "═";
			ynot::print_at(x, y, "═");
		}
		else if (output == "┼")
		{
			canvas[y][x] = "│";
			ynot::print_at(x, y, "│");
		}
		else if (output == "╬")
		{
			canvas[y][x] = "║";
			ynot::print_at(x, y, "║");
		}
	}
	if (recurse)
	{
		if (single_above || double_above)
			draw_smart_line(x, y - 1, output, canvas, false);
		if (single_right || double_right)
			draw_smart_line(x + 1, y, output, canvas, false);
		if (single_below || double_below)
			draw_smart_line(x, y + 1, output, canvas, false);
		if (single_left || double_left)
			draw_smart_line(x - 1, y, output, canvas, false);
	}
}
