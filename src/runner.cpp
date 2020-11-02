#include <Windows.h>

#include <stdio.h>

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE previousInstance, PWSTR commandLine, int commandShow)
{
    MessageBoxA(NULL, "This is a message Box", "This is the caption of the message box", MB_OK);

    return 0;
}