#pragma once
#include <windows.h>

struct LayoutInfo {
    int colMap[35];
    int rowMap[6];
    int max_r;
    int mouseX;
    int spaceY;
    int shiftCtrlY;
    int lowestY;
    int mouseRightEdge;
};

class LayoutEngine {
public:
    static bool GetGridPos(int key, int& base_col, float& offset_x, int& base_row) {
        if (key >= '1' && key <= '9') { base_col = key - '1'; offset_x = 0.0f; base_row = 0; return true; }
        if (key == '0') { base_col = 9; offset_x = 0.0f; base_row = 0; return true; }
        const char* r1 = "QWERTYUIOP"; for (int i = 0; i < 10; i++) if (key == r1[i]) { base_col = i; offset_x = 0.0f; base_row = 1; return true; }
        const char* r2 = "ASDFGHJKL";  for (int i = 0; i < 9; i++) if (key == r2[i]) { base_col = i; offset_x = 0.0f; base_row = 2; return true; }
        const char* r3 = "ZXCVBNM";    for (int i = 0; i < 7; i++) if (key == r3[i]) { base_col = i; offset_x = 0.5f; base_row = 3; return true; }
        if (key == VK_UP) { base_col = 16; offset_x = 0; base_row = 2; return true; }
        if (key == VK_LEFT) { base_col = 15; offset_x = 0; base_row = 3; return true; }
        if (key == VK_DOWN) { base_col = 16; offset_x = 0; base_row = 3; return true; }
        if (key == VK_RIGHT) { base_col = 17; offset_x = 0; base_row = 3; return true; }
        if (key == VK_DIVIDE) { base_col = 19; offset_x = 0; base_row = 0; return true; }
        if (key == VK_MULTIPLY) { base_col = 20; offset_x = 0; base_row = 0; return true; }
        if (key == VK_SUBTRACT) { base_col = 21; offset_x = 0; base_row = 0; return true; }
        if (key == VK_ADD) { base_col = 22; offset_x = 0; base_row = 0; return true; }
        if (key == VK_NUMPAD7) { base_col = 19; offset_x = 0; base_row = 1; return true; }
        if (key == VK_NUMPAD8) { base_col = 20; offset_x = 0; base_row = 1; return true; }
        if (key == VK_NUMPAD9) { base_col = 21; offset_x = 0; base_row = 1; return true; }
        if (key == VK_NUMPAD4) { base_col = 19; offset_x = 0; base_row = 2; return true; }
        if (key == VK_NUMPAD5) { base_col = 20; offset_x = 0; base_row = 2; return true; }
        if (key == VK_NUMPAD6) { base_col = 21; offset_x = 0; base_row = 2; return true; }
        if (key == VK_NUMPAD1) { base_col = 19; offset_x = 0; base_row = 3; return true; }
        if (key == VK_NUMPAD2) { base_col = 20; offset_x = 0; base_row = 3; return true; }
        if (key == VK_NUMPAD3) { base_col = 21; offset_x = 0; base_row = 3; return true; }
        if (key == VK_RETURN) { base_col = 22; offset_x = 0; base_row = 2; return true; }
        if (key == VK_NUMPAD0) { base_col = 19; offset_x = 0; base_row = 4; return true; }
        if (key == VK_DECIMAL) { base_col = 21; offset_x = 0; base_row = 4; return true; }
        return false;
    }

    static LayoutInfo ComputeLayout(const bool* showExtraKey, bool showSpace, bool showShift, bool showCtrl) {
        LayoutInfo li = {0};
        bool activeCols[35] = { false }, activeRows[6] = { false };
        for (int i = 0; i < 256; i++) {
            if (!showExtraKey[i]) continue;
            int bc, br; float ox; 
            if (GetGridPos(i, bc, ox, br)) { activeCols[bc] = true; activeRows[br] = true; }
        }
        
        int curr_c = 0; for (int i = 0; i < 35; i++) if (activeCols[i]) li.colMap[i] = curr_c++;
        int curr_r = 0; for (int i = 0; i < 6; i++) if (activeRows[i]) li.rowMap[i] = curr_r++;
        li.max_r = (curr_r > 0) ? curr_r - 1 : -1;
        
        float out_maxC = 0;
        for (int i = 0; i < 256; i++) {
            if (!showExtraKey[i]) continue;
            int bc, br; float ox;
            if (GetGridPos(i, bc, ox, br)) { float c = li.colMap[bc] + ox; if (c > out_maxC) out_maxC = c; }
        }
        
        int max_kbd_x = 15;
        if (li.max_r > -1) max_kbd_x = (int)((out_maxC * 50) + 15 + 45);
        int curY = (li.max_r > -1) ? (li.max_r * 50) + 60 : 10;
        
        if (showSpace) { 
            li.spaceY = curY; 
            if (15 + 145 > max_kbd_x) max_kbd_x = 15 + 145; 
            curY += 30; 
        } else li.spaceY = -1;

        if (showShift || showCtrl) {
            li.shiftCtrlY = curY; 
            int w = 15;
            if (showShift) w += 70; 
            if (showShift && showCtrl) w += 5; 
            if (showCtrl) w += 70;
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
};

class ColorMath {
public:
    static COLORREF GetContrastTextColor(COLORREF bg) {
        int r = GetRValue(bg);
        int g = GetGValue(bg);
        int b = GetBValue(bg);
        return ((r * 0.299 + g * 0.587 + b * 0.114) > 128) ? RGB(0, 0, 0) : RGB(255, 255, 255);
    }
};