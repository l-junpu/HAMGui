@echo OFF
cd /d %cd%
set HAMGUI=%cd%/HAMGui



powershell write-host -fore White ------------------------------------------------------------------------------------------------------
powershell write-host -fore White Searching for Visual Studio Path and MSBuild
powershell write-host -fore White ------------------------------------------------------------------------------------------------------
goto :FINDVSPATH



:FINDVSPATH
for /f "usebackq tokens=*" %%i in (`HAMGui\dep\vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
    SET MSBUILD=%%i
    GOTO :BREAK_OUT
)
:BREAK_OUT
for /f "usebackq tokens=1* delims=: " %%i in (`HAMGui\dep\vswhere -latest -requires Microsoft.VisualStudio.Workload.NativeDesktop`) do (
    if /i "%%i"=="installationPath" set VSPATH=%%j
)

IF EXIST "%MSBUILD%" ( 
    echo %VSPATH%
    GOTO :COMPILATION
    )
echo Failed to find MSBuild!!! 
GOTO :ERROR



:COMPILATION
powershell write-host -fore White ------------------------------------------------------------------------------------------------------
powershell write-host -fore White Building glfw and spdlog in Debug and Release modes
powershell write-host -fore White ------------------------------------------------------------------------------------------------------

cd /d %HAMGUI%/dep/glfw-3.3.8/glfw-3.3.8
rmdir build /s /q
cmake -S . -B build -D BUILD_SHARED_LIBS=ON -G "Visual Studio 17 2022" -A x64 -T "v142" -DCMAKE_CXX_STANDARD=20

cd /d %HAMGUI%/dep/spdlog-1.x/spdlog-1.x
rmdir build /s /q
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -T "v142" -DCMAKE_CXX_STANDARD=20

"%MSBUILD%" "%HAMGUI%/dep/glfw-3.3.8/glfw-3.3.8/build/GLFW.sln" /p:configuration=Debug /p:Platform="x64" /p:CppLanguageStandard=c++20 /verbosity:minimal
"%MSBUILD%" "%HAMGUI%/dep/glfw-3.3.8/glfw-3.3.8/build/GLFW.sln" /p:configuration=Release /p:Platform="x64" /p:CppLanguageStandard=c++20 /verbosity:minimal

"%MSBUILD%" "%HAMGUI%/dep/spdlog-1.x/spdlog-1.x/build/spdlog.sln" /p:configuration=Debug /p:Platform="x64" /p:CppLanguageStandard=c++20 /verbosity:minimal
"%MSBUILD%" "%HAMGUI%/dep/spdlog-1.x/spdlog-1.x/build/spdlog.sln" /p:configuration=Release /p:Platform="x64" /p:CppLanguageStandard=c++20 /verbosity:minimal



:DONE
powershell write-host -fore White ------------------------------------------------------------------------------------------------------
powershell write-host -fore White Successfully compiled all dependencies!
powershell write-host -fore White ------------------------------------------------------------------------------------------------------
goto :PAUSE

:ERROR
powershell write-host -fore Red ------------------------------------------------------------------------------------------------------
powershell write-host -fore Red Failed to compile dependencies!
powershell write-host -fore Red ------------------------------------------------------------------------------------------------------

:PAUSE
if %1.==. pause