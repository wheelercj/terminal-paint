#include "../ynot/ynot/ynot.h"
#include <string>
#include <vector>
using ynot::Coord;

std::vector<std::vector<std::string>> create_canvas(Coord window_size);

void clear_canvas(std::vector<std::vector<std::string>>& canvas);

void print_entire_canvas(std::vector<std::vector<std::string>>& canvas, Coord window_size);

void draw(std::string output, COORD cursor_coord, int radius, std::vector<std::vector<std::string>>& canvas, Coord window_size);

/* Returns true if successful, false otherwise. */
bool load_canvas(std::vector<std::vector<std::string>>& canvas);

/* Copies a .tpaint file's content into the canvas.
   Assumes the file content has a valid format. */
void load_canvas(std::string file_content, std::vector<std::vector<std::string>>& canvas);

/* Returns true if successful, false otherwise. */
bool save_canvas(std::vector<std::vector<std::string>>& canvas);

/* Returns true if successful, false otherwise. */
bool export_canvas(std::vector<std::vector<std::string>>& canvas);

void error_exit(std::string message, HANDLE handle, DWORD previous_input_mode);

void reset_terminal(HANDLE handle, DWORD previous_input_mode);

void reset_terminal();
