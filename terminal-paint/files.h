#include "../ynot/ynot/ynot.h"
#include <string>

/* Returns the file path, or an empty string if canceled. */
std::string load_file(std::string file_path);

/* Returns the file path, or an empty string if canceled. */
std::string choose_file_to_load();

/* Returns the file path, or an empty string if canceled. */
std::string create_save_file();

/* Returns the file path, or an empty string if canceled. */
std::string create_export_file();
