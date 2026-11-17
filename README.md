# Raylib Doodling

1. Open `RaylibDoodling.sln` in Visual Studio 2022.
2. Select **Debug** and **x64** in the toolbar.
3. Press **F5**, or **Ctrl+F5** to run without the debugger.

If Visual Studio asks to reload projects changed outside the editor, accept.
If necessary, right-click **RaylibDoodling** and choose **Set as Startup Project**.

The current example opens a window displaying "Raylib works!". Press Escape to
exit. Start experimenting in `RaylibDoodling/main.cpp`.

## Library setup

`raylib.props` points at the existing sources in `C:\raylib\raylib\src\`.
Change `RaylibSourceDir` there if you move raylib; keep the trailing backslash.
Both projects share this setting, including the IntelliSense include path.

The `raylib` project automatically builds a static `raylib.lib` before the demo.
Debug and Release each use the matching MSVC runtime; x64 and x86 (Win32) each
get their own library. No raylib DLL or package download is needed.

The demo's linker settings include `raylib.lib`, `opengl32.lib`, `gdi32.lib`,
and `winmm.lib`, following the [official Visual Studio example](https://github.com/raysan5/raylib/blob/master/projects/VS2022/examples/core_basic_window.vcxproj).
Libraries are written under `build/raylib/lib/<platform>/<configuration>/`;
the executable is under `bin/<platform>/<configuration>/RaylibDoodling.exe`.
The debugger uses `RaylibDoodling/` as its working directory for future assets.

