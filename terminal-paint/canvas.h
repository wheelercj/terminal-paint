#include "../ynot/ynot/ynot.h"
#include "draw.h"
#include <string>
#include <vector>
using ynot::Coord;

std::vector<std::vector<Pixel>> create_canvas(Coord window_size);

void clear_canvas(std::vector<std::vector<Pixel>>& canvas);

void print_entire_canvas(std::vector<std::vector<Pixel>>& canvas, Brush brush, Coord window_size);

void print_brush_info_line(Brush brush, Coord window_size);

/* Returns true if successful, false otherwise. */
bool load_canvas(std::vector<std::vector<Pixel>>& canvas, Coord window_size);

/* Copies a .tpaint file's content into the canvas.
   Assumes the file content has a valid format. */
void load_canvas(std::string file_content, std::vector<std::vector<Pixel>>& canvas, Coord window_size);

/* Enlarges the canvas to match the window's size, filling empty parts with spaces.
   Does nothing if the canvas is the same size or larger.
   Returns true if the canvas and/or any rows changed size, false otherwise. */
bool enlarge_canvas(std::vector<std::vector<Pixel>>& canvas, Coord window_size);

/* Returns true if one or more rows changed size, false otherwise. */
bool enlarge_rows(std::vector<std::vector<Pixel>>& canvas, size_t target_row_size);

/* Returns true if successful, false otherwise. */
bool save_canvas(std::vector<std::vector<Pixel>>& canvas);

/* Returns true if successful, false otherwise. */
bool export_canvas(std::vector<std::vector<Pixel>>& canvas);

bool detect_color(std::vector<std::vector<Pixel>>& canvas);

void error_exit(std::string message, HANDLE handle, DWORD previous_input_mode);

void reset_terminal(HANDLE handle, DWORD previous_input_mode);

void reset_terminal();
