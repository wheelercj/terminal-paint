#pragma once

#include "../ynot/ynot/ynot.h"
#include <string>
#include <vector>
using ynot::Coord;

void draw(std::string output, COORD cursor_coord, const int radius, std::vector<std::vector<std::string>>& canvas, Coord window_size);

void draw_forward_y_x(const int y_start, std::string output, COORD cursor_coord, const int radius, std::vector<std::vector<std::string>> canvas, Coord window_size);

void draw_reversed_y_x(const int y_start, std::string output, COORD cursor_coord, const int radius, std::vector<std::vector<std::string>> canvas, Coord window_size);

void draw_x(const int y, std::string output, COORD cursor_coord, int radius, std::vector<std::vector<std::string>> canvas, Coord window_size);

void draw_forward_x(const int x_start, const int y, std::string output, COORD cursor_coord, const int radius, std::vector<std::vector<std::string>> canvas, Coord window_size);

void draw_reversed_x(const int x_start, const int y, std::string output, COORD cursor_coord, const int radius, std::vector<std::vector<std::string>> canvas, Coord window_size);

void draw_pixel(const int x, const int y, std::string output, std::vector<std::vector<std::string>>& canvas, Coord window_size);
