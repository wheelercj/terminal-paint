#pragma once
#include <string>
#include <vector>

namespace ynot
{

	class Paginator
	{
	public:

		/* Throws invalid_argument if no text is given. */
		Paginator(
			std::string title,
			std::string text,
			std::string line_prefix = "\n",
			std::string line_suffix = "",
			std::string page_prefix = "",
			std::string page_suffix = "",
			int page_line_count = 24,
			int page_width = 70,
			bool show_page_number = true);
		
		/* Throws invalid_argument if no lines are given. */
		Paginator(
			std::string title,
			std::vector<std::string> lines,
			std::string line_prefix = "\n",
			std::string line_suffix = "",
			std::string page_prefix = "",
			std::string page_suffix = "",
			int page_line_count = 24,
			int page_width = 70,
			bool show_page_number = true);
		
		/* Returns the page number on which the paginator was
		   closed. */
		int run(int start_page = 0);

	private:
		std::vector<std::string> pages;
		int page_number = 0;  // the current index of the pages vector
		int page_width;
		bool show_page_numbers = true;
		std::string line_prefix;
		std::string line_suffix;

		void print_page();
		void print_navigation_line();

		/* The navigation functions return true if the page
		   changed, false otherwise. */
		bool navigate(std::string key);
		bool go_to_previous_page();
		bool go_to_next_page();
		bool go_to_first_page();
		bool go_to_last_page();

		/* Changes each isolated newline into a space and
		   each double space at end of a line into a newline. */
		std::string improve_spacing(std::string text);
	};

}
