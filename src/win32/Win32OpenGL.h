#ifndef __WIN32_OPENGL_H
#define __WIN32_OPENGL_H

#include <Windows.h>

#include "Platform.h"

void *LoadOpenGLProcedure(char *name);

HGLRC InitializeOpenGLContext(HDC deviceContext);
void CleanUpOpenGLContext(HDC deviceContext, HGLRC openglContext);

void OpenGLSetVerticalSync(b32 sync);
void OpenGLSwapBuffers(HDC deviceContext);

#endif