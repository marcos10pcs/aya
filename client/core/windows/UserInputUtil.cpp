
#include "UserInputUtil.hpp"
#include "Utility/Rect.hpp"
#include "Utility/standardout.hpp"
#include "DataModel/GameBasicSettings.hpp"

#include <dinput.h>

using Aya::Rect;
using Aya::Vector2;


const float UserInputUtil::HybridSensitivity = 15.0f;
const float UserInputUtil::MouseTug = 20.0f;

bool UserInputUtil::isCtrlDown(Aya::ModCode modCode)
{
    return (modCode == Aya::KMOD_LCTRL || modCode == Aya::KMOD_RCTRL);
}


// horizontal - will keep doing deltas
// vertical - will peg inset 2 pixels
//
void UserInputUtil::wrapFullScreen(const Vector2& delta, Vector2& wrapMouseDelta, Vector2& wrapMousePosition, const Vector2& windowSize)
{
    float wrapPositionY = wrapMousePosition.y + delta.y;
    wrapMouseBorderLock(delta, wrapMouseDelta, wrapMousePosition, windowSize);
    wrapMouseDelta.y = 0.0;
    wrapMousePosition.y = wrapPositionY;
    float halfHeight = (windowSize.y * 0.5);

    float wrapPositionX = wrapMousePosition.x;
    float halfWidth = (windowSize.x * 0.5);
    wrapMousePosition.x = G3D::clamp(wrapPositionX, -halfWidth, halfWidth);

    // This is changed so we can pop out into the chat area below
    wrapMousePosition.y = G3D::clamp(wrapMousePosition.y, -halfHeight, halfHeight);
    // wrapMousePosition.y = std::max(-halfHeight, wrapMousePosition.y);

    // Setting this to zero prevents the camera from panning automatically near the extents of the screen.
    wrapMouseDelta = Vector2::zero();
}

void UserInputUtil::wrapMouseHorizontalTransition(
    const Vector2& delta, Vector2& wrapMouseDelta, Vector2& wrapMousePosition, const Vector2& windowSize)
{
    float wrapPositionY = wrapMousePosition.y + delta.y;
    wrapMouseBorderTransition(delta, wrapMouseDelta, wrapMousePosition, windowSize);
    wrapMouseDelta.y = 0.0;
    wrapMousePosition.y = wrapPositionY;
}


void UserInputUtil::wrapMouseBorder(const Vector2& delta, Vector2& wrapMouseDelta, Vector2& wrapMousePosition, const Vector2& windowSize,
    const int borderWidth, const float creepFactor)

{
    Vector2 halfSize = windowSize * 0.5;
    Rect inner = Rect(-halfSize, halfSize).inset(borderWidth); // in Wrap Coordinates
    Vector2 oldPosition = wrapMousePosition;
    inner.unionWith(oldPosition); // now union of the border and old position - ratchet

    Vector2 newPositionUnclamped = oldPosition + delta;
    Vector2 newPositionClamped = inner.clamp(newPositionUnclamped);

    Vector2 positiveDistanceInBorder = newPositionUnclamped - newPositionClamped;


    if (!Aya::GameBasicSettings::singleton().inMousepanMode())
        wrapMousePosition = newPositionClamped + (positiveDistanceInBorder * creepFactor);
    wrapMouseDelta += positiveDistanceInBorder;
}

void UserInputUtil::wrapMouseBorderLock(const Vector2& delta, Vector2& wrapMouseDelta, Vector2& wrapMousePosition, const Vector2& windowSize)
{
    wrapMouseBorder(delta, wrapMouseDelta, wrapMousePosition, windowSize, 6, 0.0f);
}

void UserInputUtil::wrapMouseBorderTransition(const Vector2& delta, Vector2& wrapMouseDelta, Vector2& wrapMousePosition, const Vector2& windowSize)
{
    wrapMouseBorder(delta, wrapMouseDelta, wrapMousePosition, windowSize, 20, 0.05f);
}

