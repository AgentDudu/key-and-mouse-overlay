#pragma once
#include <windows.h>
#include "AppState.h"
#include "resource.h"

class OverlayUI {
private:
    static void DrawButton(HDC hdc, int x, int y, int w, int h, const wchar_t* text, bool pressed) {
        AppState& state = AppState::Get();
        int sx = (int)(x * state.uiScale);
        int sy = (int)(y * state.uiScale);
        int sw = (int)(w * state.uiScale);
        int sh = (int)(h * state.uiScale);
        int fontSize = (int)(18 * state.uiScale);

        HBRUSH brush = CreateSolidBrush(pressed ? RGB(0, 255, 204) : RGB(34, 34, 34));
        RECT rect = { sx, sy, sx + sw, sy + sh };
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, pressed ? RGB(0, 0, 0) : RGB(255, 255, 255));
        
        HFONT hFont = CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                                  CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
        SelectObject(hdc, hFont);
        DrawTextW(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        DeleteObject(hFont);
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        AppState& state = AppState::Get();

        if (uMsg == WM_PAINT) {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            SelectObject(memDC, memBitmap);

            HBRUSH bgBrush = CreateSolidBrush(state.isLocked ? RGB(0, 0, 0) : RGB(20, 20, 20));
            FillRect(memDC, &rect, bgBrush);
            DeleteObject(bgBrush);

            DrawButton(memDC, 65, 10, 45, 45, L"W", state.keys['W']);
            DrawButton(memDC, 15, 60, 45, 45, L"A", state.keys['A']);
            DrawButton(memDC, 65, 60, 45, 45, L"S", state.keys['S']);
            DrawButton(memDC, 115, 60, 45, 45, L"D", state.keys['D']);
            DrawButton(memDC, 15, 110, 145, 25, L"SPACE", state.keys[VK_SPACE]);
            DrawButton(memDC, 15, 140, 70, 25, L"SHIFT", state.keys[VK_LSHIFT] || state.keys[VK_SHIFT]);
            DrawButton(memDC, 90, 140, 70, 25, L"CTRL", state.keys[VK_LCONTROL] || state.keys[VK_CONTROL]);
            DrawButton(memDC, 170, 10, 35, 70, L"LMB", state.lmb);
            DrawButton(memDC, 210, 10, 35, 70, L"RMB", state.rmb);

            BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

            DeleteObject(memBitmap);
            DeleteDC(memDC);
            EndPaint(hwnd, &ps);
            return 0;
        }
        else if (uMsg == WM_NCHITTEST && !state.isLocked) {
            return HTCAPTION;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

public:
    static HWND Create(HINSTANCE hInstance) {
        WNDCLASSW wc = { };
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"OverlayClass";
        
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
        
        RegisterClassW(&wc);

        HWND hwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
            L"OverlayClass", L"Overlay",
            WS_POPUP, 400, 400, AppState::Get().BASE_WIDTH, AppState::Get().BASE_HEIGHT,
            NULL, NULL, hInstance, NULL
        );
        SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
        return hwnd;
    }

    static void UpdateSize() {
        AppState& state = AppState::Get();
        SetWindowPos(state.hwndOverlay, NULL, 0, 0, 
            (int)(state.BASE_WIDTH * state.uiScale), 
            (int)(state.BASE_HEIGHT * state.uiScale), 
            SWP_NOMOVE | SWP_NOZORDER);
        InvalidateRect(state.hwndOverlay, NULL, FALSE);
    }
};