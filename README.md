# HAMGui

Notes:
- glfw3.3.8 and spdlog requires the .sln to be manually built
	- Command goes something like this: cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -T "v142" -DCMAKE_CXX_STANDARD=20

To Do:
- Test out mINI to see how it works and create configuration files that allow for window customization                // Done - Might want to extend this to config buttons that can be set inside of imgui
	- Fullscreen / Resizable / Docking Enabled / Etc...
- Add customization option to toggle Console Terminal / ImGui							                              // Partially - ImGui panel toggling stuff not done yet
- Add spdlog log to imterm								         													  // Done
- Make spdlog_mt wrapper class                                                                                        // Done
- Allow for configuration of spdlog/imterm during runtime															  // Might not want to do this since its slower


- Write batch file to compile and build required
- Add input handling options - Maybe bitset (Add this as a utility class and reorganize utility files)
	- Released
	- Held
	- Pressed
- Clean up im_term command line commands
- Allow for users to add custom commands externally
	- This is for use when we integrate this library into our chat console
- Allow for creation of buttons that are able to have images overlayed (Similar to clicking on the left side of discord)
