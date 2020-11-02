#include "Win32OpenGL.h"

#include <stdio.h>

#include <gl/GL.h>
#include <glext.h>
#include <wglext.h>

global PFNWGLCHOOSEPIXELFORMATARBPROC     wglChoosePixelFormatARB;
global PFNWGLCREATECONTEXTATTRIBSARBPROC  wglCreateContextAttribsARB;
global PFNWGLMAKECONTEXTCURRENTARBPROC    wglMakeContextCurrentARB;
global PFNWGLSWAPINTERVALEXTPROC          wglSwapIntervalEXT;

void *LoadOpenGLProcedure(char *name)
{
    void *proc = (void *)wglGetProcAddress(name);
    if (!proc)
    {
        printf("Failed to load OpenGL procedure: %s\n", name);
    }

    return proc;
}

HGLRC InitializeOpenGLContext(HDC deviceContext)
{
    HGLRC result = 0;

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,
        8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);

    if (pixelFormat)
    {
        SetPixelFormat(deviceContext, pixelFormat, &pfd);
        HGLRC dummyContext = wglCreateContext(deviceContext);
        wglMakeCurrent(deviceContext, dummyContext);

        {
            wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
            wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
            wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)wglGetProcAddress("wglMakeContextCurrentARB");
            wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
        }

        int pixelFormatAttribs[] = {
            WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
            WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
            WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
            WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
            WGL_COLOR_BITS_ARB, 32,
            WGL_DEPTH_BITS_ARB, 24,
            WGL_STENCIL_BITS_ARB, 8,
            0
        };

        UINT numFormats = 0;
        wglChoosePixelFormatARB(deviceContext, pixelFormatAttribs, NULL, 1, &pixelFormat, &numFormats);

        if (pixelFormat)
        {
            int contextAttribs[] = {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
                WGL_CONTEXT_MINOR_VERSION_ARB, 6,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB
            };

            result = wglCreateContextAttribsARB(deviceContext, dummyContext, contextAttribs);
            if (result)
            {
                wglMakeCurrent(deviceContext, NULL);
                wglDeleteContext(dummyContext);
                wglMakeCurrent(deviceContext, result);
                wglSwapIntervalEXT(0);
            }
        }
    }

    return result;
}

void CleanUpOpenGLContext(HDC deviceContext, HGLRC openglContext)
{
    wglMakeCurrent(deviceContext, NULL);
    wglDeleteContext(openglContext);
}

void OpenGLSetVerticalSync(b32 vsync)
{
    wglSwapIntervalEXT(!!vsync);
}

void OpenGLSwapBuffers(HDC deviceContext)
{
    wglSwapLayerBuffers(deviceContext, WGL_SWAP_MAIN_PLANE);
}