void UserInputUtil::wrapMouseNone(const Vector2& delta, Vector2& wrapMouseDelta, Vector2& wrapMousePosition)
{
    wrapMouseDelta = Vector2::zero();
    wrapMousePosition += delta;
}

void UserInputUtil::wrapMouseCenter(const Vector2& delta, Vector2& wrapMouseDelta, Vector2& wrapMousePosition)
{
    wrapMouseDelta += delta;
    // don't move the cursor....
    // wrapMousePosition = G3D::Vector2::zero();
}

void UserInputUtil::wrapMousePos(
    const Vector2& delta, Vector2& wrapMouseDelta, Vector2& wrapMousePosition, const Vector2& windowSize, Vector2& posToWrapTo, bool autoMoveMouse)
{
    if (posToWrapTo.length() > 2)
    {
        Vector2 windowDelta = posToWrapTo / windowSize;
        wrapMouseDelta = windowDelta * HybridSensitivity;
        posToWrapTo -= (wrapMouseDelta * HybridSensitivity * 0.266f); // 0.266 is a tuning constant

        float xDiff = std::abs(wrapMousePosition.x / wrapMousePosition.length()) * 0.3f;
        float yDiff = std::abs(wrapMousePosition.y / wrapMousePosition.length()) * 0.4f;

        if (autoMoveMouse)
        {
            if (wrapMousePosition.x < 0)
            {
                wrapMousePosition.x += xDiff * wrapMouseDelta.length() * MouseTug;
                if (wrapMousePosition.x > 0)
                    wrapMousePosition.x = 0;
            }
            else if (wrapMousePosition.x > 0)
            {
                wrapMousePosition.x -= xDiff * wrapMouseDelta.length() * MouseTug;
                if (wrapMousePosition.x < 0)
                    wrapMousePosition.x = 0;
            }


            if (wrapMousePosition.y < 0)
            {
                wrapMousePosition.y += yDiff * wrapMouseDelta.length() * MouseTug;
                if (wrapMousePosition.y > 0)
                    wrapMousePosition.y = 0;
            }
            else if (wrapMousePosition.y > 0)
            {
                wrapMousePosition.y -= yDiff * wrapMouseDelta.length() * MouseTug;
                if (wrapMousePosition.y < 0)
                    wrapMousePosition.y = 0;
            }
        }
    }

    wrapMousePosition += delta;
}


void UserInputUtil::wrapMouseHorizontalCenter(const Vector2& delta, Vector2& wrapMouseDelta, Vector2& wrapMousePosition)
{
    wrapMouseDelta.x += delta.x;
    //	wrapMousePosition = G3D::Vector2::zero();
}



G3D::Vector2 UserInputUtil::didodToVector2(const DIDEVICEOBJECTDATA& didod)
{
    G3D::Vector2 answer(0, 0);
    float data = (float)((int)didod.dwData);

    if (didod.dwOfs == DIMOFS_X)
    {
        answer.x = data;
    }
    else
    {
        answer.y = data;
    }
    return answer;
}


