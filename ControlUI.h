#pragma once
#include <windows.h>
#include <commdlg.h>
#include "AppState.h"
#include "OverlayUI.h"
#include "ExtraKeysUI.h"
#include "resource.h"

class ControlUI {
private:
    static void ApplyScheme(int sel) {
        AppState& state = AppState::Get();
        if (sel == 0) { state.activeBg = RGB(0, 255, 204); state.inactiveBg = RGB(34, 34, 34); state.activeText = RGB(0, 0, 0); state.inactiveText = RGB(255, 255, 255); } 
        else if (sel == 1) { state.activeBg = RGB(255, 70, 70); state.inactiveBg = RGB(45, 20, 20); state.activeText = RGB(255, 255, 255); state.inactiveText = RGB(255, 255, 255); } 
        else if (sel == 2) { state.activeBg = RGB(70, 255, 70); state.inactiveBg = RGB(20, 45, 20); state.activeText = RGB(0, 0, 0); state.inactiveText = RGB(255, 255, 255); }
    }

    static void ChooseCustomColor(HWND hwndParent, bool isHighlight) {
        AppState& state = AppState::Get(); CHOOSECOLORW cc = {0}; static COLORREF acrCustClr[16]; 
        cc.lStructSize = sizeof(cc); cc.hwndOwner = hwndParent; cc.lpCustColors = (LPDWORD)acrCustClr;
        cc.rgbResult = isHighlight ? state.activeBg : state.inactiveBg; cc.Flags = CC_FULLOPEN | CC_RGBINIT;
        if (ChooseColorW(&cc) == TRUE) {
            if (isHighlight) {
                state.activeBg = cc.rgbResult; int r = GetRValue(state.activeBg), g = GetGValue(state.activeBg), b = GetBValue(state.activeBg);
                if ((r * 0.299 + g * 0.587 + b * 0.114) > 128) state.activeText = RGB(0, 0, 0); else state.activeText = RGB(255, 255, 255);
            } else {
                state.inactiveBg = cc.rgbResult; int r = GetRValue(state.inactiveBg), g = GetGValue(state.inactiveBg), b = GetBValue(state.inactiveBg);
                if ((r * 0.299 + g * 0.587 + b * 0.114) > 128) state.inactiveText = RGB(0, 0, 0); else state.inactiveText = RGB(255, 255, 255);
            }
            state.currentScheme = 3; HWND hCombo = GetDlgItem(hwndParent, 5); SendMessageW(hCombo, CB_SETCURSEL, 3, 0);
            InvalidateRect(state.hwndOverlay, NULL, FALSE); InvalidateRect(hwndParent, NULL, TRUE); 
        }
    }

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        AppState& state = AppState::Get();

        if (uMsg == WM_CREATE) {
            CreateWindowW(L"BUTTON", L"Unlock / Move Overlay", WS_CHILD | WS_VISIBLE, 20, 15, 200, 30, hwnd, (HMENU)1, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Size +", WS_CHILD | WS_VISIBLE, 20, 55, 95, 30, hwnd, (HMENU)2, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Size -", WS_CHILD | WS_VISIBLE, 125, 55, 95, 30, hwnd, (HMENU)3, NULL, NULL);
            
            CreateWindowW(L"BUTTON", L"Add Extra Keys...", WS_CHILD | WS_VISIBLE, 20, 95, 200, 30, hwnd, (HMENU)8, NULL, NULL);
            
            CreateWindowW(L"STATIC", L"Color Scheme:", WS_CHILD | WS_VISIBLE, 20, 135, 100, 20, hwnd, NULL, NULL, NULL);
            HWND hCombo = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE, 20, 155, 200, 100, hwnd, (HMENU)5, NULL, NULL);
            SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Cyan (Default)"); SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Red");
            SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Green"); SendMessageW(hCombo, CB_ADDSTRING, 0, (LPARAM)L"Custom...");
            SendMessageW(hCombo, CB_SETCURSEL, state.currentScheme, 0);

            CreateWindowW(L"BUTTON", L"Pick Base", WS_CHILD | WS_VISIBLE, 20, 190, 120, 25, hwnd, (HMENU)6, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Pick Highlight", WS_CHILD | WS_VISIBLE, 20, 225, 120, 25, hwnd, (HMENU)7, NULL, NULL);
            CreateWindowW(L"BUTTON", L"Close App", WS_CHILD | WS_VISIBLE, 20, 265, 200, 30, hwnd, (HMENU)4, NULL, NULL);
            return 0;
        }
        else if (uMsg == WM_PAINT) {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
            RECT rcBase = { 150, 190, 220, 215 }; HBRUSH brushBase = CreateSolidBrush(state.inactiveBg); FillRect(hdc, &rcBase, brushBase); FrameRect(hdc, &rcBase, (HBRUSH)GetStockObject(BLACK_BRUSH)); DeleteObject(brushBase);
            RECT rcHigh = { 150, 225, 220, 250 }; HBRUSH brushHigh = CreateSolidBrush(state.activeBg); FillRect(hdc, &rcHigh, brushHigh); FrameRect(hdc, &rcHigh, (HBRUSH)GetStockObject(BLACK_BRUSH)); DeleteObject(brushHigh);
            EndPaint(hwnd, &ps); return 0;
        }
        else if (uMsg == WM_COMMAND) {
            int wmId = LOWORD(wParam);
            if (HIWORD(wParam) == CBN_SELCHANGE && wmId == 5) {
                HWND hCombo = (HWND)lParam; int sel = SendMessageW(hCombo, CB_GETCURSEL, 0, 0);
                if (sel != 3) { state.currentScheme = sel; ApplyScheme(sel); InvalidateRect(state.hwndOverlay, NULL, FALSE); InvalidateRect(hwnd, NULL, TRUE); }
            }
            if (wmId == 1) { 
                state.isLocked = !state.isLocked; LONG exStyle = GetWindowLong(state.hwndOverlay, GWL_EXSTYLE);
                if (state.isLocked) { SetWindowLong(state.hwndOverlay, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT); SetLayeredWindowAttributes(state.hwndOverlay, RGB(0, 0, 0), 0, LWA_COLORKEY); } 
                else { SetWindowLong(state.hwndOverlay, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT); SetLayeredWindowAttributes(state.hwndOverlay, RGB(0, 0, 0), 200, LWA_ALPHA); }
                InvalidateRect(state.hwndOverlay, NULL, FALSE);
            }
            else if (wmId == 2) { if (state.uiScale < 3.0f) state.uiScale += 0.1f; OverlayUI::UpdateSize(); }
            else if (wmId == 3) { if (state.uiScale > 0.5f) state.uiScale -= 0.1f; OverlayUI::UpdateSize(); }
            else if (wmId == 4) { PostQuitMessage(0); }
            else if (wmId == 6) { ChooseCustomColor(hwnd, false); }
            else if (wmId == 7) { ChooseCustomColor(hwnd, true); }
            
            else if (wmId == 8) { ExtraKeysUI::Show(); }
            return 0;
        }
        else if (uMsg == WM_DESTROY) { PostQuitMessage(0); return 0; }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

public:
    static HWND Create(HINSTANCE hInstance) {
        WNDCLASSW wc = { }; wc.lpfnWndProc = WindowProc; wc.hInstance = hInstance; wc.lpszClassName = L"ControlClass";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON)); 
        RegisterClassW(&wc);
        return CreateWindowExW(0, L"ControlClass", L"Overlay", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 100, 100, 260, 350, NULL, NULL, hInstance, NULL);
    }
};