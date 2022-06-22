#include "common.h"
#include <fstream>
#include "Paginator.h"
#include <signal.h>
#include "str.h"
#include "tui_tests.h"
#include "ynot.h"
using namespace std;
using ynot::Style;
using ynot::CursorStyle;
using ynot::Coord;

void run_tui_tests()
{
	test_get_key();
	test_get_key_without_waiting();
	test_getline_ac();
	test_getline_ac_menu();
	test_getline_ac_numbered_menu();
	test_getline_ac_with_empty_suggestion();
	test_getline_ac_without_input_validation();
	test_getline_ac_without_showing_suggestions();
	test_wrap();
	test_paginator();
	test_set_cursor_style();
	test_set_window_title();
	test_wide_set_window_title();
	test_wide_print();
	test_print_styled();
	test_wide_print_styled();
	test_print_multi_styled();
	test_wide_print_multi_styled();
	test_set_style();
	test_set_multi_style();
	test_reset_style();
	test_print_rgb();
	test_print_invalid_rgb();
	test_wide_print_rgb();
	test_print_background_rgb();
	test_wide_print_background_rgb();
	test_save_cursor_coords();
	test_print_at();
	test_wide_print_at();
	test_set_cursor_coordinates();
	test_get_cursor_location();
	test_restore_cursor_location();
	test_move_cursor_up();
	test_move_cursor_down();
	test_move_cursor_right();
	test_move_cursor_left();
	test_get_window_size();
	test_getch_();
	test_kbhit__();
	test_getch_if_kbhit();
	test_insert();
	test_wide_insert();
	test_delete_chars();
	test_insert_lines();
	test_delete_lines();
	test_alternate_screen_buffer();
	test_sleep_();
	test_get_key_with_cursor_movements();
}

void signal_callback_handler(int signal_number)
{
	cout << "\r                                                                   \r";
	cout << "You pressed Ctrl + c or Ctrl + 2 which ends the program early.";
	exit(signal_number);
}

void test_get_key()
{
	signal(SIGINT, signal_callback_handler);
	cout << "Press any key and its name will be shown on the next line. Press escape to stop.\n";
	string input = "";
	while (input != "escape")
	{
		input = ynot::get_key();
		cout << "\r                           \r" << input;
	}
	cout << endl;
}

void test_get_key_without_waiting()
{
	ynot::set_cursor_style(CursorStyle::hidden);
	string input;
	for (int i = 0; "yes"; i++)
	{
		cout << "\r(" << i << ") Waiting for you to press a key without completely blocking.";
		input = ynot::get_key(false);
		if (input.size())
			break;
		ynot::sleep_(1000);
	}
	ynot::set_cursor_style(CursorStyle::not_hidden);
	cout << "\nYou pressed " << input << endl;
}

void test_getline_ac()
{
	cout << "Autocomplete suggestions example:\n";
	vector<string> large_city_names = { "Ahmedabad", "Alexandria", "Atlanta", "Baghdad", "Bangalore", "Bangkok", "Barcelona", "Beijing", "Belo Horizonte", "Bogotá", "Buenos Aires", "Cairo", "Chengdu", "Chennai", "Chicago", "Chongqing", "Dalian", "Dallas", "Dar es Salaam", "Delhi", "Dhaka", "Dongguan", "Foshan", "Fukuoka", "Guadalajara", "Guangzhou", "Hangzhou", "Harbin", "Ho Chi Minh City", "Hong Kong", "Houston", "Hyderabad", "Istanbul", "Jakarta", "Jinan", "Karachi", "Khartoum", "Kinshasa", "Kolkata", "Kuala Lumpur", "Lagos", "Lahore", "Lima", "London", "Los Angeles", "Luanda", "Madrid", "Manila", "Mexico City", "Miami", "Moscow", "Mumbai", "Nagoya", "Nanjing", "New York", "Osaka", "Paris", "Philadelphia", "Pune", "Qingdao", "Rio de Janeiro", "Riyadh", "Saint Petersburg", "Santiago", "Seoul", "Shanghai", "Shenyang", "Shenzhen", "Singapore", "Surat", "Suzhou", "São Paulo", "Tehran", "Tianjin", "Tokyo", "Toronto", "Washington", "Wuhan", "Xi'an", "Yangon" };
	string name = ynot::getline_ac(large_city_names, "type a large city's name");
	cout << "\nYou chose " << name << endl;
}