// Maps DIK_* to Aya::SDLK_*
Aya::KeyCode UserInputUtil::directInputToKeyCode(DWORD diKey)
{
    RBXASSERT(diKey >= 0);
    RBXASSERT(diKey < 256);

    static Aya::KeyCode keymap[256];
    static bool initialized = false;
    if (!initialized)
    {
        for (int i = 0; i < 256; ++i)
            keymap[i] = Aya::SDLK_UNKNOWN;

        keymap[DIK_ESCAPE] = Aya::SDLK_ESCAPE;
        keymap[DIK_1] = Aya::SDLK_1;
        keymap[DIK_2] = Aya::SDLK_2;
        keymap[DIK_3] = Aya::SDLK_3;
        keymap[DIK_4] = Aya::SDLK_4;
        keymap[DIK_5] = Aya::SDLK_5;
        keymap[DIK_6] = Aya::SDLK_6;
        keymap[DIK_7] = Aya::SDLK_7;
        keymap[DIK_8] = Aya::SDLK_8;
        keymap[DIK_9] = Aya::SDLK_9;
        keymap[DIK_0] = Aya::SDLK_0;
        keymap[DIK_MINUS] = Aya::SDLK_MINUS;
        keymap[DIK_EQUALS] = Aya::SDLK_EQUALS;
        keymap[DIK_BACK] = Aya::SDLK_BACKSPACE;
        keymap[DIK_TAB] = Aya::SDLK_TAB;
        keymap[DIK_Q] = Aya::SDLK_q;
        keymap[DIK_W] = Aya::SDLK_w;
        keymap[DIK_E] = Aya::SDLK_e;
        keymap[DIK_R] = Aya::SDLK_r;
        keymap[DIK_T] = Aya::SDLK_t;
        keymap[DIK_Y] = Aya::SDLK_y;
        keymap[DIK_U] = Aya::SDLK_u;
        keymap[DIK_I] = Aya::SDLK_i;
        keymap[DIK_O] = Aya::SDLK_o;
        keymap[DIK_P] = Aya::SDLK_p;

        keymap[DIK_LBRACKET] = Aya::SDLK_LEFTBRACKET;
        keymap[DIK_AT] = Aya::SDLK_AT;
        keymap[DIK_RBRACKET] = Aya::SDLK_RIGHTBRACKET;
        keymap[DIK_PREVTRACK] = Aya::SDLK_EQUALS;
        keymap[DIK_COLON] = Aya::SDLK_COLON;
        keymap[DIK_KANJI] = Aya::SDLK_BACKQUOTE; // weird key mapping....

        keymap[DIK_RETURN] = Aya::SDLK_RETURN;
        keymap[DIK_LCONTROL] = Aya::SDLK_LCTRL;
        keymap[DIK_A] = Aya::SDLK_a;
        keymap[DIK_S] = Aya::SDLK_s;
        keymap[DIK_D] = Aya::SDLK_d;
        keymap[DIK_F] = Aya::SDLK_f;
        keymap[DIK_G] = Aya::SDLK_g;
        keymap[DIK_H] = Aya::SDLK_h;
        keymap[DIK_J] = Aya::SDLK_j;
        keymap[DIK_K] = Aya::SDLK_k;
        keymap[DIK_L] = Aya::SDLK_l;
        keymap[DIK_SEMICOLON] = Aya::SDLK_SEMICOLON;
        keymap[DIK_APOSTROPHE] = Aya::SDLK_QUOTE;
        keymap[DIK_GRAVE] = Aya::SDLK_BACKQUOTE;
        keymap[DIK_LSHIFT] = Aya::SDLK_LSHIFT;
        keymap[DIK_BACKSLASH] = Aya::SDLK_BACKSLASH;
        keymap[DIK_OEM_102] = Aya::SDLK_BACKSLASH;
        keymap[DIK_Z] = Aya::SDLK_z;
        keymap[DIK_X] = Aya::SDLK_x;
        keymap[DIK_C] = Aya::SDLK_c;
        keymap[DIK_V] = Aya::SDLK_v;
        keymap[DIK_B] = Aya::SDLK_b;
        keymap[DIK_N] = Aya::SDLK_n;
        keymap[DIK_M] = Aya::SDLK_m;
        keymap[DIK_COMMA] = Aya::SDLK_COMMA;
        keymap[DIK_PERIOD] = Aya::SDLK_PERIOD;
        keymap[DIK_SLASH] = Aya::SDLK_SLASH;
        keymap[DIK_RSHIFT] = Aya::SDLK_RSHIFT;
        keymap[DIK_MULTIPLY] = Aya::SDLK_KP_MULTIPLY;
        keymap[DIK_LMENU] = Aya::SDLK_LALT;
        keymap[DIK_SPACE] = Aya::SDLK_SPACE;
        keymap[DIK_CAPITAL] = Aya::SDLK_CAPSLOCK;
        keymap[DIK_F1] = Aya::SDLK_F1;
        keymap[DIK_F2] = Aya::SDLK_F2;
        keymap[DIK_F3] = Aya::SDLK_F3;
        keymap[DIK_F4] = Aya::SDLK_F4;
        keymap[DIK_F5] = Aya::SDLK_F5;
        keymap[DIK_F6] = Aya::SDLK_F6;
        keymap[DIK_F7] = Aya::SDLK_F7;
        keymap[DIK_F8] = Aya::SDLK_F8;
        keymap[DIK_F9] = Aya::SDLK_F9;
        keymap[DIK_F10] = Aya::SDLK_F10;
        keymap[DIK_NUMLOCK] = Aya::SDLK_NUMLOCK;
        keymap[DIK_SCROLL] = Aya::SDLK_SCROLLOCK;
        keymap[DIK_NUMPAD7] = Aya::SDLK_KP7;
        keymap[DIK_NUMPAD8] = Aya::SDLK_KP8;
        keymap[DIK_NUMPAD9] = Aya::SDLK_KP9;
        keymap[DIK_SUBTRACT] = Aya::SDLK_KP_MINUS;
        keymap[DIK_NUMPAD4] = Aya::SDLK_KP4;
        keymap[DIK_NUMPAD5] = Aya::SDLK_KP5;
        keymap[DIK_NUMPAD6] = Aya::SDLK_KP6;
        keymap[DIK_ADD] = Aya::SDLK_KP_PLUS;
        keymap[DIK_NUMPAD1] = Aya::SDLK_KP1;
        keymap[DIK_NUMPAD2] = Aya::SDLK_KP2;
        keymap[DIK_NUMPAD3] = Aya::SDLK_KP3;
        keymap[DIK_NUMPAD0] = Aya::SDLK_KP0;
        keymap[DIK_DECIMAL] = Aya::SDLK_KP_PERIOD;
        keymap[DIK_F11] = Aya::SDLK_F11;
        keymap[DIK_F12] = Aya::SDLK_F12;
        keymap[DIK_F13] = Aya::SDLK_F13;
        keymap[DIK_F14] = Aya::SDLK_F14;
        keymap[DIK_F15] = Aya::SDLK_F15;
        keymap[DIK_NUMPADEQUALS] = Aya::SDLK_KP_EQUALS;
        keymap[DIK_NUMPADENTER] = Aya::SDLK_KP_ENTER;
        keymap[DIK_RCONTROL] = Aya::SDLK_RCTRL;
        keymap[DIK_DIVIDE] = Aya::SDLK_KP_DIVIDE;
        keymap[DIK_SYSRQ] = Aya::SDLK_SYSREQ;
        keymap[DIK_RMENU] = Aya::SDLK_RALT;
        keymap[DIK_PAUSE] = Aya::SDLK_PAUSE;
        keymap[DIK_HOME] = Aya::SDLK_HOME;
        keymap[DIK_UP] = Aya::SDLK_UP;
        keymap[DIK_PRIOR] = Aya::SDLK_PAGEUP;
        keymap[DIK_LEFT] = Aya::SDLK_LEFT;
        keymap[DIK_RIGHT] = Aya::SDLK_RIGHT;
        keymap[DIK_END] = Aya::SDLK_END;
        keymap[DIK_DOWN] = Aya::SDLK_DOWN;
        keymap[DIK_NEXT] = Aya::SDLK_PAGEDOWN;
        keymap[DIK_INSERT] = Aya::SDLK_INSERT;
        keymap[DIK_DELETE] = Aya::SDLK_DELETE;
        keymap[DIK_LWIN] = Aya::SDLK_LMETA;
        keymap[DIK_RWIN] = Aya::SDLK_RMETA;
        keymap[DIK_APPS] = Aya::SDLK_MENU;
        initialized = true;
    }

    return keymap[diKey];
}

