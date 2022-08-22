#pragma once

#include "canvas.h"
#include <map>

class App
{
public:

	App(std::string version, std::map<std::string, std::vector<std::string>> brush_map);
	~App();
	void run();

private:

	std::string version;
	Coord window_size;
	std::vector<std::vector<Pixel>> canvas;
	Brush brush;
	std::map<std::string, std::vector<std::string>> brush_map;

	bool confirmed_dont_save();

	void show_help();

	/* Returns true if the canvas changed, false otherwise. */
	bool run_canvas();

	/* Returns true if the canvas changed, false otherwise. */
	bool run_canvas_loop(HANDLE handle, DWORD previous_input_mode);

	void on_canvas_mouse_event(MOUSE_EVENT_RECORD mer, bool& canvas_changed);

	/* Returns true if returning to the canvas, false if returning to the main menu. */
	bool on_canvas_key_event(WCHAR key, bool& clear_input_buffer);

	/* Returns true if returning to the canvas, false if returning to the main menu. */
	bool run_brush_menu();

	void print_brush_menu();

	/* Returns true if returning to the canvas, false if returning to the main menu. */
	bool run_brush_menu_loop();

	/* Returns true if a new brush was chosen, false otherwise. */
	bool run_brush_menu(const std::vector<std::string>& brush_options);

	void print_brush_menu(const std::vector<std::string>& brush_options);

	/* Returns true if a new brush was chosen, false otherwise. */
	bool run_brush_menu_loop(const std::vector<std::string>& brush_options);

	void run_color_menu();

	void print_color_options(std::string category);

	bool is_number(std::string str);

	// Returns an empty string if the user pressed escape.
	std::string get_color_number_input();
};
