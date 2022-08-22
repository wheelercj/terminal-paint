# terminal paint

Draw in the terminal with your mouse or touchscreen!

![demo](https://media.giphy.com/media/9EJpbGOsraRrup46iY/giphy.gif)

This app requires the windows.h header file which is normally only available in Windows.

## features

* Left click to draw and right click to erase, or use a touchscreen/stylus/etc.
* Draw with any Unicode symbol including emoji.
* Choose from hundreds of color options.
* Box-drawing characters automatically join to each other.
* Easily save, load, and export your drawings.

## download

1. Open a terminal where you want this app's folder to be.
2. Use `git clone https://github.com/wheelercj/terminal-paint.git --recurse-submodules`
3. Build the app from the code. If you have Visual Studio, you can simply open the .sln file and choose the build option.
4. Run the app in a modern terminal such as [Windows Terminal](https://aka.ms/terminal). If you're using Windows 10, see [how to run your C++ app in Windows Terminal](https://wheelercj.github.io/notes/pages/20220506214620.html).

## how it works

This app's menus were created with the [ynot library](https://github.com/wheelercj/ynot), and the drawing feature itself was mostly built with [Microsoft's console API](https://docs.microsoft.com/en-us/windows/console/reading-input-buffer-events).
