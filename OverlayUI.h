#pragma once
#include <windows.h>
#include "AppState.h"
#include "resource.h" 

class OverlayUI {
private:
    struct LayoutInfo {
        int colMap[15];
        int rowMap[5];
        int max_r;
        int mouseX;
        int spaceY;
        int shiftCtrlY;
        int lowestY;
        int mouseRightEdge;
    };

    static bool GetGridPos(int key, int& base_col, float& offset_x, int& base_row) {
        if (key >= '1' && key <= '9') { base_col = key - '1'; offset_x = 0.0f; base_row = 0; return true; }
        if (key == '0') { base_col = 9; offset_x = 0.0f; base_row = 0; return true; }
        const char* r1 = "QWERTYUIOP"; for (int i = 0; i < 10; i++) if (key == r1[i]) { base_col = i; offset_x = 0.0f; base_row = 1; return true; }
        const char* r2 = "ASDFGHJKL";  for (int i = 0; i < 9; i++) if (key == r2[i]) { base_col = i; offset_x = 0.0f; base_row = 2; return true; }
        const char* r3 = "ZXCVBNM";    for (int i = 0; i < 7; i++) if (key == r3[i]) { base_col = i; offset_x = 0.5f; base_row = 3; return true; }
        return false;
    }

    static LayoutInfo ComputeLayout() {
        AppState& state = AppState::Get();
        LayoutInfo li = {0};

        bool activeCols[15] = { false }, activeRows[5] = { false };
        for (int i = '0'; i <= 'Z'; i++) {
            if (!state.showExtraKey[i]) continue;
            int bc, br; float ox;
            if (GetGridPos(i, bc, ox, br)) { activeCols[bc] = true; activeRows[br] = true; }
        }

        int curr_c = 0; for (int i = 0; i < 15; i++) if (activeCols[i]) li.colMap[i] = curr_c++;
        int curr_r = 0; for (int i = 0; i < 5; i++) if (activeRows[i]) li.rowMap[i] = curr_r++;
        li.max_r = (curr_r > 0) ? curr_r - 1 : -1;
        
        float out_maxC = 0;
        for (int i = '0'; i <= 'Z'; i++) {
            if (!state.showExtraKey[i]) continue;
            int bc, br; float ox;
            if (GetGridPos(i, bc, ox, br)) {
                float c = li.colMap[bc] + ox;
                if (c > out_maxC) out_maxC = c;
            }
        }

        int max_kbd_x = 15;
        if (li.max_r > -1) max_kbd_x = (int)((out_maxC * 50) + 15 + 45);

        int curY = (li.max_r > -1) ? (li.max_r * 50) + 60 : 10;
        
        if (state.showSpace) {
            li.spaceY = curY;
            if (15 + 145 > max_kbd_x) max_kbd_x = 15 + 145; 
            curY += 30;
        } else li.spaceY = -1;

        if (state.showShift || state.showCtrl) {
            li.shiftCtrlY = curY;
            int w = 15;
            if (state.showShift) w += 70;
            if (state.showShift && state.showCtrl) w += 5;
            if (state.showCtrl) w += 70;
            
            if (w > max_kbd_x) max_kbd_x = w; 
            curY += 30;
        } else li.shiftCtrlY = -1;

        li.lowestY = curY; 

        li.mouseX = max_kbd_x + 60; 
        
        int mouseBottom = 10 + 45 + 55 + 10; 
        if (mouseBottom > li.lowestY) li.lowestY = mouseBottom;

        li.mouseRightEdge = li.mouseX + 35 * 2 + 5 + 15; 
        return li;
    }

