#pragma once
#include "terminal.h"
#include <string>
#include <vector>
#include <optional>  // C++17+

namespace ynot
{

    enum class opt
    {
        none = 0x00,
        no_validation = 0x01,
        case_sensitive = 0x02,
        hide_suggestions = 0x04
    };

    /* Gets a line of input while showing autocomplete suggestions (not autocorrect).
       Press tab to accept a suggestion.
       Optional arguments:
       * a default message to show
       * flags for choosing options that can be combined with the bitwise OR operator (|):
            * opt::none - for not choosing any special options
            * opt::no_validation - don't require using a suggestion
            * opt::case_sensitive - require case to match that of suggestions for suggestions to match
            * opt::hide_suggestions - hide suggestions
       * the maximum width of the input field, or -1 for no limit */
    std::string getline_ac(
        const std::vector<std::string>& suggestions,
        std::string default_message = "",
        opt options = opt::none,
        int max_width = -1);

    class InputField
    {
    public:
        InputField(
            const std::vector<std::string>& suggestions,
            std::string default_message,
            bool must_use_suggestion,
            bool case_sensitive,
            bool show_suggestions,
            int max_width);
        std::string getline_ac();
    private:
        std::string key = "";
        std::string input = "";
        size_t input_index = 0;
        std::optional<std::string> latest_suggestion;
        Coord start = get_cursor_coords();
        Coord current = start;
        Coord input_end = current;
        Coord suggestion_end = current;
        std::vector<std::string> suggestions;
        std::string default_message;
        bool must_use_suggestion;
        bool case_sensitive;
        bool show_suggestions;
        int max_width;

        void find_and_print_suggestion();
        std::optional<std::string> find_suggestion();
        bool is_suggestion(std::string& str);
        void clear_suggestion();
        void red_flash_text();
        void print_suggestion(std::string suggestion);
        std::optional<std::string> key_enter();
        std::optional<std::string> key_tab();
        void key_char();
        void key_backspace();
        void key_ctrl_backspace();
        void key_delete();
        void key_ctrl_delete();
        void key_left_arrow();
        void key_right_arrow();
        void key_up_arrow();
        void key_down_arrow();
        void key_home();
        void key_end();
        void key_ctrl_left_arrow();
        void key_ctrl_right_arrow();
    };
}
