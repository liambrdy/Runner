#include <Windows.h>

#include "Platform.h"

#include "Win32OpenGL.h"

#include <gl/GL.h>

global b32 globalRunning;

internal LRESULT CALLBACK MainWindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (message)
    {
        case WM_QUIT:
        case WM_CLOSE:
        {
            globalRunning = false;
        } break;

        default:
        {
            result = DefWindowProc(window, message, wParam, lParam);
        }
    }

    return result;
}

internal void ProcessPendingMessages(PlatformInput *input)
{
    MSG message;
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
    {
        u32 type = message.message;
        WPARAM wParam = message.wParam;
        LPARAM lParam = message.lParam;
        
        switch (type)
        {
            case WM_LBUTTONDOWN:
            {
                if (!input->leftMouseDown)
                {
                    input->leftMousePressed = true;
                }
                input->leftMouseDown = true;
            } break;

            case WM_LBUTTONUP:
            {
                input->leftMouseDown = false;
                input->leftMousePressed = false;
            } break;

            case WM_RBUTTONDOWN:
            {
                if (!input->rightMouseDown)
                {
                    input->rightMousePressed = true;
                }
                input->rightMouseDown = true;
            } break;

            case WM_RBUTTONUP:
            {
                input->rightMouseDown = false;
                input->rightMousePressed = false;
            } break;

            case WM_MOUSEWHEEL:
            {
                u16 wheelDelta = HIWORD(wParam);
                input->mouseScrollY = (f32)wheelDelta;
            } break;

            case WM_MOUSEHWHEEL:
            {
                u16 wheelDelta = HIWORD(wParam);
                input->mouseScrollX = (f32)wheelDelta;
            } break;

            case WM_KEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            {
                u64 vkCode = wParam;
                b32 isDown = !(lParam & (1 << 30));

                u64 keyInput = 0;

                if ((vkCode >= 'A' && vkCode <= 'Z') || (vkCode >= '0' && vkCode <= '9'))
                {
                    keyInput = (vkCode >= 'A' && vkCode <= 'Z') ? KEY_a + (vkCode - 'A') : KEY_0 + (vkCode - '0');
                }
                else
                {
                    if (vkCode == VK_ESCAPE)
                    {
                        keyInput = KEY_esc;
                    }
                    else if (vkCode >= VK_F1 && vkCode <= VK_F12)
                    {
                        keyInput = KEY_f1 + (vkCode - VK_F1);
                    }
                    else if (vkCode == VK_OEM_3)
                    {
                        keyInput = KEY_grave_accent;
                    }
                    else if (vkCode == VK_OEM_MINUS)
                    {
                        keyInput = KEY_minus;
                    }
                    else if (vkCode == VK_OEM_PLUS)
                    {
                        keyInput = KEY_equal;
                    }
                    else if (vkCode == VK_BACK)
                    {
                        keyInput = KEY_backspace;
                    }
                    else if (vkCode == VK_DELETE)
                    {
                        keyInput = KEY_delete;
                    }
                    else if (vkCode == VK_TAB)
                    {
                        keyInput = KEY_tab;
                    }
                    else if (vkCode == VK_SPACE)
                    {
                        keyInput = KEY_space;
                    }
                    else if (vkCode == VK_RETURN)
                    {
                        keyInput = KEY_enter;
                    }
                    else if (vkCode == VK_CONTROL)
                    {
                        keyInput = KEY_ctrl;
                    }
                    else if (vkCode == VK_SHIFT)
                    {
                        keyInput = KEY_ctrl;
                    }
                    else if (vkCode == VK_MENU)
                    {
                        keyInput = KEY_alt;
                    }
                    else if (vkCode == VK_UP)
                    {
                        keyInput = KEY_up;
                    }
                    else if (vkCode == VK_DOWN)
                    {
                        keyInput = KEY_down;
                    }
                    else if (vkCode == VK_LEFT)
                    {
                        keyInput = KEY_left;
                    }
                    else if (vkCode == VK_RIGHT)
                    {
                        keyInput = KEY_right;
                    }
                    else if (vkCode == VK_PRIOR)
                    {
                        keyInput = KEY_page_up;
                    }
                    else if (vkCode == VK_NEXT)
                    {
                        keyInput = KEY_page_down;
                    }
                }

                if (isDown)
                {
                    if (!input->keyDown[keyInput])
                    {
                        input->keyPressed[keyInput] = true;
                    }
                    input->keyDown[keyInput] = true;
                }
                else
                {
                    input->keyDown[keyInput] = false;
                    input->keyPressed[keyInput] = false;
                }
            } break;

            default:
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
    }
}

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE previousInstance, PWSTR commandLine, int commandShow)
{
    char *className = "RunnerWindowClass";

    WNDCLASSA windowClass = {};
    windowClass.hInstance = instance;
    windowClass.lpszClassName = className;
    windowClass.lpfnWndProc = MainWindowProcedure;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;

    if (!SUCCEEDED(RegisterClass(&windowClass)))
    {
        //TODO: Logging
        goto classFailed;
    }

    HWND window = CreateWindow(className, "Runner", WS_OVERLAPPEDWINDOW,    
                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
                  NULL, NULL, instance, NULL);
    
    if (!window)
    {
        //TODO: Logging
        goto windowFailed;
    }

    ShowWindow(window, commandShow);
    UpdateWindow(window);

    HDC deviceContext = GetDC(window);
    HGLRC openglContext = InitializeOpenGLContext(deviceContext);

    b32 sleepIsGranular = (timeBeginPeriod(1) == TIMERR_NOERROR);
    LARGE_INTEGER performanceCounterFrequency;
    LARGE_INTEGER beginFrameTime;
    LARGE_INTEGER endFrameTime;

    QueryPerformanceFrequency(&performanceCounterFrequency);

    globalRunning = true;

    PlatformInput input = {};
    input.targetDeltaTime = 60.0f;

    while (globalRunning)
    {
        QueryPerformanceCounter(&beginFrameTime);

        ProcessPendingMessages(&input);

        {
            POINT mouse;
            GetCursorPos(&mouse);
            ScreenToClient(window, &mouse);
            input.mouseX = (f32)mouse.x;
            input.mouseY = (f32)mouse.y;
        }

        PlatformUpdate(&input);

        OpenGLSwapBuffers(deviceContext);

        QueryPerformanceCounter(&endFrameTime);

        {
            i64 desiredFrameCounts = (i64)(performanceCounterFrequency.QuadPart / input.targetDeltaTime);
            i64 frameCount = endFrameTime.QuadPart - beginFrameTime.QuadPart;
            i64 countsToWait = desiredFrameCounts - frameCount;

            LARGE_INTEGER beginWaitTime;
            LARGE_INTEGER endWaitTime;

            QueryPerformanceCounter(&beginWaitTime);

            while (countsToWait > 0)
            {
                if (sleepIsGranular)
                {
                    DWORD millisecondsToSleep = (DWORD)(1000.0f * ((f64)countsToWait / performanceCounterFrequency.QuadPart));
                    if (millisecondsToSleep > 1)
                    {
                        --millisecondsToSleep;
                        Sleep(millisecondsToSleep);
                    }
                }

                QueryPerformanceCounter(&endWaitTime);
                countsToWait -= endWaitTime.QuadPart - beginWaitTime.QuadPart;
                beginWaitTime = endWaitTime;
            }
        }
    }

    ReleaseDC(window, deviceContext);
    CleanUpOpenGLContext(deviceContext, openglContext);

    DestroyWindow(window);
windowFailed:
    UnregisterClass(className, instance);
classFailed:
    return 0;
}
