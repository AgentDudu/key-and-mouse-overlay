#include <windows.h>
#include <gdiplus.h>
#include "AppState.h"
#include "InputManager.h"
#include "OverlayUI.h"
#include "ExtraKeysUI.h"
#include "ControlUI.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comdlg32.lib") 
#pragma comment(lib, "gdiplus.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    AppState::Get().hwndOverlay = OverlayUI::Create(hInstance);
    HWND hwndControl = ControlUI::Create(hInstance);
    ExtraKeysUI::Init(hInstance);

    InputManager::Install(hInstance);

    ShowWindow(hwndControl, nCmdShow);
    ShowWindow(AppState::Get().hwndOverlay, SW_SHOWNA);
    SendMessage(AppState::Get().hwndOverlay, WM_REDRAW_OVERLAY, 0, 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    InputManager::Remove();
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}