setlocal enabledelayedexpansion
for /r %%i in (.\src\*.frag, .\src\*.vert) do (
	set b=%%~xi
	set b=!b:.=!
	%VULKAN_SDK%/Bin/glslangValidator.exe -V -o %%~ni_!b!.spv %%i
)
pause