// Maps Aya::Aya::SDLK_* to DIK_*
DWORD UserInputUtil::keyCodeToDirectInput(Aya::KeyCode keyCode)
{
    static DWORD keymap[Aya::SDLK_LAST];
    static bool initialized = false;
    if (!initialized)
    {
        for (int i = 0; i < Aya::SDLK_LAST; ++i)
            keymap[i] = 0;

        keymap[Aya::SDLK_ESCAPE] = DIK_ESCAPE;
        keymap[Aya::SDLK_1] = DIK_1;
        keymap[Aya::SDLK_2] = DIK_2;
        keymap[Aya::SDLK_3] = DIK_3;
        keymap[Aya::SDLK_4] = DIK_4;
        keymap[Aya::SDLK_5] = DIK_5;
        keymap[Aya::SDLK_6] = DIK_6;
        keymap[Aya::SDLK_7] = DIK_7;
        keymap[Aya::SDLK_8] = DIK_8;
        keymap[Aya::SDLK_9] = DIK_9;
        keymap[Aya::SDLK_0] = DIK_0;
        keymap[Aya::SDLK_MINUS] = DIK_MINUS;
        keymap[Aya::SDLK_EQUALS] = DIK_EQUALS;
        keymap[Aya::SDLK_BACKSPACE] = DIK_BACK;
        keymap[Aya::SDLK_TAB] = DIK_TAB;
        keymap[Aya::SDLK_q] = DIK_Q;
        keymap[Aya::SDLK_w] = DIK_W;
        keymap[Aya::SDLK_e] = DIK_E;
        keymap[Aya::SDLK_r] = DIK_R;
        keymap[Aya::SDLK_t] = DIK_T;
        keymap[Aya::SDLK_y] = DIK_Y;
        keymap[Aya::SDLK_u] = DIK_U;
        keymap[Aya::SDLK_i] = DIK_I;
        keymap[Aya::SDLK_o] = DIK_O;
        keymap[Aya::SDLK_p] = DIK_P;
        keymap[Aya::SDLK_LEFTBRACKET] = DIK_LBRACKET;
        keymap[Aya::SDLK_RIGHTBRACKET] = DIK_RBRACKET;
        keymap[Aya::SDLK_RETURN] = DIK_RETURN;
        keymap[Aya::SDLK_LCTRL] = DIK_LCONTROL;
        keymap[Aya::SDLK_a] = DIK_A;
        keymap[Aya::SDLK_s] = DIK_S;
        keymap[Aya::SDLK_d] = DIK_D;
        keymap[Aya::SDLK_f] = DIK_F;
        keymap[Aya::SDLK_g] = DIK_G;
        keymap[Aya::SDLK_h] = DIK_H;
        keymap[Aya::SDLK_j] = DIK_J;
        keymap[Aya::SDLK_k] = DIK_K;
        keymap[Aya::SDLK_l] = DIK_L;
        keymap[Aya::SDLK_SEMICOLON] = DIK_SEMICOLON;
        keymap[Aya::SDLK_QUOTE] = DIK_APOSTROPHE;
        keymap[Aya::SDLK_BACKQUOTE] = DIK_GRAVE;
        keymap[Aya::SDLK_LSHIFT] = DIK_LSHIFT;
        keymap[Aya::SDLK_BACKSLASH] = DIK_BACKSLASH;
        keymap[Aya::SDLK_BACKSLASH] = DIK_OEM_102;
        keymap[Aya::SDLK_z] = DIK_Z;
        keymap[Aya::SDLK_x] = DIK_X;
        keymap[Aya::SDLK_c] = DIK_C;
        keymap[Aya::SDLK_v] = DIK_V;
        keymap[Aya::SDLK_b] = DIK_B;
        keymap[Aya::SDLK_n] = DIK_N;
        keymap[Aya::SDLK_m] = DIK_M;
        keymap[Aya::SDLK_COMMA] = DIK_COMMA;
        keymap[Aya::SDLK_PERIOD] = DIK_PERIOD;
        keymap[Aya::SDLK_SLASH] = DIK_SLASH;
        keymap[Aya::SDLK_RSHIFT] = DIK_RSHIFT;
        keymap[Aya::SDLK_KP_MULTIPLY] = DIK_MULTIPLY;
        keymap[Aya::SDLK_LALT] = DIK_LMENU;
        keymap[Aya::SDLK_SPACE] = DIK_SPACE;
        keymap[Aya::SDLK_CAPSLOCK] = DIK_CAPITAL;
        keymap[Aya::SDLK_F1] = DIK_F1;
        keymap[Aya::SDLK_F2] = DIK_F2;
        keymap[Aya::SDLK_F3] = DIK_F3;
        keymap[Aya::SDLK_F4] = DIK_F4;
        keymap[Aya::SDLK_F5] = DIK_F5;
        keymap[Aya::SDLK_F6] = DIK_F6;
        keymap[Aya::SDLK_F7] = DIK_F7;
        keymap[Aya::SDLK_F8] = DIK_F8;
        keymap[Aya::SDLK_F9] = DIK_F9;
        keymap[Aya::SDLK_F10] = DIK_F10;
        keymap[Aya::SDLK_NUMLOCK] = DIK_NUMLOCK;
        keymap[Aya::SDLK_SCROLLOCK] = DIK_SCROLL;
        keymap[Aya::SDLK_KP7] = DIK_NUMPAD7;
        keymap[Aya::SDLK_KP8] = DIK_NUMPAD8;
        keymap[Aya::SDLK_KP9] = DIK_NUMPAD9;
        keymap[Aya::SDLK_KP_MINUS] = DIK_SUBTRACT;
        keymap[Aya::SDLK_KP4] = DIK_NUMPAD4;
        keymap[Aya::SDLK_KP5] = DIK_NUMPAD5;
        keymap[Aya::SDLK_KP6] = DIK_NUMPAD6;
        keymap[Aya::SDLK_KP_PLUS] = DIK_ADD;
        keymap[Aya::SDLK_KP1] = DIK_NUMPAD1;
        keymap[Aya::SDLK_KP2] = DIK_NUMPAD2;
        keymap[Aya::SDLK_KP3] = DIK_NUMPAD3;
        keymap[Aya::SDLK_KP0] = DIK_NUMPAD0;
        keymap[Aya::SDLK_KP_PERIOD] = DIK_DECIMAL;
        keymap[Aya::SDLK_F11] = DIK_F11;
        keymap[Aya::SDLK_F12] = DIK_F12;
        keymap[Aya::SDLK_F13] = DIK_F13;
        keymap[Aya::SDLK_F14] = DIK_F14;
        keymap[Aya::SDLK_F15] = DIK_F15;
        keymap[Aya::SDLK_KP_EQUALS] = DIK_NUMPADEQUALS;
        keymap[Aya::SDLK_KP_ENTER] = DIK_NUMPADENTER;
        keymap[Aya::SDLK_RCTRL] = DIK_RCONTROL;
        keymap[Aya::SDLK_KP_DIVIDE] = DIK_DIVIDE;
        keymap[Aya::SDLK_SYSREQ] = DIK_SYSRQ;
        keymap[Aya::SDLK_RALT] = DIK_RMENU;
        keymap[Aya::SDLK_PAUSE] = DIK_PAUSE;
        keymap[Aya::SDLK_HOME] = DIK_HOME;
        keymap[Aya::SDLK_UP] = DIK_UP;
        keymap[Aya::SDLK_PAGEUP] = DIK_PRIOR;
        keymap[Aya::SDLK_LEFT] = DIK_LEFT;
        keymap[Aya::SDLK_RIGHT] = DIK_RIGHT;
        keymap[Aya::SDLK_END] = DIK_END;
        keymap[Aya::SDLK_DOWN] = DIK_DOWN;
        keymap[Aya::SDLK_PAGEDOWN] = DIK_NEXT;
        keymap[Aya::SDLK_INSERT] = DIK_INSERT;
        keymap[Aya::SDLK_DELETE] = DIK_DELETE;
        keymap[Aya::SDLK_LMETA] = DIK_LWIN;
        keymap[Aya::SDLK_RMETA] = DIK_RWIN;
        keymap[Aya::SDLK_MENU] = DIK_APPS;
        initialized = true;
    }

    return keymap[keyCode];
}


