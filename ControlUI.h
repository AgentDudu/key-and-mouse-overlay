#pragma once
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h> // <--- NEW: For System Tray Icon
#include "AppState.h"
#include "OverlayUI.h"
#include "ExtraKeysUI.h" 
#include "resource.h"

#define WM_TRAYICON (WM_USER + 2)

class ControlUI {
private:
    static void ChooseCustomColor(HWND hwndParent, int target) {
        AppState& state = AppState::Get(); CHOOSECOLORW cc = {0}; static COLORREF acrCustClr[16]; 
        cc.lStructSize = sizeof(cc); cc.hwndOwner = hwndParent; cc.lpCustColors = (LPDWORD)acrCustClr;
        cc.rgbResult = (target == 1) ? state.activeBg : (target == 2) ? state.outlineColor : state.inactiveBg; cc.Flags = CC_FULLOPEN | CC_RGBINIT;
        if (ChooseColorW(&cc) == TRUE) {
            if (target == 1) { state.activeBg = cc.rgbResult; int r = GetRValue(state.activeBg), g = GetGValue(state.activeBg), b = GetBValue(state.activeBg); state.activeText = ((r * 0.299 + g * 0.587 + b * 0.114) > 128) ? RGB(0, 0, 0) : RGB(255, 255, 255); } 
            else if (target == 0) { state.inactiveBg = cc.rgbResult; int r = GetRValue(state.inactiveBg), g = GetGValue(state.inactiveBg), b = GetBValue(state.inactiveBg); state.inactiveText = ((r * 0.299 + g * 0.587 + b * 0.114) > 128) ? RGB(0, 0, 0) : RGB(255, 255, 255); } 
            else if (target == 2) { state.outlineColor = cc.rgbResult; }
            if (target != 2) { state.currentScheme = 3; SendMessageW(GetDlgItem(hwndParent, 5), CB_SETCURSEL, 3, 0); }
            PostMessage(state.hwndOverlay, WM_REDRAW_OVERLAY, 0, 0); InvalidateRect(hwndParent, NULL, TRUE); 
        }
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        AppState& state = AppState::Get();
        if (uMsg == WM_CREATE) {
            
            NOTIFYICONDATAW nid = {};
            nid.cbSize = sizeof(NOTIFYICONDATAW);
            nid.hWnd = hwnd;
            nid.uID = 1;
            nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
            nid.uCallbackMessage = WM_TRAYICON;
            nid.hIcon = LoadIcon((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDI_APP_ICON));
            lstrcpyW(nid.szTip, L"Overlay Settings");
            Shell_NotifyIconW(NIM_ADD, &nid);

            CreateWindowW(L"BUTTON", L"Unlock / Move Overlay", WS_CHILD | WS_VISIBLE, 20, 15, 200, 30, hwnd, (HMENU)1, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Size +", WS_CHILD | WS_VISIBLE, 20, 50, 95, 30, hwnd, (HMENU)2, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Size -", WS_CHILD | WS_VISIBLE, 125, 50, 95, 30, hwnd, (HMENU)3, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Add Extra Keys...", WS_CHILD | WS_VISIBLE, 20, 85, 200, 30, hwnd, (HMENU)8, NULL, NULL);
            CreateWindowW(L"STATIC", L"Color Scheme:", WS_CHILD | WS_VISIBLE, 20, 120, 100, 20, hwnd, NULL, NULL, NULL);
            HWND hCombo = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE, 20, 140, 200, 100, hwnd, (HMENU)5, NULL, NULL);
            SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Cyan (Default)"); SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Red");
            SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Green"); SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Custom...");
            SendMessageW(hCombo, CB_SETCURSEL, state.currentScheme, 0);

