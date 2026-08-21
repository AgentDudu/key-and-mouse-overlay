#pragma once
#include <windows.h>
#include "AppState.h"
#include "OverlayUI.h"
#include "resource.h"

class ExtraKeysUI {
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        AppState& state = AppState::Get();

        if (uMsg == WM_CREATE) {
            CreateWindowW(L"STATIC", L"Toggle Layout Keys:", WS_CHILD | WS_VISIBLE, 20, 15, 300, 20, hwnd, NULL, NULL, NULL);
            
            int startX = 20;
            int y = 40;

            struct KeyboardRow { const char* keys; int offsetX; };
            KeyboardRow rows[4] = {
                {"1234567890", 0},   
                {"QWERTYUIOP", 15},  
                {"ASDFGHJKL", 25},   
                {"ZXCVBNM", 45}      
            };

            for (int r = 0; r < 4; r++) {
                int x = startX + rows[r].offsetX;
                for (int i = 0; rows[r].keys[i] != '\0'; i++) {
                    char k = rows[r].keys[i];
                    wchar_t txt[2] = { (wchar_t)k, 0 };
                    
                    HWND cb = CreateWindowW(L"BUTTON", txt, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 
                                            x, y, 35, 20, hwnd, (HMENU)(UINT_PTR)k, NULL, NULL);
                    
                    if (state.showExtraKey[k]) SendMessage(cb, BM_SETCHECK, BST_CHECKED, 0);
                    x += 40; 
                }
                y += 30; 
            }

            y += 10;
            CreateWindowW(L"STATIC", L"Other:", WS_CHILD | WS_VISIBLE, 20, y, 200, 20, hwnd, NULL, NULL, NULL);
            y += 20;

            struct SpecialKey { const wchar_t* name; int id; bool* statePtr; };
            SpecialKey specials[] = {
                { L"Space", 1001, &state.showSpace },
                { L"Shift", 1002, &state.showShift },
                { L"Ctrl",  1003, &state.showCtrl },
                { L"MB 4",  1004, &state.showMB4 },
                { L"MB 5",  1005, &state.showMB5 }
            };

            int specX = 20;
            for (int i = 0; i < 5; i++) {
                HWND cb = CreateWindowW(L"BUTTON", specials[i].name, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 
                                        specX, y, 65, 20, hwnd, (HMENU)(UINT_PTR)specials[i].id, NULL, NULL);
                if (*(specials[i].statePtr)) SendMessage(cb, BM_SETCHECK, BST_CHECKED, 0);
                specX += 70;
            }

            return 0;
        }
        else if (uMsg == WM_COMMAND) {
            int wmId = LOWORD(wParam);
            
            if ((wmId >= '0' && wmId <= '9') || (wmId >= 'A' && wmId <= 'Z')) {
                bool isChecked = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED;
                state.showExtraKey[wmId] = isChecked;
                OverlayUI::UpdateSize();
            }
            else if (wmId >= 1001 && wmId <= 1005) {
                bool isChecked = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0) == BST_CHECKED;
                if (wmId == 1001) state.showSpace = isChecked;
                if (wmId == 1002) state.showShift = isChecked;
                if (wmId == 1003) state.showCtrl = isChecked;
                if (wmId == 1004) state.showMB4 = isChecked;
                if (wmId == 1005) state.showMB5 = isChecked;
                OverlayUI::UpdateSize();
            }
            return 0;
        }
        else if (uMsg == WM_CLOSE) {
            ShowWindow(hwnd, SW_HIDE); 
            return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

public:
    static void Init(HINSTANCE hInstance) {
        WNDCLASSW wc = { }; wc.lpfnWndProc = WindowProc; wc.hInstance = hInstance; wc.lpszClassName = L"ExtraKeysClass";
        wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON)); 
        RegisterClassW(&wc);

        AppState::Get().hwndExtraKeys = CreateWindowExW(
            0, L"ExtraKeysClass", L"Extra Keys Settings",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
            150, 150, 470, 265, 
            NULL, NULL, hInstance, NULL
        );
    }

    static void Show() {
        ShowWindow(AppState::Get().hwndExtraKeys, SW_SHOW);
        SetForegroundWindow(AppState::Get().hwndExtraKeys);
    }
};