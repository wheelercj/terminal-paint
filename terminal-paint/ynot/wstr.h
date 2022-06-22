#pragma once
#include <string>
#include <vector>
#include <limits.h>  // INT_MAX

namespace ynot
{
	/* Makes an entire string lowercase. */
	std::wstring to_lower(std::wstring str);

	/* Makes an entire string uppercase. */
	std::wstring to_upper(std::wstring str);

	/* Returns part of a given string. The start, end, and step parameters can be
	   negative. A negative start and/or end counts from the end of the string
	   instead of the beginning. A negative step reverses the result. An end
	   value greater than the length of the string is changed to length of the
	   string. The start is inclusive but the end is exclusive. */
	std::wstring slice(std::wstring str, int start, int end = INT_MAX, int step = 1);

	/* Splits a string by a substring. Throws std::invalid_argument if split_by
	   is an empty string. */
	std::vector<std::wstring> split(std::wstring str, std::wstring split_by = L" ");

	/* Joins strings, optionally putting another string between them. */
	std::wstring join(std::vector<std::wstring> strings, std::wstring join_by = L"");

	/* Removes characters from the edges of a string. */
	std::wstring strip(std::wstring str, std::wstring characters);

	/* Removes characters from the left edge of a string. */
	std::wstring lstrip(std::wstring str, std::wstring characters);

	/* Removes characters from the right edge of a string. */
	std::wstring rstrip(std::wstring str, std::wstring characters);

	/* Centers text, filling with a chosen character if needed.
	   Returns the string unchanged if width is <= the string's width. */
	std::wstring center(std::wstring str, int width, wchar_t fill_char = ' ');

	/* Justifies text to the left, filling with a chosen character if needed.
	   Returns the string unchanged if width is <= the string's width. */
	std::wstring ljust(std::wstring str, int width, wchar_t fill_char = ' ');

	/* Justifies text to the right, filling with a chosen character if needed.
	   Returns the string unchanged if width is <= the string's width. */
	std::wstring rjust(std::wstring str, int width, wchar_t fill_char = ' ');

	/* Determines whether a string starts with a given substr. */
	bool startswith(std::wstring str, std::wstring prefix);

	/* Determines whether a string ends with a given substr. */
	bool endswith(std::wstring str, std::wstring suffix);

	/* Prepends a prefix to each line of a multiline string. */
	std::wstring indent(std::wstring str, std::wstring line_prefix = L"    ");

	/* Evenly removes leading whitespace from each line of a multiline string. */
	std::wstring dedent(std::wstring str);

	/* Wraps words cleanly into lines. Throws std::invalid_argument if the wrap
	   width is not greater than the line prefix and suffix widths. */
	std::wstring wrap(std::wstring str, int width, std::wstring line_prefix = L"\n", std::wstring line_suffix = L"");

	/* Determines whether a string contains a given substring. */
	bool contains(std::wstring str, std::wstring substr);

	/* Determines whether a string contains a given character. */
	bool contains(std::wstring str, wchar_t ch);

	/* Finds the index of the next space after an optional starting index.
	   Returns -1 if there is no next space. */
	int find_next_space(std::wstring str, size_t start = 0);

	/* Finds the index of the latest space before an optional starting index.
	   Returns -1 if there is no previous space. */
	int find_previous_space(std::wstring str, size_t start = std::wstring::npos);
}
