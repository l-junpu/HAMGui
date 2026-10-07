# HAMGui

## Notes

- `BuildScript.bat` builds glfw 3.3.8 and spdlog in Debug and Release modes using Visual Studio 2022, x64, C++20, and the `v143` toolset; manual builds of their solutions are not required by the script.

## To Do

- **Done:** Test mINI and create configuration files for window customization.
  - Fullscreen, resizable, docking enabled, etc.
  - Possible extension: configure buttons from within ImGui.
- **Partial:** Add customization options to toggle the console terminal and ImGui; ImGui panel toggling is not done yet.
- **Done:** Add spdlog logging to imterm.
- **Done:** Make a `spdlog_mt` wrapper class.
- **Planned (under consideration):** Allow runtime configuration of spdlog/imterm; this may be undesirable because it is slower.
- **Planned:** Write a batch file to compile and build required dependencies.
- **Planned:** Add input handling options, possibly using a bitset (as a utility class, with utility files reorganized).
  - Released
  - Held
  - Pressed
- **Planned:** Clean up im_term command-line commands.
- **Planned:** Allow users to add custom commands externally.
  - Intended for integration into our chat console.
- **Planned:** Allow creation of buttons with image overlays (similar to clicking on the left side of Discord).

## Contributing

Contributions are welcome. Please open an issue to discuss proposed changes or submit a pull request with a clear description of your changes.
