# Setting Up

1. Configure VS Code IntelliSense:
   - Press `Ctrl + Shift + P`
   - Select `C/C++: Select IntelliSense Configuration`
   - Choose `gcc.exe`

2. Create the following directories:
   - `src/` — All source files (`.cpp`)
   - `external/` — External library dependencies (GLAD, GLFW, etc.)
   - `include/` — Class header files (`.h`)

3. Create `CMakeLists.txt` and `CMakePresets.json`.

4. Run Build & Debug in VS Code to generate `.vscode/tasks.json`.

5. Add custom build and run tasks.