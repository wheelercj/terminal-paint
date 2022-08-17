#include "files.h"
#include <windows.h>
using namespace std;

string load_file(string file_path)
{
	HANDLE file_handle = CreateFileA(  // opens a file, doesn't create one
		file_path.c_str(),
		GENERIC_READ,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
	);
	if (file_handle == INVALID_HANDLE_VALUE)
	{
		ynot::notify("Error: failed to open the file.");
		return "";
	}
	DWORD file_size = GetFileSize(file_handle, 0);
	DWORD bytes_read = 0;
	string content;
	content.resize(size_t(file_size + 1));
	BOOL ok = ReadFile(file_handle, content.data(), file_size, &bytes_read, 0);
	CloseHandle(file_handle);
	if (!ok || bytes_read == 0 || !ynot::contains(content, "␝"))
	{
		ynot::notify("Error: opened but failed to read the file.");
		return "";
	}
	return content;
}

string choose_file_to_load()
{
	while (true)
	{
		OPENFILENAMEA ofna;
		char sz_file[100]{};
		ZeroMemory(&ofna, sizeof(ofna));
		ofna.lStructSize = sizeof(ofna);
		ofna.hwndOwner = NULL;
		ofna.lpstrFile = sz_file;
		ofna.lpstrFile[0] = '\0';
		ofna.nMaxFile = sizeof(sz_file);
		ofna.lpstrFilter = ".tpaint\0\0";
		ofna.lpstrDefExt = "tpaint\0";
		ofna.nFilterIndex = 1;
		ofna.lpstrFileTitle = NULL;
		ofna.nMaxFileTitle = 0;
		ofna.lpstrInitialDir = NULL;
		ofna.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER;
		BOOL ok = GetOpenFileNameA(&ofna);
		string file_path = ofna.lpstrFile;
		if (!ok)
			return "";
		else if (!ynot::endswith(file_path, ".tpaint"))
		{
			ynot::notify("Error: the file extension must be .tpaint");
			ynot::notify("Choose a .tpaint file to load.", false);
		}
		else
			return file_path;
	}
}

string create_save_file()
{
	while (true)
	{
		OPENFILENAMEA ofna;
		char sz_file[100]{};
		ZeroMemory(&ofna, sizeof(ofna));
		ofna.lStructSize = sizeof(ofna);
		ofna.hwndOwner = NULL;
		ofna.lpstrFile = sz_file;
		ofna.lpstrFile[0] = '\0';
		ofna.nMaxFile = sizeof(sz_file);
		ofna.lpstrFilter = ".tpaint\0\0";
		ofna.lpstrDefExt = "tpaint\0";
		ofna.nFilterIndex = 1;
		ofna.lpstrFileTitle = NULL;
		ofna.nMaxFileTitle = 0;
		ofna.lpstrInitialDir = NULL;
		BOOL ok = GetSaveFileNameA(&ofna);
		string file_path = ofna.lpstrFile;
		if (!ok)
		{
			ynot::notify("Canceled saving.");
			return "";
		}
		else if (!ynot::endswith(file_path, ".tpaint"))
		{
			ynot::notify("Error: the file extension must be .tpaint");
			ynot::notify("Choose a file name.", false);
		}
		else
			return file_path;
	}
}

string create_export_file()
{
	OPENFILENAMEA ofna;
	char sz_file[100]{};
	ZeroMemory(&ofna, sizeof(ofna));
	ofna.lStructSize = sizeof(ofna);
	ofna.hwndOwner = NULL;
	ofna.lpstrFile = sz_file;
	ofna.lpstrFile[0] = '\0';
	ofna.nMaxFile = sizeof(sz_file);
	ofna.lpstrFilter = "*.txt\0\0";
	ofna.lpstrDefExt = "txt\0";
	ofna.nFilterIndex = 1;
	ofna.lpstrFileTitle = NULL;
	ofna.nMaxFileTitle = 0;
	ofna.lpstrInitialDir = NULL;
	BOOL ok = GetSaveFileNameA(&ofna);
	if (ok)
		return ofna.lpstrFile;
	ynot::notify("Canceled exporting.");
	return "";
}
