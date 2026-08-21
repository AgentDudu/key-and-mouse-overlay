#pragma once
#include <windows.h>
#include "AppState.h"

class InputManager {
private:
    static InputManager& Get() {
        static InputManager instance;
        return instance;
    }

    HHOOK hKeyboardHook = NULL;
    HHOOK hMouseHook = NULL;

    static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION) {
            KBDLLHOOKSTRUCT* pKeyBoard = (KBDLLHOOKSTRUCT*)lParam;
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                if (pKeyBoard->vkCode < 256) AppState::Get().keys[pKeyBoard->vkCode] = true;
                if (AppState::Get().hwndOverlay) InvalidateRect(AppState::Get().hwndOverlay, NULL, FALSE);
            }
            else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
                if (pKeyBoard->vkCode < 256) AppState::Get().keys[pKeyBoard->vkCode] = false;
                if (AppState::Get().hwndOverlay) InvalidateRect(AppState::Get().hwndOverlay, NULL, FALSE);
            }
        }
        return CallNextHookEx(Get().hKeyboardHook, nCode, wParam, lParam);
    }

    static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION) {
            MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;

            if (wParam == WM_LBUTTONDOWN) { AppState::Get().lmb = true; InvalidateRect(AppState::Get().hwndOverlay, NULL, FALSE); }
            else if (wParam == WM_LBUTTONUP) { AppState::Get().lmb = false; InvalidateRect(AppState::Get().hwndOverlay, NULL, FALSE); }
            else if (wParam == WM_RBUTTONDOWN) { AppState::Get().rmb = true; InvalidateRect(AppState::Get().hwndOverlay, NULL, FALSE); }
            else if (wParam == WM_RBUTTONUP) { AppState::Get().rmb = false; InvalidateRect(AppState::Get().hwndOverlay, NULL, FALSE); }
            else if (wParam == WM_MBUTTONDOWN) { AppState::Get().mmb = true; InvalidateRect(AppState::Get().hwndOverlay, NULL, FALSE); }
            else if (wParam == WM_MBUTTONUP) { AppState::Get().mmb = false; InvalidateRect(AppState::Get().hwndOverlay, NULL, FALSE); }
            else if (wParam == WM_MOUSEWHEEL) { 
                AppState::Get().isScrolling = true; 
                SetTimer(AppState::Get().hwndOverlay, 1, 150, NULL); 
                InvalidateRect(AppState::Get().hwndOverlay, NULL, FALSE); 
            }
            else if (wParam == WM_XBUTTONDOWN || wParam == WM_XBUTTONUP) {
                bool isDown = (wParam == WM_XBUTTONDOWN);
                if (HIWORD(pMouse->mouseData) == XBUTTON1) {
                    AppState::Get().mb4 = isDown;
                } else if (HIWORD(pMouse->mouseData) == XBUTTON2) {
                    AppState::Get().mb5 = isDown;
                }
                InvalidateRect(AppState::Get().hwndOverlay, NULL, FALSE);
            }
        }
        return CallNextHookEx(Get().hMouseHook, nCode, wParam, lParam);
    }

public:
    static void Install(HINSTANCE hInstance) {
        Get().hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInstance, 0);
        Get().hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, hInstance, 0);
    }

    static void Remove() {
        UnhookWindowsHookEx(Get().hKeyboardHook);
        UnhookWindowsHookEx(Get().hMouseHook);
    }
};