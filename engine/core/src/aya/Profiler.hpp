#pragma once

#include <stdint.h>

#include "RbxFormat.hpp" // for RBX_PRINTF_ATTR

#ifndef __APPLE__
#define RBXPROFILER
#endif

void ExternalMicroProfileDrawText(int nX, int nY, uint32_t nColor, const char* pText, uint32_t nNumCharacters);

namespace Aya
{
namespace Profiler
{
typedef uint64_t Token;

Token getToken(const char* group, const char* name, int color = -1);
Token getLabelToken(const char* group);
Token getCounterToken(const char* name);

uint64_t enterRegion(Token token);
void leaveRegion(Token token, uint64_t enterTimestamp);

void addLabel(Token token, const char* name);
RBX_PRINTF_ATTR(2, 3) void addLabelFormat(Token token, const char* name, ...);

void counterAdd(Token token, long long count);
void counterSet(Token token, long long count);

void onThreadCreate(const char* name);
void onThreadExit();

void onFrame();

enum Flags
{
    Flag_MouseMove = 1 << 0,
    Flag_MouseWheel = 1 << 1,
    Flag_MouseDown = 1 << 2,
    Flag_MouseUp = 1 << 3,
};

void gpuInit(void* context);
void gpuShutdown();

bool isCapturingMouseInput();
bool handleMouse(unsigned int flags, int mouseX, int mouseY, int mouseWheel, int mouseButton);

bool forceOn();
bool forceOff();
bool toggleVisible();
bool togglePause();

struct Renderer
{
    virtual ~Renderer() {}

    virtual void drawText(
        int x, int y, unsigned int color, const char* text, unsigned int length, unsigned int textWidth, unsigned int textHeight) = 0;
    virtual void drawBox(int x0, int y0, int x1, int y1, unsigned int color0, unsigned int color1) = 0;
    virtual void drawLine(unsigned int vertexCount, const float* vertexData, unsigned int color) = 0;
};

bool isVisible();
void render(Renderer* renderer, unsigned int width, unsigned int height);

struct Scope
{
    Token token;
    uint64_t timestamp;

    Scope(Token token)
        : token(token)
    {
        timestamp = enterRegion(token);
    }

    ~Scope()
    {
        leaveRegion(token, timestamp);
    }
};
} // namespace Profiler
} // namespace Aya

#define RBXPROFILER_TOKEN_PASTE0(a, b) a##b
#define RBXPROFILER_TOKEN_PASTE(a, b) RBXPROFILER_TOKEN_PASTE0(a, b)

#ifdef RBXPROFILER
#define RBXPROFILER_SCOPE(group, name, ...) \
    static ::Aya::Profiler::Token RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__) = ::Aya::Profiler::getToken(group "", name "", ##__VA_ARGS__); \
    ::Aya::Profiler::Scope RBXPROFILER_TOKEN_PASTE(profscope, __LINE__)(RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__))
#define RBXPROFILER_LABEL(group, label) \
    static ::Aya::Profiler::Token RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__) = ::Aya::Profiler::getLabelToken(group ""); \
    ::Aya::Profiler::addLabel(RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__), label)
#define RBXPROFILER_LABELF(group, label, ...) \
    static ::Aya::Profiler::Token RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__) = ::Aya::Profiler::getLabelToken(group ""); \
    ::Aya::Profiler::addLabelFormat(RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__), label, ##__VA_ARGS__)
#define RBXPROFILER_COUNTER_ADD(name, count) \
    static ::Aya::Profiler::Token RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__) = ::Aya::Profiler::getCounterToken(name ""); \
    ::Aya::Profiler::counterAdd(RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__), static_cast<long long>(count))
#define RBXPROFILER_COUNTER_SUB(name, count) \
    static ::Aya::Profiler::Token RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__) = ::Aya::Profiler::getCounterToken(name ""); \
    ::Aya::Profiler::counterAdd(RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__), -static_cast<long long>(count))
#define RBXPROFILER_COUNTER_SET(name, count) \
    static ::Aya::Profiler::Token RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__) = ::Aya::Profiler::getCounterToken(name ""); \
    ::Aya::Profiler::counterSet(RBXPROFILER_TOKEN_PASTE(proftoken, __LINE__), count)
#else
#define RBXPROFILER_SCOPE(group, name, ...) (void)0
#define RBXPROFILER_LABEL(group, label) (void)0
#define RBXPROFILER_LABELF(group, label, ...) (void)sizeof(0, __VA_ARGS__)
#define RBXPROFILER_COUNTER_ADD(name, count) (void)0
#define RBXPROFILER_COUNTER_SUB(name, count) (void)0
#define RBXPROFILER_COUNTER_SET(name, count) (void)0
#endif
