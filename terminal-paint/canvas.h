#include "../ynot/ynot/ynot.h"
#include <string>
#include <vector>
using ynot::Coord;

std::vector<std::vector<std::string>> create_canvas(Coord window_size);

void clear_canvas(std::vector<std::vector<std::string>>& canvas);

void print_entire_canvas(std::vector<std::vector<std::string>>& canvas, Coord window_size);

void draw(std::string output, COORD cursor_coord, const int radius, std::vector<std::vector<std::string>>& canvas, Coord window_size);

void draw_forward_y_x(const int y_start, std::string output, COORD cursor_coord, const int radius, std::vector<std::vector<std::string>> canvas, Coord window_size);

void draw_reversed_y_x(const int y_start, std::string output, COORD cursor_coord, const int radius, std::vector<std::vector<std::string>> canvas, Coord window_size);

void draw_x(const int y, std::string output, COORD cursor_coord, int radius, std::vector<std::vector<std::string>> canvas, Coord window_size);

void draw_forward_x(const int x_start, const int y, std::string output, COORD cursor_coord, const int radius, std::vector<std::vector<std::string>> canvas, Coord window_size);

void draw_reversed_x(const int x_start, const int y, std::string output, COORD cursor_coord, const int radius, std::vector<std::vector<std::string>> canvas, Coord window_size);

void draw_pixel(const int x, const int y, std::string output, std::vector<std::vector<std::string>>& canvas, Coord window_size);

/* Returns true if successful, false otherwise. */
bool load_canvas(std::vector<std::vector<std::string>>& canvas, Coord window_size);

/* Copies a .tpaint file's content into the canvas.
   Assumes the file content has a valid format. */
void load_canvas(std::string file_content, std::vector<std::vector<std::string>>& canvas, Coord window_size);

/* Enlarges the canvas to match the window's size, filling empty parts with spaces.
   Does nothing if the canvas is the same size or larger.
   Returns true if the canvas and/or any rows changed size, false otherwise. */
bool enlarge_canvas(std::vector<std::vector<std::string>>& canvas, Coord window_size);

/* Returns true if one or more rows changed size, false otherwise. */
bool enlarge_rows(std::vector<std::vector<std::string>>& canvas, size_t target_row_size);

/* Returns true if successful, false otherwise. */
bool save_canvas(std::vector<std::vector<std::string>>& canvas);

/* Returns true if successful, false otherwise. */
bool export_canvas(std::vector<std::vector<std::string>>& canvas);

void error_exit(std::string message, HANDLE handle, DWORD previous_input_mode);

void reset_terminal(HANDLE handle, DWORD previous_input_mode);

void reset_terminal();
