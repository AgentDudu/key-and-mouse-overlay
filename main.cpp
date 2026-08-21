#include <windows.h>
#include "AppState.h"
#include "InputManager.h"
#include "OverlayUI.h"
#include "ControlUI.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    AppState::Get().hwndOverlay = OverlayUI::Create(hInstance);
    HWND hwndControl = ControlUI::Create(hInstance);

    InputManager::Install(hInstance);

    ShowWindow(hwndControl, nCmdShow);
    ShowWindow(AppState::Get().hwndOverlay, SW_SHOWNA);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    InputManager::Remove();
    return 0;
}