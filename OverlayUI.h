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

        HBRUSH brush = CreateSolidBrush(pressed ? state.activeBg : state.inactiveBg);
        RECT rect = { sx, sy, sx + sw, sy + sh };
        FillRect(hdc, &rect, brush);
        DeleteObject(brush);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, pressed ? state.activeText : state.inactiveText);
        
        HFONT hFont = CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                                  CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
        SelectObject(hdc, hFont);
        DrawTextW(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        DeleteObject(hFont);
    }

    static void DrawMouseUI(HDC hdc, int x, int y) {
        AppState& state = AppState::Get();
        int sx = (int)(x * state.uiScale);
        int sy = (int)(y * state.uiScale);
        
        int bw = (int)(35 * state.uiScale);
        int bh = (int)(45 * state.uiScale);
        int gap = (int)(5 * state.uiScale);
        int bodyH = (int)(55 * state.uiScale);
        int corner = (int)(15 * state.uiScale);

        HBRUSH lmbBrush = CreateSolidBrush(state.lmb ? state.activeBg : state.inactiveBg);
        HBRUSH rmbBrush = CreateSolidBrush(state.rmb ? state.activeBg : state.inactiveBg);
        HBRUSH bodyBrush = CreateSolidBrush(state.inactiveBg);
        HBRUSH wheelBrush = CreateSolidBrush(state.inactiveText); // Scroll wheel matches text color

        HPEN nullPen = CreatePen(PS_NULL, 0, RGB(0,0,0));
        HPEN oldPen = (HPEN)SelectObject(hdc, nullPen);

        SelectObject(hdc, lmbBrush);
        RoundRect(hdc, sx, sy, sx + bw, sy + bh, corner, corner);
        
        SelectObject(hdc, rmbBrush);
        RoundRect(hdc, sx + bw + gap, sy, sx + bw * 2 + gap, sy + bh, corner, corner);
        
        SelectObject(hdc, bodyBrush);
        RoundRect(hdc, sx, sy + bh + (int)(3 * state.uiScale), sx + bw * 2 + gap, sy + bh + (int)(3 * state.uiScale) + bodyH, corner * 2, corner * 2);

        SelectObject(hdc, wheelBrush);
        int wx = sx + bw - (int)(1 * state.uiScale);
        int wy = sy + (int)(15 * state.uiScale);
        int ww = gap + (int)(2 * state.uiScale);
        int wh = (int)(18 * state.uiScale);
        RoundRect(hdc, wx, wy, wx + ww, wy + wh, (int)(4 * state.uiScale), (int)(4 * state.uiScale));

        SetBkMode(hdc, TRANSPARENT);
        HFONT hFont = CreateFontW((int)(16 * state.uiScale), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, 
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                                  CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
        HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

        RECT rcLmb = { sx, sy, sx + bw, sy + bh };
        SetTextColor(hdc, state.lmb ? state.activeText : state.inactiveText);
        DrawTextW(hdc, L"L", -1, &rcLmb, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        RECT rcRmb = { sx + bw + gap, sy, sx + bw * 2 + gap, sy + bh };
        SetTextColor(hdc, state.rmb ? state.activeText : state.inactiveText);
        DrawTextW(hdc, L"R", -1, &rcRmb, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, oldFont);
        SelectObject(hdc, oldPen);
        DeleteObject(nullPen);
        DeleteObject(hFont);
        DeleteObject(lmbBrush);
        DeleteObject(rmbBrush);
        DeleteObject(bodyBrush);
        DeleteObject(wheelBrush);
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
            
            DrawMouseUI(memDC, 170, 10);

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