void test_getline_ac_menu()
{
	cout << "\nSample menu:"
		"\n * Create"
		"\n * Read"
		"\n * Update"
		"\n * Delete"
		"\n> ";
	string choice = ynot::getline_ac(
		{ "Create", "Read", "Update", "Delete" },
		"type an option");
	cout << "\nYou chose " << choice << "\n\n";
}

void test_getline_ac_numbered_menu()
{
	cout << "\nSample menu:"
		"\n 1. New"
		"\n 2. View"
		"\n 3. Edit"
		"\n 4. Remove"
		"\n> ";
	string choice = ynot::getline_ac({ "1", "2", "3", "4" });
	cout << "\nYou chose ";
	switch (choice[0])
	{
	case '1':
		cout << "New";
		break;
	case '2':
		cout << "View";
		break;
	case '3':
		cout << "Edit";
		break;
	case '4':
		cout << "Delete";
		break;
	}
	cout << "\n\n";
}

void test_getline_ac_with_empty_suggestion()
{
	cout << "\nEnter a month or just press enter to skip: ";
	string month = ynot::getline_ac({ "", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" });
	if (month.empty())
		cout << "\nYou skipped the question";
	else
		cout << "\nYou chose " << month;
}

void test_getline_ac_without_input_validation()
{
	cout << "\nEnter a color or make up your own:\n";
	string color = ynot::getline_ac(
		{ "red", "orange", "yellow", "green", "blue", "purple" },
		"type a color", ynot::opt::no_validation);
	cout << "\nYou entered: " << color << "\n";
}

void test_getline_ac_without_showing_suggestions()
{
	ifstream file("dictionary.txt");
	if (!file)
	{
		cout << "Error: could not load dictionary.txt";
		return;
	}
	vector<string> words;
	string word;
	cout << "\nLoading dictionary.txt";
	while (!file.eof())
	{
		file >> word;
		if (word.empty())
			throw runtime_error("Not loading words correctly.");
		words.push_back(word);
	}
	cout << "\nTry typing a few letters and pressing tab. Enter stop to stop.\n";
	string input = "";
	while (input != "stop")
	{
		input = ynot::getline_ac(words, "", ynot::opt::hide_suggestions);
		cout << " ";
	}
}

void test_wrap()
{
	Coord window_size = ynot::get_window_size();
	cout << "\n" << ynot::wrap("This is a very long sentence that will be nicely wrapped in the terminal window no matter what size the terminal window is, unless the terminal window changes size after this is printed.", window_size.x, "\n    ") << "\n";
}

void test_paginator()
{
	std::cout << "\nPaginator test. Press any key to start the paginator, and escape to close the paginator.";
	ynot::pause();
	std::string title = "Recursive descent parser";
	std::string line_prefix = "\n    ";
	std::string body = ynot::dedent(R"(
		In computer science, a recursive descent parser is a kind of top-down parser built from a set of mutually recursive procedures (or a non-recursive equivalent) where each such procedure implements one of the nonterminals of the grammar. Thus the structure of the resulting program closely mirrors that of the grammar it recognizes. [1]

		A predictive parser is a recursive descent parser that does not require backtracking. [2] Predictive parsing is possible only for the class of LL(k) grammars, which are the context - free grammars for which there exists some positive integer k that allows a recursive descent parser to decide which production to use by examining only the next k tokens of input. The LL(k) grammars therefore exclude all ambiguous grammars, as well as all grammars that contain left recursion. Any context - free grammar can be transformed into an equivalent grammar that has no left recursion, but removal of left recursion does not always yield an LL(k) grammar. A predictive parser runs in linear time.

		Recursive descent with backtracking is a technique that determines which production to use by trying each production in turn. Recursive descent with backtracking is not limited to LL(k) grammars, but is not guaranteed to terminate unless the grammar is LL(k). Even when they terminate, parsers that use recursive descent with backtracking may require exponential time.

		Although predictive parsers are widely used, and are frequently chosen if writing a parser by hand, programmers often prefer to use a table - based parser produced by a parser generator [citation needed], either for an LL(k) language or using an alternative parser, such as LALR or LR. This is particularly the case if a grammar is not in LL(k) form, as transforming the grammar to LL to make it suitable for predictive parsing is involved. Predictive parsers can also be automatically generated, using tools like ANTLR.

		Predictive parsers can be depicted using transition diagrams for each non - terminal symbol where the edges between the initial and the final states are labelled by the symbols(terminals and non - terminals) of the right side of the production rule. [3]

		Source: https://en.wikipedia.org/wiki/Recursive_descent_parser)");
	ynot::Paginator paginator(title, body, line_prefix);
	paginator.run();
	std::cout << endl;
}

void test_set_cursor_style()
{
	cout << "\nCursor visible";
	ynot::pause();
	ynot::set_cursor_style(CursorStyle::hidden);
	cout << "\nCursor not visible";
	ynot::pause();
	ynot::set_cursor_style(CursorStyle::not_hidden);
	cout << "\nCursor visible again";
	ynot::pause();
	ynot::set_cursor_style(CursorStyle::blinking_default);
	cout << "\nBlinking default cursor";
	ynot::pause();
	ynot::set_cursor_style(CursorStyle::steady_default);
	cout << "\nSteady default cursor";
	ynot::pause();
	ynot::set_cursor_style(CursorStyle::blinking_block);
	cout << "\nBlinking block cursor";
	ynot::pause();
	ynot::set_cursor_style(CursorStyle::steady_block);
	cout << "\nSteady block cursor";
	ynot::pause();
	ynot::set_cursor_style(CursorStyle::blinking_underline);
	cout << "\nBlinking underline cursor";
	ynot::pause();
	ynot::set_cursor_style(CursorStyle::steady_underline);
	cout << "\nSteady underline cursor";
	ynot::pause();
	ynot::set_cursor_style(CursorStyle::blinking_i_beam);
	cout << "\nBlinking i-beam cursor";
	ynot::pause();
	ynot::set_cursor_style(CursorStyle::steady_i_beam);
	cout << "\nSteady i-beam cursor";
	ynot::pause();
	ynot::set_cursor_style(CursorStyle::blinking_default);
	cout << "\nBack to the default cursor";
	ynot::pause();
	cout << "\n\n";
}

void test_set_window_title()
{
	ynot::set_window_title("hey look, a custom title!");
	cout << "Title set to \"hey look, a custom title!\" The title will only be "
		"visible in some terminals such as Command Prompt.\n\n";
	ynot::pause();
}

void test_wide_set_window_title()
{
	ynot::set_window_title(L"a custom title with emoji! 🔥");
	cout << "Title set to \"a custom title with emoji! (flame emoji here)\" The "
		"title will only be visible in some terminals such as Command Prompt.\n";
	ynot::pause();
}

void test_wide_print()
{
	ynot::wprint(L"A message with emoji! ✨ The emoji will not be visible in some "
		"terminals such as Command Prompt.\n");
	ynot::pause();
}

void test_print_styled()
{
	ynot::print_styled("This is underlined.", { Style::underlined });
	ynot::pause();
}

void test_wide_print_styled()
{
	ynot::print_styled(L"\nThis text has a bright red background.", { Style::bright_bg_red });
	ynot::pause();
}

void test_print_multi_styled()
{
	ynot::print_styled("\nThis is magenta with a green background and strikethrough. "
		"The strikethrough is only visible in some terminals such as Windows Terminal.",
		{ Style::magenta, Style::bg_green, Style::strikethrough });
	ynot::pause();
}

void test_wide_print_multi_styled()
{
	ynot::print_styled(L"\nThis is overlined, double underlined, and italic with an emoji: ✅. "
		"These are only visible in some terminals such as Windows Terminal.\n",
		{ Style::double_underlined, Style::overlined, Style::italic });
	ynot::pause();
}

void test_set_style()
{
	ynot::set_style({ Style::bright_blue });
	cout << "This is \"bright\" blue.\n\n";
	ynot::reset_style();
	ynot::pause();
}

void test_set_multi_style()
{
	ynot::set_style({ Style::bg_white, Style::black, Style::bold });
	cout << "This is black and bold with a white background.\n\n";
	ynot::reset_style();
	ynot::pause();
}

void test_reset_style()
{
	ynot::set_style({ Style::red });
	cout << "This is red.\n\n";
	ynot::reset_style();
	cout << "This is the default color.\n\n";
	ynot::pause();
}

void test_print_rgb()
{
	ynot::print_rgb(95, 255, 95, "This is bright green.\n");
	ynot::pause();
}

void test_print_invalid_rgb()
{
	cout << "Attempting to use an invalid rgb value.\n";
	try
	{
		ynot::print_rgb(95, 256, 95, "This will raise an exception.");
	}
	catch (invalid_argument& e)
	{
		cout << e.what() << endl;
	}
}

void test_wide_print_rgb()
{
	ynot::print_rgb(37, 100, 188, L"This is blue, and here's an emoji that's only visible in "
		"some terminals like Windows Terminal: ⚓.\n");
	ynot::pause();
}

void test_print_background_rgb()
{
	ynot::print_bg_rgb(242, 203, 30, "This has a yellow background.\n");
	ynot::pause();
}

void test_wide_print_background_rgb()
{
	ynot::print_bg_rgb(183, 84, 4, L"This has a brown/orange background, and here's an "
		"emoji that's only visible in some terminals like Windows Terminal: ☃.\n");
	ynot::pause();
}

void test_save_cursor_coords()
{
	cout << "Saving cursor location after this message.\n";
	ynot::save_cursor_location();
	ynot::pause();
}

void test_print_at()
{
	ynot::set_style({ Style::red });
	ynot::print_at(0, 0, "This is in the top left corner of the window.");
	ynot::reset_style();
	ynot::pause();
}

void test_wide_print_at()
{
	ynot::set_style({ Style::bg_blue });
	ynot::print_at(10, 10, L"This starts at coordinates (10,10), and here's an emoji "
		"that's only visible in some terminals like Windows Terminal: ☔.\n");
	ynot::reset_style();
	ynot::pause();
}

void test_set_cursor_coordinates()
{
	ynot::set_style({ Style::bright_bg_magenta });
	ynot::set_cursor_coords(5, 5);
	cout << "This starts at coordinates (5,5).\n";
	ynot::reset_style();
	ynot::pause();
}

void test_get_cursor_location()
{
	ynot::set_style({ Style::bright_bg_green });
	Coord coords = ynot::get_cursor_coords();
	cout << "The cursor was at coordinates (" << coords.x << "," << coords.y << ") at "
		"the start of this message.";
	ynot::reset_style();
	ynot::pause();
}

void test_restore_cursor_location()
{
	ynot::restore_cursor_location();
	ynot::print_rgb(105, 127, 68, "Restored to the saved cursor location.");
	ynot::pause();
}

void test_move_cursor_up()
{
	ynot::set_style({ Style::bg_magenta });
	ynot::move_cursor_up(3);
	cout << "This is 3 lines higher than the last output.";
	ynot::reset_style();
	ynot::pause();
}

void test_move_cursor_down()
{
	ynot::set_style({ Style::bg_magenta });
	ynot::move_cursor_down(3);
	cout << "This is 3 lines lower than the last output.";
	ynot::reset_style();
	ynot::pause();
}

void test_move_cursor_right()
{
	cout << endl;
	ynot::set_style({ Style::bg_blue });
	ynot::move_cursor_right(3);
	cout << "This is 3 columns to the right from the window's edge.\n";
	ynot::reset_style();
	ynot::pause();
}

void test_move_cursor_left()
{
	ynot::save_cursor_location();
	ynot::move_cursor_right(20);
	ynot::move_cursor_up(15);
	ynot::set_style({ Style::bg_green });
	cout << "This is higher and further to the right.";
	ynot::pause();
	ynot::move_cursor_left(5);
	ynot::move_cursor_down();
	ynot::set_style({ Style::bg_cyan });
	cout << "This is 5 columns to the left and 1 line below the end of the last output.";
	ynot::reset_style();
	ynot::restore_cursor_location();
	ynot::pause();
}

void test_get_window_size()
{
	Coord window_size = ynot::get_window_size();
	cout << "\nThe window has a size of (" << window_size.x << "," << window_size.y << ").\n";
	cout << "Try resizing the window now.\n";
	ynot::pause();
	window_size = ynot::get_window_size();
	cout << "Now the window has a size of (" << window_size.x << "," << window_size.y << ").\n";
	ynot::pause();
}

void test_getch_()
{
	cout << "Waiting for you to press a key.\n";
	char input = ynot::getch_();
	cout << "You pressed " << input << endl;
	ynot::pause();
}

void test_kbhit__()
{
	cout << "Waiting for you to press a key.";
	while (!ynot::kbhit__());
	cout << "\nYou pressed a key.\n";
	string _ = ynot::get_key();
}

void test_getch_if_kbhit()
{
	ynot::set_cursor_style(CursorStyle::hidden);
	char input = 0;
	for (int i = 0; "yes"; i++)
	{
		cout << "\r(" << i << ") Waiting for you to press a key without blocking.";
		if (ynot::kbhit__())
			input = ynot::getch_();
		if (input != 0)
			break;
		ynot::sleep_(1000);
	}
	ynot::set_cursor_style(CursorStyle::not_hidden);
	cout << "\nYou pressed " << input << endl;
	ynot::pause();
}

void test_insert()
{
	string message = "This is printed first.";
	cout << message;
	ynot::move_cursor_left(message.size());
	ynot::insert("This is printed second, inserted in front without overwriting. ");
	ynot::pause();
}

void test_wide_insert()
{
	cout << endl;
	wstring message = L"This is also printed first.";
	ynot::wprint(message);
	ynot::move_cursor_left(message.size());
	ynot::insert(L"Now this is also inserted, with emoji: ☕. ");
	// TODO: figure out why the first letter of the first output gets overwritten in
	// Windows Terminal and not Command Prompt. Does it have something to do with the
	// use of wide characters?
	ynot::pause();
}

void test_delete_chars()
{
	cout << "\nHere is yet more text. ";
	string message1 = "This is sentence is about to be deleted. ";
	cout << message1;
	string message2 = "This sentence will move to the left.";
	cout << message2;
	ynot::pause();
	ynot::move_cursor_left(message1.size() + message2.size());
	ynot::delete_chars(message1.size());
}

void test_insert_lines()
{
	cout << "\n-----------\n";
	ynot::move_cursor_up();
	ynot::pause();
	ynot::insert_lines(1);
	cout << "The line of minuses moved down.\n\n";
	ynot::pause();
}

void test_delete_lines()
{
	cout << "This line is about to disappear.\nSo is this one.";
	ynot::pause();
	ynot::move_cursor_up(1);
	ynot::delete_lines(2);
}

void test_alternate_screen_buffer()
{
	cout << "Now opening the alternate screen buffer.\n";
	ynot::pause();
	ynot::alternate_screen_buffer();
	cout << "Here is the alternate screen buffer.\n\n";
	cout << "Now clearing the screen.\n";
	ynot::pause();
	ynot::clear_screen();
	cout << "Now restoring the original screen buffer.\n";
	ynot::pause();
	ynot::restore_screen_buffer();
	cout << "And we're back.\n";
	ynot::pause();
}

void test_sleep_()
{
	cout << "Sleeping for 5 seconds.\n";
	ynot::sleep_(5000);
	cout << "5 seconds have passed.\n";
}

void test_get_key_with_cursor_movements()
{
	ynot::pause();
	cout << "\nTry moving the cursor with the arrow keys. Press any other key to stop.";
	ynot::set_cursor_style(CursorStyle::steady_default);
	while (true)
	{
		string input = ynot::get_key();
		if (input == "up arrow")
			ynot::move_cursor_up();
		else if (input == "left arrow")
			ynot::move_cursor_left();
		else if (input == "down arrow")
			ynot::move_cursor_down();
		else if (input == "right arrow")
			ynot::move_cursor_right();
		else
		{
			ynot::set_cursor_style(CursorStyle::blinking_default);
			return;
		}
	}
}
