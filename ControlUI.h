#pragma once
#include <windows.h>
#include "AppState.h"
#include "OverlayUI.h"
#include "resource.h"

class ControlUI {
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        AppState& state = AppState::Get();

        if (uMsg == WM_CREATE) {
            CreateWindowW(L"BUTTON", L"Unlock / Move Overlay", WS_CHILD | WS_VISIBLE, 20, 20, 200, 30, hwnd, (HMENU)1, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Size +", WS_CHILD | WS_VISIBLE, 20, 60, 95, 30, hwnd, (HMENU)2, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Size -", WS_CHILD | WS_VISIBLE, 125, 60, 95, 30, hwnd, (HMENU)3, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Close App", WS_CHILD | WS_VISIBLE, 20, 110, 200, 30, hwnd, (HMENU)4, NULL, NULL);
            return 0;
        }
        else if (uMsg == WM_COMMAND) {
            int wmId = LOWORD(wParam);
            if (wmId == 1) {
                state.isLocked = !state.isLocked;
                LONG exStyle = GetWindowLong(state.hwndOverlay, GWL_EXSTYLE);
                if (state.isLocked) {
                    SetWindowLong(state.hwndOverlay, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
                    SetLayeredWindowAttributes(state.hwndOverlay, RGB(0, 0, 0), 0, LWA_COLORKEY);
                } else {
                    SetWindowLong(state.hwndOverlay, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
                    SetLayeredWindowAttributes(state.hwndOverlay, RGB(0, 0, 0), 200, LWA_ALPHA);
                }
                InvalidateRect(state.hwndOverlay, NULL, FALSE);
            }
            else if (wmId == 2) {
                if (state.uiScale < 3.0f) state.uiScale += 0.1f;
                OverlayUI::UpdateSize();
            }
            else if (wmId == 3) {
                if (state.uiScale > 0.5f) state.uiScale -= 0.1f;
                OverlayUI::UpdateSize();
            }
            else if (wmId == 4) {
                PostQuitMessage(0);
            }
            return 0;
        }
        else if (uMsg == WM_DESTROY) {
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

public:
    static HWND Create(HINSTANCE hInstance) {
        WNDCLASSW wc = { };
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"ControlClass";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON)); 
        
        RegisterClassW(&wc);

        return CreateWindowExW(
            0, L"ControlClass", L"Overlay",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
            100, 100, 260, 200,
            NULL, NULL, hInstance, NULL
        );
    }
};