// Maps Aya::Aya::SDLK_* to VK_*
DWORD UserInputUtil::keyCodeToVK(Aya::KeyCode keyCode)
{
    static DWORD keymap[Aya::SDLK_LAST];
    static bool initialized = false;
    if (!initialized)
    {
        for (int i = 0; i < Aya::SDLK_LAST; ++i)
            keymap[i] = 0;

        keymap[Aya::SDLK_PRINT] = VK_PRINT;
        keymap[Aya::SDLK_SYSREQ] = VK_SNAPSHOT;
        keymap[Aya::SDLK_ESCAPE] = VK_ESCAPE;
        keymap[Aya::SDLK_BACKSPACE] = VK_BACK;
        keymap[Aya::SDLK_TAB] = VK_TAB;
        keymap[Aya::SDLK_RETURN] = VK_RETURN;
        keymap[Aya::SDLK_LCTRL] = VK_LCONTROL;
        keymap[Aya::SDLK_LSHIFT] = VK_LSHIFT;
        keymap[Aya::SDLK_BACKSLASH] = VK_OEM_102;
        keymap[Aya::SDLK_RSHIFT] = VK_RSHIFT;
        keymap[Aya::SDLK_KP_MULTIPLY] = VK_MULTIPLY;
        keymap[Aya::SDLK_LALT] = VK_LMENU;
        keymap[Aya::SDLK_SPACE] = VK_SPACE;
        keymap[Aya::SDLK_CAPSLOCK] = VK_CAPITAL;
        keymap[Aya::SDLK_F1] = VK_F1;
        keymap[Aya::SDLK_F2] = VK_F2;
        keymap[Aya::SDLK_F3] = VK_F3;
        keymap[Aya::SDLK_F4] = VK_F4;
        keymap[Aya::SDLK_F5] = VK_F5;
        keymap[Aya::SDLK_F6] = VK_F6;
        keymap[Aya::SDLK_F7] = VK_F7;
        keymap[Aya::SDLK_F8] = VK_F8;
        keymap[Aya::SDLK_F9] = VK_F9;
        keymap[Aya::SDLK_F10] = VK_F10;
        keymap[Aya::SDLK_NUMLOCK] = VK_NUMLOCK;
        keymap[Aya::SDLK_SCROLLOCK] = VK_SCROLL;
        keymap[Aya::SDLK_KP7] = VK_NUMPAD7;
        keymap[Aya::SDLK_KP8] = VK_NUMPAD8;
        keymap[Aya::SDLK_KP9] = VK_NUMPAD9;
        keymap[Aya::SDLK_KP_MINUS] = VK_SUBTRACT;
        keymap[Aya::SDLK_KP4] = VK_NUMPAD4;
        keymap[Aya::SDLK_KP5] = VK_NUMPAD5;
        keymap[Aya::SDLK_KP6] = VK_NUMPAD6;
        keymap[Aya::SDLK_KP_PLUS] = VK_ADD;
        keymap[Aya::SDLK_KP1] = VK_NUMPAD1;
        keymap[Aya::SDLK_KP2] = VK_NUMPAD2;
        keymap[Aya::SDLK_KP3] = VK_NUMPAD3;
        keymap[Aya::SDLK_KP0] = VK_NUMPAD0;
        keymap[Aya::SDLK_KP_PERIOD] = VK_DECIMAL;
        keymap[Aya::SDLK_F11] = VK_F11;
        keymap[Aya::SDLK_F12] = VK_F12;
        keymap[Aya::SDLK_F13] = VK_F13;
        keymap[Aya::SDLK_F14] = VK_F14;
        keymap[Aya::SDLK_F15] = VK_F15;
        keymap[Aya::SDLK_KP_ENTER] = VK_RETURN;
        keymap[Aya::SDLK_RCTRL] = VK_RCONTROL;
        keymap[Aya::SDLK_KP_DIVIDE] = VK_DIVIDE;
        keymap[Aya::SDLK_RALT] = VK_RMENU;
        keymap[Aya::SDLK_HOME] = VK_HOME;
        keymap[Aya::SDLK_UP] = VK_UP;
        keymap[Aya::SDLK_PAGEUP] = VK_PRIOR;
        keymap[Aya::SDLK_LEFT] = VK_LEFT;
        keymap[Aya::SDLK_RIGHT] = VK_RIGHT;
        keymap[Aya::SDLK_END] = VK_END;
        keymap[Aya::SDLK_DOWN] = VK_DOWN;
        keymap[Aya::SDLK_PAGEDOWN] = VK_NEXT;
        keymap[Aya::SDLK_INSERT] = VK_INSERT;
        keymap[Aya::SDLK_DELETE] = VK_DELETE;
        keymap[Aya::SDLK_LMETA] = VK_LWIN;
        keymap[Aya::SDLK_RMETA] = VK_RWIN;
        keymap[Aya::SDLK_MENU] = VK_APPS;
        initialized = true;
    }

    return keymap[keyCode];
}

