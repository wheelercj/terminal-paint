#pragma once

#include "../ynot/ynot/ynot.h"
#include <regex>
#include <string>
#include <vector>
using ynot::Coord;

struct Brush
{
	std::string symbol;
	int radius;
	std::string color;  // an ANSI escape sequence if there is a color
	int fg_color;  // the color number that will replace # in \x1b[38;5;#m
	int bg_color;  // the color number that will replace # in \x1b[48;5;#m

	Brush(std::string symbol = "┼", std::string color = "", int radius = 1)
	{
		this->symbol = symbol;
		this->color = color;
		this->radius = radius;
	}

	std::string get()
	{
		if (this->color.size())
			return this->color + this->symbol + "\x1b[0m";
		else
			return this->symbol;
	}
};

struct Pixel
{
	std::string symbol;
	std::string color;  // an ANSI escape sequence if there is a color
	
	Pixel(std::string symbol = " ", std::string color = "")
	{
		this->symbol = symbol;
		this->color = color;
	}

	void set(std::string symbol, std::string color)
	{
		this->symbol = symbol;
		this->color = color;
	}
	
	std::string get()
	{
		if (this->color.size())
			return this->color + this->symbol + "\x1b[0m";
		else
			return this->symbol;
	}
};

void draw(Brush brush, COORD cursor_coord, std::vector<std::vector<Pixel>>& canvas, Coord window_size);

void draw_forward_y_x(const int y_start, Brush brush, COORD cursor_coord, std::vector<std::vector<Pixel>>& canvas, Coord window_size);

void draw_reversed_y_x(const int y_start, Brush brush, COORD cursor_coord, std::vector<std::vector<Pixel>>& canvas, Coord window_size);

void draw_x(int y, Brush brush, COORD cursor_coord, std::vector<std::vector<Pixel>>& canvas, Coord window_size);

void draw_forward_x(const int x_start, int y, Brush brush, COORD cursor_coord, std::vector<std::vector<Pixel>>& canvas, Coord window_size);

void draw_reversed_x(const int x_start, int y, Brush brush, COORD cursor_coord, std::vector<std::vector<Pixel>>& canvas, Coord window_size);

void draw_pixel(int x, int y, Brush brush, std::vector<std::vector<Pixel>>& canvas, Coord window_size);

void draw_smart_line(int x, int y, Brush brush, std::vector<std::vector<Pixel>>& canvas, bool recurse = true);
