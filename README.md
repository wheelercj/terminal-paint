# terminal paint

Draw in the terminal using your mouse!

![demo](https://media.giphy.com/media/XAUcYQJnqrdlEYhhLE/giphy.gif)

## features

* Left click to draw, right click to erase.
* Draw with any Unicode symbol including emoji.
* Save to and load from files.

## requirements

* The windows.h header file.
* C++17 or newer. If you're using Visual Studio, you can choose the version of C++ with project > Properties > C/C++ > Language > C++ Language Standard.
* Some features require using a modern terminal such as [Windows Terminal](https://aka.ms/terminal).

## usage

Make sure any of this app's files that have emoji or other high Unicode symbols in them are encoded with the [UTF-8 encoding](https://docs.microsoft.com/en-us/visualstudio/ide/how-to-save-and-open-files-with-encoding?view=vs-2022) (_without_ BOM/signature).

## how it works

This app's menus were created with the [ynot library](https://github.com/wheelercj/ynot), and the drawing feature itself was built with [Microsoft's console API](https://docs.microsoft.com/en-us/windows/console/reading-input-buffer-events).