Aya::InputObject::UserInputState UserInputUtil::msgToEventState(UINT uMsg)
{
    switch (uMsg)
    {
    case WM_MOUSEMOVE:
        return Aya::InputObject::INPUT_STATE_CHANGE;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN: // intentional fall thru
        return Aya::InputObject::INPUT_STATE_BEGIN;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP: // intentional fall thru
        return Aya::InputObject::INPUT_STATE_END;
    default:
        return Aya::InputObject::INPUT_STATE_NONE;
    }
}
Aya::InputObject::UserInputType UserInputUtil::msgToEventType(UINT uMsg)
{
    switch (uMsg)
    {
    case WM_MOUSEMOVE:
        return Aya::InputObject::TYPE_MOUSEMOVEMENT;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP: // intentional fall thru
        return Aya::InputObject::TYPE_MOUSEBUTTON1;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP: // intentional fall thru
        return Aya::InputObject::TYPE_MOUSEBUTTON2;
    default:
        return Aya::InputObject::TYPE_NONE;
    }
}


Aya::ModCode UserInputUtil::createModCode(const DiKeys& diKeys)
{
    unsigned int modCode = 0;

    if (diKeys[DIK_LSHIFT] & 0x80)
    {
        modCode = modCode | Aya::KMOD_LSHIFT;
    }
    if (diKeys[DIK_RSHIFT] & 0x80)
    {
        modCode = modCode | Aya::KMOD_RSHIFT;
    }
    if (diKeys[DIK_LCONTROL] & 0x80)
    {
        modCode = modCode | Aya::KMOD_LCTRL;
    }
    if (diKeys[DIK_RCONTROL] & 0x80)
    {
        modCode = modCode | Aya::KMOD_RCTRL;
    }
    if (diKeys[DIK_LMENU] & 0x80)
    {
        modCode = modCode | Aya::KMOD_LALT;
    }
    if (diKeys[DIK_LMENU] & 0x80)
    {
        modCode = modCode | Aya::KMOD_RALT;
    }
    /*if (diKeys[DIK_CAPSLOCK] & 0x80)
    {
            modCode = modCode | Aya::KMOD_CAPS;
    }*/
    if (::GetKeyState(VK_CAPITAL))
    {
        modCode = modCode | Aya::KMOD_CAPS;
    }

    return (Aya::ModCode)modCode;
}
