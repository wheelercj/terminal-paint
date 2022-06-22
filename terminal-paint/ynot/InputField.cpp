#include "InputField.h"
#include "str.h"
#include "common.h"

namespace ynot
{

    std::string getline_ac(
            const std::vector<std::string>& suggestions,
            std::string default_message,
            opt options,
            int max_width)
            {
        bool must_use_suggestion = !(int(options) & int(opt::no_validation));
        bool case_sensitive = int(options) & int(opt::case_sensitive);
        bool show_suggestions = !(int(options) & int(opt::hide_suggestions));
        InputField input_field(
            suggestions,
            default_message,
            must_use_suggestion,
            case_sensitive,
            show_suggestions,
            max_width);
        return input_field.getline_ac();
    }

    InputField::InputField(
            const std::vector<std::string>& suggestions,
            std::string default_message,
            bool must_use_suggestion,
            bool case_sensitive,
            bool show_suggestions,
            int max_width)
            {
        this->suggestions = suggestions;
        this->default_message = default_message;
        this->must_use_suggestion = must_use_suggestion;
        this->case_sensitive = case_sensitive;
        this->show_suggestions = show_suggestions;
        if (default_message.size())
        {
            find_and_print_suggestion();
            this->latest_suggestion = default_message;
        }
        this->max_width = max_width;
    }

    std::string InputField::getline_ac()
    {
        std::optional<std::string> result;
        while (true)
        {
            this->key = get_key();
            if (this->key == "enter")
            {
                result = key_enter();
                if (result && (result->size() || !must_use_suggestion || is_suggestion(*result)))
                    return *result;
            }
            else if (this->key == "tab")
            {
                result = key_tab();
                if (result && (result->size() || is_suggestion(*result)))
                    return *result;
            }
            else if (this->key.size() == 1)
                key_char();
            else if (this->key == "backspace")
                key_backspace();
            else if (this->key == "ctrl+backspace")
                key_ctrl_backspace();
            else if (this->key == "delete")
                key_delete();
            else if (this->key == "ctrl+delete")
                key_ctrl_delete();
            else if (this->key == "left arrow")
                key_left_arrow();
            else if (this->key == "right arrow")
                key_right_arrow();
            else if (this->key == "up arrow")
                key_up_arrow();
            else if (this->key == "down arrow")
                key_down_arrow();
            else if (this->key == "home")
                key_home();
            else if (this->key == "end")
                key_end();
            else if (this->key == "ctrl+left arrow")
                key_ctrl_left_arrow();
            else if (this->key == "ctrl+right arrow")
                key_ctrl_right_arrow();
        }
    }

    void InputField::find_and_print_suggestion()
    {
        clear_suggestion();
        if (this->input.empty())
        {
            if (show_suggestions && this->default_message.size())
                print_suggestion(this->default_message);
            return;
        }
        this->latest_suggestion = find_suggestion();
        if (show_suggestions && this->latest_suggestion)
            print_suggestion(*this->latest_suggestion);
        set_cursor_coords(this->current);
    }

    std::optional<std::string> InputField::find_suggestion()
    {
        for (auto it = this->suggestions.begin(); it != this->suggestions.end(); it++)
        {
            std::string suggestion = *it;
            std::string lc_suggestion = suggestion;
            std::string lc_str = this->input;
            if (!this->case_sensitive)
            {
                lc_str = to_lower(lc_str);
                lc_suggestion = to_lower(lc_suggestion);
            }
            if (lc_suggestion.find(lc_str) == 0)
                return suggestion;
        }
        return {};
    }

    /* This function changes the capitalization of the parameter to match the
       suggestion if the function returns true. */
    bool InputField::is_suggestion(std::string& str)
    {
        for (auto it = this->suggestions.begin(); it != this->suggestions.end(); it++)
        {
            std::string suggestion = *it;
            std::string lc_suggestion = suggestion;
            std::string lc_str = str;
            if (!this->case_sensitive)
            {
                lc_str = to_lower(lc_str);
                lc_suggestion = to_lower(lc_suggestion);
            }
            if (lc_suggestion == lc_str)
            {
                str = suggestion;
                return true;
            }
        }
        return false;
    }

    void InputField::clear_suggestion()
    {
        set_cursor_coords(this->input_end);
        delete_chars(size_t(this->suggestion_end.x) - size_t(this->input_end.x));
        this->suggestion_end = this->input_end;
        this->latest_suggestion = {};
    }