    static void DrawButton(HDC hdc, int x, int y, int w, int h, const wchar_t* text, bool pressed) {
        AppState& state = AppState::Get();
        int sx = (int)(x * state.uiScale), sy = (int)(y * state.uiScale);
        int sw = (int)(w * state.uiScale), sh = (int)(h * state.uiScale), fontSize = (int)(18 * state.uiScale);

        HBRUSH brush = CreateSolidBrush(pressed ? state.activeBg : state.inactiveBg);
        RECT rect = { sx, sy, sx + sw, sy + sh };
        FillRect(hdc, &rect, brush); DeleteObject(brush);

        SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, pressed ? state.activeText : state.inactiveText);
        HFONT hFont = CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
        SelectObject(hdc, hFont); DrawTextW(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE); DeleteObject(hFont);
    }

    static void DrawMouseUI(HDC hdc, int x, int y) {
        AppState& state = AppState::Get();
        int sx = (int)(x * state.uiScale), sy = (int)(y * state.uiScale);
        int bw = (int)(35 * state.uiScale), bh = (int)(45 * state.uiScale), gap = (int)(5 * state.uiScale); 
        int bodyH = (int)(55 * state.uiScale), corner = (int)(15 * state.uiScale); 

        HBRUSH lmbBrush = CreateSolidBrush(state.lmb ? state.activeBg : state.inactiveBg);
        HBRUSH rmbBrush = CreateSolidBrush(state.rmb ? state.activeBg : state.inactiveBg);
        HBRUSH bodyBrush = CreateSolidBrush(state.inactiveBg);
        HBRUSH wheelBrush = CreateSolidBrush(state.mmb || state.isScrolling ? state.activeBg : state.inactiveText); 
        HBRUSH m4Brush = CreateSolidBrush(state.mb4 ? state.activeBg : state.inactiveBg);
        HBRUSH m5Brush = CreateSolidBrush(state.mb5 ? state.activeBg : state.inactiveBg);
        HPEN nullPen = CreatePen(PS_NULL, 0, RGB(0,0,0)); HPEN oldPen = (HPEN)SelectObject(hdc, nullPen);

        int sideW = (int)(8 * state.uiScale), sideH = (int)(22 * state.uiScale), sideX = sx - sideW - (int)(2 * state.uiScale);
        
        if (state.showMB5) {
            SelectObject(hdc, m5Brush); 
            RoundRect(hdc, sideX, sy + bh + (int)(4 * state.uiScale), sideX + sideW, sy + bh + (int)(4 * state.uiScale) + sideH, (int)(4 * state.uiScale), (int)(4 * state.uiScale));
        }
        if (state.showMB4) {
            SelectObject(hdc, m4Brush); 
            RoundRect(hdc, sideX, sy + bh + (int)(29 * state.uiScale), sideX + sideW, sy + bh + (int)(29 * state.uiScale) + sideH, (int)(4 * state.uiScale), (int)(4 * state.uiScale));
        }

        SelectObject(hdc, lmbBrush); RoundRect(hdc, sx, sy, sx + bw, sy + bh, corner, corner);
        SelectObject(hdc, rmbBrush); RoundRect(hdc, sx + bw + gap, sy, sx + bw * 2 + gap, sy + bh, corner, corner);
        SelectObject(hdc, bodyBrush); RoundRect(hdc, sx, sy + bh + (int)(3 * state.uiScale), sx + bw * 2 + gap, sy + bh + (int)(3 * state.uiScale) + bodyH, corner * 2, corner * 2);
        
        SelectObject(hdc, wheelBrush);
        int wx = sx + bw - (int)(1 * state.uiScale), wy = sy + (int)(15 * state.uiScale), ww = gap + (int)(2 * state.uiScale), wh = (int)(18 * state.uiScale);
        RoundRect(hdc, wx, wy, wx + ww, wy + wh, (int)(4 * state.uiScale), (int)(4 * state.uiScale));

        SetBkMode(hdc, TRANSPARENT);
        HFONT hFont = CreateFontW((int)(16 * state.uiScale), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Segoe UI");
        HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
        RECT rcLmb = { sx, sy, sx + bw, sy + bh }; SetTextColor(hdc, state.lmb ? state.activeText : state.inactiveText); DrawTextW(hdc, L"L", -1, &rcLmb, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        RECT rcRmb = { sx + bw + gap, sy, sx + bw * 2 + gap, sy + bh }; SetTextColor(hdc, state.rmb ? state.activeText : state.inactiveText); DrawTextW(hdc, L"R", -1, &rcRmb, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdc, oldFont); SelectObject(hdc, oldPen); DeleteObject(nullPen); DeleteObject(hFont);
        DeleteObject(lmbBrush); DeleteObject(rmbBrush); DeleteObject(bodyBrush); DeleteObject(wheelBrush); DeleteObject(m4Brush); DeleteObject(m5Brush);
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        AppState& state = AppState::Get();

        if (uMsg == WM_TIMER && wParam == 1) { state.isScrolling = false; KillTimer(hwnd, 1); InvalidateRect(hwnd, NULL, FALSE); return 0; }

        if (uMsg == WM_PAINT) {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
            RECT rect; GetClientRect(hwnd, &rect);
            HDC memDC = CreateCompatibleDC(hdc); HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom); SelectObject(memDC, memBitmap);

            HBRUSH bgBrush = CreateSolidBrush(state.isLocked ? RGB(0, 0, 0) : RGB(20, 20, 20));
            FillRect(memDC, &rect, bgBrush); DeleteObject(bgBrush);

            LayoutInfo li = ComputeLayout();

            for (int i = '0'; i <= 'Z'; i++) {
                if (!state.showExtraKey[i]) continue;
                int bc, br; float ox;
                if (GetGridPos(i, bc, ox, br)) {
                    int x = (int)((li.colMap[bc] + ox) * 50) + 15;
                    int y = (int)(li.rowMap[br] * 50) + 10;
                    wchar_t txt[2] = { (wchar_t)i, 0 };
                    DrawButton(memDC, x, y, 45, 45, txt, state.keys[i]);
                }
            }

            if (state.showSpace) {
                DrawButton(memDC, 15, li.spaceY, 145, 25, L"SPACE", state.keys[VK_SPACE]);
            }
            if (state.showShift || state.showCtrl) {
                int curX = 15;
                if (state.showShift) {
                    DrawButton(memDC, curX, li.shiftCtrlY, 70, 25, L"SHIFT", state.keys[VK_LSHIFT] || state.keys[VK_SHIFT]);
                    curX += 75; 
                }
                if (state.showCtrl) {
                    DrawButton(memDC, curX, li.shiftCtrlY, 70, 25, L"CTRL", state.keys[VK_LCONTROL] || state.keys[VK_CONTROL]);
                }
            }
            
            DrawMouseUI(memDC, li.mouseX, 10);

            BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);
            DeleteObject(memBitmap); DeleteDC(memDC); EndPaint(hwnd, &ps); return 0;
        }
        else if (uMsg == WM_NCHITTEST && !state.isLocked) return HTCAPTION;
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

public:
    static HWND Create(HINSTANCE hInstance) {
        WNDCLASSW wc = { }; wc.lpfnWndProc = WindowProc; wc.hInstance = hInstance; wc.lpszClassName = L"OverlayClass";
        wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON)); RegisterClassW(&wc);
        HWND hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, L"OverlayClass", L"Overlay", WS_POPUP, 400, 400, AppState::Get().dynamicWidth, AppState::Get().dynamicHeight, NULL, NULL, hInstance, NULL);
        SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY); return hwnd;
    }

    static void UpdateSize() {
        AppState& state = AppState::Get();
        LayoutInfo li = ComputeLayout();
        
        state.dynamicHeight = li.lowestY;
        state.dynamicWidth = li.mouseRightEdge; 

        SetWindowPos(state.hwndOverlay, NULL, 0, 0, (int)(state.dynamicWidth * state.uiScale), (int)(state.dynamicHeight * state.uiScale), SWP_NOMOVE | SWP_NOZORDER);
        InvalidateRect(state.hwndOverlay, NULL, FALSE);
    }
};