#include <iostream>
#include <cassert>
#include <string>
#include "CoreLogic.h"

#define RUN_TEST(TestFunction) \
    std::cout << "Running " << #TestFunction << "... "; \
    TestFunction(); \
    std::cout << "[PASSED]\n";

void TestColorMath() {
    COLORREF darkBg = RGB(34, 34, 34);
    assert(ColorMath::GetContrastTextColor(darkBg) == RGB(255, 255, 255));

    COLORREF brightBg = RGB(0, 255, 204);
    assert(ColorMath::GetContrastTextColor(brightBg) == RGB(0, 0, 0));
}

void TestDefaultLayout() {
    bool fakeKeys[256] = { false };
    fakeKeys['W'] = true; fakeKeys['A'] = true; 
    fakeKeys['S'] = true; fakeKeys['D'] = true;

    LayoutInfo li = LayoutEngine::ComputeLayout(fakeKeys, true, true, true);
    assert(li.spaceY > 0);
    assert(li.shiftCtrlY == li.spaceY + 30);
    assert(li.mouseX == 220); 
}

void TestGridCompression() {
    bool fakeKeys[256] = { false };
    fakeKeys['W'] = true; fakeKeys['A'] = true; 
    fakeKeys['S'] = true; fakeKeys['D'] = true;
    fakeKeys[VK_DECIMAL] = true; 

    LayoutInfo li = LayoutEngine::ComputeLayout(fakeKeys, true, true, true);
    assert(li.mouseX == 270);
    assert(li.mouseRightEdge == 360); 
}

void TestRowCompression() {
    bool fakeKeys[256] = { false };
    fakeKeys['Z'] = true; 

    LayoutInfo li = LayoutEngine::ComputeLayout(fakeKeys, true, false, false); 
    assert(li.spaceY == 60);
    assert(li.mouseX == 220); 
}

void TestMinimalLayout() {
    bool fakeKeys[256] = { false };
    LayoutInfo li = LayoutEngine::ComputeLayout(fakeKeys, false, false, false);

    assert(li.mouseX == 75);
    assert(li.spaceY == -1);
    assert(li.shiftCtrlY == -1);
    assert(li.mouseRightEdge == 165);
}

void TestFullLayoutStressTest() {
    bool fakeKeys[256] = { false };
    for(int i = 0; i < 256; i++) fakeKeys[i] = true;

    LayoutInfo li = LayoutEngine::ComputeLayout(fakeKeys, true, true, true);

    assert(li.mouseX == 920);
    assert(li.spaceY == 260);
}

void TestUnmappedKeyIgnore() {
    bool fakeKeys[256] = { false };
    fakeKeys[VK_F1] = true;

    LayoutInfo li = LayoutEngine::ComputeLayout(fakeKeys, false, false, false);
    assert(li.mouseX == 75);
}

// ==========================================
// NEW: ADVANCED EDGE-CASE TESTS
// ==========================================

void TestModifierWidthProtection() {
    // Scenario: User turns OFF all keys and Spacebar. Turns ON Shift + Ctrl.
    bool fakeKeys[256] = { false };
    LayoutInfo li = LayoutEngine::ComputeLayout(fakeKeys, false, true, true);
    
    // Width of Shift (70) + Gap (5) + Ctrl (70) + Left Margin (15) = 160.
    // The engine must push the MouseX past 160 to avoid collision.
    // MouseX = 160 + 60 gap = 220.
    assert(li.mouseX == 220);
}

void TestDynamicHeight() {
    bool fakeKeys[256] = { false };
    
    // Scenario A: Nothing active. The Mouse dictates the window height.
    LayoutInfo li1 = LayoutEngine::ComputeLayout(fakeKeys, false, false, false);
    // Mouse bottom is 10(Y) + 45(Buttons) + 55(Body) + 10(Padding) = 120.
    assert(li1.lowestY == 120);

    // Scenario B: Extremely tall keyboard. WASD + Numpad0 + Modifiers.
    bool fakeKeysTall[256] = { false };
    fakeKeysTall['W'] = true;        // Base Row 1 -> Compresses to Row 0
    fakeKeysTall[VK_NUMPAD0] = true; // Base Row 4 -> Compresses to Row 1
    LayoutInfo li2 = LayoutEngine::ComputeLayout(fakeKeysTall, true, true, true);
    
    // Max Compressed Row = 1.
    // SpaceY = (1 * 50) + 60 = 110.
    // ShiftY = 110 + 30 = 140.
    // Lowest Edge = 140 + 30 = 170.
    // Since the keyboard (170) is taller than the mouse (120), lowestY MUST be 170.
    assert(li2.lowestY == 170);
}

void TestFractionalOffset() {
    // The Z row (ZXCVBNM) uses a 0.5 horizontal offset to mimic physical QWERTY stagger.
    bool fakeKeys[256] = { false };
    fakeKeys['M'] = true; 

    LayoutInfo li = LayoutEngine::ComputeLayout(fakeKeys, false, false, false);

    // 'M' is the only key, so it safely compresses to Column 0.
    // BUT the engine must retain its 0.5 float offset so it visually staggers!
    // X Coordinate = (0.5 offset * 50px) + 15 margin + 45 width = 85.
    // MouseX = 85 + 60 gap = 145.
    assert(li.mouseX == 145);
}

int main() {
    std::cout << "=================================\n";
    std::cout << "  GAMER OVERLAY - TEST SUITE     \n";
    std::cout << "=================================\n";

    RUN_TEST(TestColorMath);
    RUN_TEST(TestDefaultLayout);
    RUN_TEST(TestGridCompression);
    RUN_TEST(TestRowCompression);
    RUN_TEST(TestMinimalLayout);
    RUN_TEST(TestFullLayoutStressTest);
    RUN_TEST(TestUnmappedKeyIgnore);
    
    // New Tests
    RUN_TEST(TestModifierWidthProtection);
    RUN_TEST(TestDynamicHeight);
    RUN_TEST(TestFractionalOffset);

    std::cout << "=================================\n";
    std::cout << "  ALL 10 TESTS PASSED FLAWLESSLY \n";
    std::cout << "=================================\n";
    return 0;
}