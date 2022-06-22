#include "Paginator.h"
#include <regex>
#include "str.h"
#include "terminal.h"

namespace ynot
{
	Paginator::Paginator(
		std::string title,
		std::string text,
		std::string line_prefix,
		std::string line_suffix,
		std::string page_prefix,
		std::string page_suffix,
		int page_line_count,
		int page_width,
		bool show_page_numbers)
	{
		if (text.empty())
			throw std::invalid_argument("Text must be given.");
		this->page_width = page_width;
		this->show_page_numbers = show_page_numbers;
		this->line_prefix = line_prefix;
		this->line_suffix = line_suffix;
		text = improve_spacing(text);
		text = wrap(text, page_width, line_prefix, line_suffix);
		if (startswith(text, "\n"))
			text = slice(text, 1);
		std::vector<std::string> lines = split(text, "\n");
		std::string page_title = line_prefix + "\x1b[4m" + title + "\x1b[24m" + "\n";
		std::string page;
		while (lines.size())
		{
			page = page_title;
			page += page_prefix;
			for (int i = 0; i < page_line_count && lines.size(); i++)
			{
				page += "\n" + lines[0];
				lines.erase(lines.begin(), lines.begin() + 1);
			}
			page += page_suffix;
			this->pages.push_back(page);
		}
	}

	Paginator::Paginator(std::string title,
		std::vector<std::string> lines,
		std::string line_prefix,
		std::string line_suffix,
		std::string page_prefix,
		std::string page_suffix,
		int page_line_count,
		int page_width,
		bool show_page_number)
	{
		if (lines.empty())
			throw std::invalid_argument("Lines must be given.");
		this->page_width = page_width;
		this->show_page_numbers = show_page_numbers;
		this->line_prefix = line_prefix;
		this->line_suffix = line_suffix;
		for (std::string& line : lines)
			line = wrap(line, page_width, line_prefix, line_suffix);
		std::string page_title = title + "\n";
		std::string page;
		while (lines.size())
		{
			page = page_title;
			page += page_prefix;
			for (int i = 0; i < page_line_count && lines.size(); i++)
			{
				page += lines[0];
				lines.erase(lines.begin(), lines.begin() + 1);
			}
			page += page_suffix;
			this->pages.push_back(page);
		}
	}

	int Paginator::run(int start_page)
	{
		save_cursor_location();
		set_cursor_style(CursorStyle::hidden);
		this->page_number = start_page;
		alternate_screen_buffer();
		bool page_changed = true;
		std::string key = "";
		while (key != "escape")
		{
			if (page_changed)
			{
				clear_screen();
				set_cursor_coords(1, 1);
				print_page();
			}
			key = get_key();
			page_changed = navigate(key);
		}
		restore_screen_buffer();
		restore_cursor_location();
		set_cursor_style(CursorStyle::not_hidden);
		return this->page_number;
	}

	void Paginator::print_page()
	{
		std::cout << this->pages[this->page_number];
		print_navigation_line();
	}

	void Paginator::print_navigation_line()
	{
		int prefix_and_suffix_width = int(this->line_prefix.size() + this->line_suffix.size());
		std::string page_number_str;
		if (!this->show_page_numbers)
			page_number_str = center("", this->page_width - 4 - prefix_and_suffix_width);
		else
		{
			page_number_str =
				std::to_string(this->page_number + 1)
				+ "/" + std::to_string(pages.size());
			page_number_str = center(page_number_str, this->page_width - 4 - prefix_and_suffix_width);
		}
		std::cout << "\n" << this->line_prefix;
		if (this->page_number > 0)
			std::cout << "<-";
		else
			std::cout << "  ";
		std::cout << page_number_str;
		if (this->page_number < pages.size() - 1)
			std::cout << "->";
		else
			std::cout << "  ";
		std::cout << this->line_suffix;
	}

	bool Paginator::navigate(std::string key)
	{
		if (key == "left arrow" || key == "up arrow" || key == "page up")
		{
			if (go_to_previous_page())
				return true;
		}
		else if (key == "right arrow" || key == "down arrow" || key == "page down")
		{
			if (go_to_next_page())
				return true;
		}
		else if (key == "home")
		{
			if (go_to_first_page())
				return true;
		}
		else if (key == "end")
		{
			if (go_to_last_page())
				return true;
		}
		return false;
	}

	bool Paginator::go_to_previous_page()
	{
		if (this->page_number > 0)
		{
			this->page_number -= 1;
			return true;
		}
		return false;
	}

	bool Paginator::go_to_next_page()
	{
		if (this->page_number < pages.size() - 1)
		{
			this->page_number += 1;
			return true;
		}
		return false;
	}

	bool Paginator::go_to_first_page()
	{
		if (this->page_number > 0)
		{
			this->page_number = 0;
			return true;
		}
		return false;
	}

	bool Paginator::go_to_last_page()
	{
		if (this->page_number < pages.size() - 1)
		{
			this->page_number = int(pages.size()) - 1;
			return true;
		}
		return false;
	}
	
	std::string Paginator::improve_spacing(std::string text)
	{
		return std::regex_replace(text, std::regex(R"((?!\n|  )\n(?!\n))"), " ");
	}
}
