#pragma once
#include <SDL2/SDL.h>

class DummyWindow
{
public:
#ifndef __linux
    void* handle;
#else
    SDL_Window* handle;
#endif
    DummyWindow(int width, int height);
    ~DummyWindow(void);
};