            CreateWindowW(L"BUTTON", L"Pick Base", WS_CHILD | WS_VISIBLE, 20, 175, 80, 25, hwnd, (HMENU)6, NULL, NULL);
            CreateWindowW(L"BUTTON", L"-", WS_CHILD | WS_VISIBLE, 145, 175, 20, 25, hwnd, (HMENU)11, NULL, NULL);
            CreateWindowW(L"BUTTON", L"+", WS_CHILD | WS_VISIBLE, 200, 175, 20, 25, hwnd, (HMENU)12, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Pick High", WS_CHILD | WS_VISIBLE, 20, 210, 80, 25, hwnd, (HMENU)7, NULL, NULL);
            CreateWindowW(L"BUTTON", L"-", WS_CHILD | WS_VISIBLE, 145, 210, 20, 25, hwnd, (HMENU)13, NULL, NULL);
            CreateWindowW(L"BUTTON", L"+", WS_CHILD | WS_VISIBLE, 200, 210, 20, 25, hwnd, (HMENU)14, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Pick Outln", WS_CHILD | WS_VISIBLE, 20, 245, 80, 25, hwnd, (HMENU)16, NULL, NULL);
            CreateWindowW(L"BUTTON", L"-", WS_CHILD | WS_VISIBLE, 145, 245, 20, 25, hwnd, (HMENU)17, NULL, NULL);
            CreateWindowW(L"BUTTON", L"+", WS_CHILD | WS_VISIBLE, 200, 245, 20, 25, hwnd, (HMENU)18, NULL, NULL);

            CreateWindowW(L"BUTTON", L"Hide to Tray (Right-Click Tray to Exit)", WS_CHILD | WS_VISIBLE, 20, 285, 200, 30, hwnd, (HMENU)4, NULL, NULL);
            
            EnumChildWindows(hwnd, [](HWND child, LPARAM font) -> BOOL { SendMessageW(child, WM_SETFONT, font, TRUE); return TRUE; }, (LPARAM)state.hFontUI);
            return 0;
        }
        
        else if (uMsg == WM_TRAYICON) {
            if (lParam == WM_LBUTTONUP) {
                ShowWindow(hwnd, IsWindowVisible(hwnd) ? SW_HIDE : SW_RESTORE);
                SetForegroundWindow(hwnd);
            }
            else if (lParam == WM_RBUTTONUP) {
                POINT pt; GetCursorPos(&pt);
                HMENU hMenu = CreatePopupMenu();
                AppendMenuW(hMenu, MF_STRING, 101, L"Settings");
                AppendMenuW(hMenu, MF_STRING, 102, state.isLocked ? L"Unlock Overlay" : L"Lock Overlay");
                AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenuW(hMenu, MF_STRING, 103, L"Exit Application");
                SetForegroundWindow(hwnd);
                int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(hMenu);
                if (cmd == 101) ShowWindow(hwnd, SW_RESTORE);
                if (cmd == 102) SendMessage(hwnd, WM_COMMAND, 1, 0); 
                if (cmd == 103) PostQuitMessage(0);
            }
            return 0;
        }

        else if (uMsg == WM_PAINT) {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
            RECT rcBase = { 110, 175, 135, 200 }; HBRUSH brBase = CreateSolidBrush(state.inactiveBg); FillRect(hdc, &rcBase, brBase); FrameRect(hdc, &rcBase, (HBRUSH)GetStockObject(BLACK_BRUSH)); DeleteObject(brBase);
            RECT rcHigh = { 110, 210, 135, 235 }; HBRUSH brHigh = CreateSolidBrush(state.activeBg); FillRect(hdc, &rcHigh, brHigh); FrameRect(hdc, &rcHigh, (HBRUSH)GetStockObject(BLACK_BRUSH)); DeleteObject(brHigh);
            RECT rcOut  = { 110, 245, 135, 270 }; HBRUSH brOut = CreateSolidBrush(state.outlineColor); FillRect(hdc, &rcOut, brOut); FrameRect(hdc, &rcOut, (HBRUSH)GetStockObject(BLACK_BRUSH)); DeleteObject(brOut);
            
            SetBkMode(hdc, TRANSPARENT); SelectObject(hdc, state.hFontUI); 
            wchar_t buf[16];
            wsprintfW(buf, L"%d%%", state.baseAlpha); TextOutW(hdc, 168, 178, buf, lstrlenW(buf));
            wsprintfW(buf, L"%d%%", state.highlightAlpha); TextOutW(hdc, 168, 213, buf, lstrlenW(buf));
            wsprintfW(buf, L"%d%%", state.outlineAlpha); TextOutW(hdc, 168, 248, buf, lstrlenW(buf));
            EndPaint(hwnd, &ps); return 0;
        }
        else if (uMsg == WM_COMMAND) {
            int wmId = LOWORD(wParam);
            if (HIWORD(wParam) == CBN_SELCHANGE && wmId == 5) { HWND hCombo = (HWND)lParam; int sel = SendMessageW(hCombo, CB_GETCURSEL, 0, 0); if (sel != 3) { state.currentScheme = sel; 
                if (sel == 0) { state.activeBg = RGB(0, 255, 204); state.inactiveBg = RGB(34, 34, 34); state.activeText = RGB(0, 0, 0); state.inactiveText = RGB(255, 255, 255); } 
                else if (sel == 1) { state.activeBg = RGB(255, 70, 70); state.inactiveBg = RGB(45, 20, 20); state.activeText = RGB(255, 255, 255); state.inactiveText = RGB(255, 255, 255); } 
                else if (sel == 2) { state.activeBg = RGB(70, 255, 70); state.inactiveBg = RGB(20, 45, 20); state.activeText = RGB(0, 0, 0); state.inactiveText = RGB(255, 255, 255); }
                state.baseAlpha = 80; state.highlightAlpha = 100; state.outlineAlpha = 0; 
                PostMessage(state.hwndOverlay, WM_REDRAW_OVERLAY, 0, 0); InvalidateRect(hwnd, NULL, TRUE); } 
            }
            if (wmId == 1) { state.isLocked = !state.isLocked; LONG exStyle = GetWindowLong(state.hwndOverlay, GWL_EXSTYLE); if (state.isLocked) SetWindowLong(state.hwndOverlay, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT); else SetWindowLong(state.hwndOverlay, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT); PostMessage(state.hwndOverlay, WM_REDRAW_OVERLAY, 0, 0); }
            else if (wmId == 2) { if (state.uiScale < 3.0f) state.uiScale += 0.1f; OverlayUI::UpdateSize(); }
            else if (wmId == 3) { if (state.uiScale > 0.5f) state.uiScale -= 0.1f; OverlayUI::UpdateSize(); }
            
            else if (wmId == 4) { ShowWindow(hwnd, SW_HIDE); }
            
            else if (wmId == 6) { ChooseCustomColor(hwnd, 0); } else if (wmId == 7) { ChooseCustomColor(hwnd, 1); } else if (wmId == 16) { ChooseCustomColor(hwnd, 2); }
            else if (wmId == 8) { ExtraKeysUI::Show(); }
            else if (wmId == 11) { state.baseAlpha = (state.baseAlpha > 10) ? state.baseAlpha - 10 : 0; InvalidateRect(hwnd, NULL, TRUE); PostMessage(state.hwndOverlay, WM_REDRAW_OVERLAY, 0, 0); }
            else if (wmId == 12) { state.baseAlpha = (state.baseAlpha < 90) ? state.baseAlpha + 10 : 100; InvalidateRect(hwnd, NULL, TRUE); PostMessage(state.hwndOverlay, WM_REDRAW_OVERLAY, 0, 0); }
            else if (wmId == 13) { state.highlightAlpha = (state.highlightAlpha > 10) ? state.highlightAlpha - 10 : 0; InvalidateRect(hwnd, NULL, TRUE); PostMessage(state.hwndOverlay, WM_REDRAW_OVERLAY, 0, 0); }
            else if (wmId == 14) { state.highlightAlpha = (state.highlightAlpha < 90) ? state.highlightAlpha + 10 : 100; InvalidateRect(hwnd, NULL, TRUE); PostMessage(state.hwndOverlay, WM_REDRAW_OVERLAY, 0, 0); }
            else if (wmId == 17) { state.outlineAlpha = (state.outlineAlpha > 10) ? state.outlineAlpha - 10 : 0; InvalidateRect(hwnd, NULL, TRUE); PostMessage(state.hwndOverlay, WM_REDRAW_OVERLAY, 0, 0); }
            else if (wmId == 18) { state.outlineAlpha = (state.outlineAlpha < 90) ? state.outlineAlpha + 10 : 100; InvalidateRect(hwnd, NULL, TRUE); PostMessage(state.hwndOverlay, WM_REDRAW_OVERLAY, 0, 0); }
            return 0;
        }
        else if (uMsg == WM_CLOSE) {
            ShowWindow(hwnd, SW_HIDE);
            return 0;
        }
        else if (uMsg == WM_DESTROY) {
            NOTIFYICONDATAW nid = {};
            nid.cbSize = sizeof(NOTIFYICONDATAW);
            nid.hWnd = hwnd;
            nid.uID = 1;
            Shell_NotifyIconW(NIM_DELETE, &nid);
            PostQuitMessage(0); 
            return 0; 
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

public:
    static HWND Create(HINSTANCE hInstance) {
        WNDCLASSW wc = { }; wc.lpfnWndProc = WindowProc; wc.hInstance = hInstance; wc.lpszClassName = L"ControlClass";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON)); RegisterClassW(&wc);
        return CreateWindowExW(0, L"ControlClass", L"Overlay Settings", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 100, 100, 260, 365, NULL, NULL, hInstance, NULL); 
    }
};