    void InputField::red_flash_text()
    {
        set_cursor_style(CursorStyle::hidden);
        set_cursor_coords(this->start);
        print_styled(this->input, { Style::red });
        sleep_(300);
        set_cursor_coords(this->start);
        std::cout << this->input;
        set_cursor_coords(this->current);
        set_cursor_style(CursorStyle::not_hidden);
    }

    void InputField::print_suggestion(std::string suggestion)
    {
        if (suggestion.size() <= this->input.size())
            return;
        std::string suffix = suggestion.substr(this->input.size());
        print_styled(suffix, { Style::bright_gray });
        this->suggestion_end = get_cursor_coords();
        set_cursor_coords(this->current);
    }

    std::optional<std::string> InputField::key_enter()
    {
        if (!this->must_use_suggestion)
        {
            clear_suggestion();
            return this->input;
        }
        if (is_suggestion(this->input))
            return this->input;
        red_flash_text();
        return {};
    }

    std::optional<std::string> InputField::key_tab()
    {
        if (is_suggestion(this->input))
            return this->input;
        if (this->latest_suggestion && is_suggestion(*this->latest_suggestion))
        {
            set_cursor_coords(this->start);
            std::cout << *this->latest_suggestion;
            return *this->latest_suggestion;
        }
        return {};
    }

    void InputField::key_char()
    {
        if (this->input_index < this->input.size())
            this->input[this->input_index] = this->key[0];
        else if (this->input.size() == this->max_width)
            return;
        else
        {
            this->input.append(this->key);
            this->input_end.x += 1;
        }
        std::cout << this->key;
        this->current.x += 1;
        this->input_index += 1;
        find_and_print_suggestion();
    }

    void InputField::key_backspace()
    {
        if (this->current.x <= this->start.x || !this->input.size())
            return;
        this->input.erase(this->input_index - 1, 1);
        backspace_chars(1);
        this->current.x -= 1;
        this->input_end.x -= 1;
        this->input_index -= 1;
        find_and_print_suggestion();
    }

    void InputField::key_ctrl_backspace()
    {
        if (this->current.x <= this->start.x || !this->input.size())
            return;
        int s = find_previous_space(this->input, this->input_index - 1);
        if (s == -1)
            s = 0;
        int i = int(this->input_index);
        int diff = i - s;
        backspace_chars(diff);
        this->input.erase(s, diff);
        this->current.x -= diff;
        this->input_end.x -= diff;
        this->input_index -= diff;
        find_and_print_suggestion();
    }

    void InputField::key_delete()
    {
        if (this->current.x >= this->input_end.x)
            return;
        delete_chars(1);
        this->input_end.x -= 1;
        this->input.erase(this->input_index, 1);
        find_and_print_suggestion();
    }

    void InputField::key_ctrl_delete()
    {
        if (this->current.x >= this->input_end.x)
            return;
        int s = find_next_space(this->input, this->input_index + 1);
        if (s == -1)
            s = int(this->input.size());
        int i = int(this->input_index);
        int diff = s - i;
        delete_chars(diff);
        this->input.erase(i, diff);
        this->input_end.x -= diff;
        find_and_print_suggestion();
    }

    void InputField::key_left_arrow()
    {
        if (this->current.x <= this->start.x)
            return;
        this->current.x -= 1;
        this->input_index -= 1;
        set_cursor_coords(this->current);
    }

    void InputField::key_right_arrow()
    {
        if (this->current.x >= this->input_end.x)
            return;
        this->current.x += 1;
        this->input_index += 1;
        set_cursor_coords(this->current);
    }

    void InputField::key_up_arrow()
    {
        // TODO
    }

    void InputField::key_down_arrow()
    {
        // TODO
    }

    void InputField::key_home()
    {
        this->current = this->start;
        this->input_index = 0;
        set_cursor_coords(this->current);
    }

    void InputField::key_end()
    {
        this->current = this->input_end;
        this->input_index = this->input.size();
        set_cursor_coords(this->current);
    }

    void InputField::key_ctrl_left_arrow()
    {
        if (this->input_index <= 0)
            return;
        int i = find_previous_space(this->input, this->input_index - 1);
        if (i == -1)
            i = 0;
        this->current.x -= int(this->input_index) - i;
        this->input_index = i;
        set_cursor_coords(this->current);
    }

    void InputField::key_ctrl_right_arrow()
    {
        if (this->input_index >= this->input.size())
            return;
        int i = find_next_space(this->input, this->input_index + 1);
        if (i == -1)
            i = int(this->input.size());
        this->current.x += i - int(this->input_index);
        this->input_index = i;
        set_cursor_coords(this->current);
    }
}
