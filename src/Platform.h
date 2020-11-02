#ifndef __PLATFORM_H
#define __PLATFORM_H

#include <stdint.h>

#define global static
#define internal static
#define local_persist static

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef bool b32;

enum
{
#define Key(name, str) KEY_##name,
#include "PlatformKeyList.h"
#undef Key
    KEY_MAX
};

struct PlatformInput
{
    b32 keyPressed[KEY_MAX];
    b32 keyDown[KEY_MAX];

    f32 mouseX, mouseY;
    f32 mouseScrollX, mouseScrollY;
    b32 leftMouseDown, leftMousePressed;
    b32 rightMouseDown, rightMousePressed;

    f32 targetDeltaTime;
};

void PlatformUpdate(PlatformInput *input);

#endif // __PLATFORM_H