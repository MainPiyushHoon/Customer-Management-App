#include "TestRunner.hpp"
#include "ui/AnimationManager.hpp"
#include "ui/NeoWidgets.hpp"
#include "imgui.h"

int main() {
    std::cout << "Running AnimationManager & Tactile Interaction Tests...\n";

    // 1. ViewTransitionManager tests
    App::UI::ViewTransitionManager vtm;
    TestRunner::assert_true(vtm.isFinished(), "Transition manager starts finished at rest");
    TestRunner::assert_equal(vtm.getAlpha(), 1.0f, "Initial alpha is 1.0f");

    vtm.triggerTransition(0.15f);
    TestRunner::assert_false(vtm.isFinished(), "Transition starts active");
    TestRunner::assert_equal(vtm.getAlpha(), 0.0f, "Alpha starts at 0.0f on trigger");

    vtm.update(0.075f); // Halfway (50%)
    TestRunner::assert_false(vtm.isFinished(), "Transition still active halfway");
    TestRunner::assert_true(vtm.getAlpha() >= 0.49f && vtm.getAlpha() <= 0.51f, "Alpha is ~0.5 at 50% duration");

    vtm.update(0.075f); // Remaining 50%
    TestRunner::assert_true(vtm.isFinished(), "Transition finishes after 150ms");
    TestRunner::assert_equal(vtm.getAlpha(), 1.0f, "Alpha reaches 1.0f upon completion");

    // Over-update past duration
    vtm.update(1.0f);
    TestRunner::assert_true(vtm.isFinished(), "Transition stays finished after over-updating");
    TestRunner::assert_equal(vtm.getAlpha(), 1.0f, "Alpha clamped to 1.0f");

    // Reset test
    vtm.triggerTransition(0.20f);
    TestRunner::assert_false(vtm.isFinished(), "Trigger starts active");
    vtm.reset();
    TestRunner::assert_true(vtm.isFinished(), "Reset sets finished to true");
    TestRunner::assert_equal(vtm.getAlpha(), 1.0f, "Reset restores alpha to 1.0f");

    // 2. ToastNotificationManager tests
    App::UI::ToastNotificationManager tnm;
    TestRunner::assert_false(tnm.hasActiveToasts(), "No toasts initially");
    TestRunner::assert_equal(tnm.getActiveToastCount(), static_cast<size_t>(0), "Initial toast count is 0");

    tnm.showToast("ORDER DELETED", 0xFFBCAEFF, 3.0f);
    TestRunner::assert_true(tnm.hasActiveToasts(), "Toast is active after showToast");
    TestRunner::assert_equal(tnm.getActiveToastCount(), static_cast<size_t>(1), "Toast count is 1");

    // Check slide-in progress
    tnm.update(0.15f); // halfway into 0.25s slide-in
    TestRunner::assert_true(tnm.hasActiveToasts(), "Toast still active during slide-in");
    TestRunner::assert_true(tnm.getToasts()[0].alpha > 0.0f, "Toast alpha increasing during slide-in");
    TestRunner::assert_true(tnm.getToasts()[0].yOffset > -50.0f, "Toast yOffset sliding downwards towards 0");

    // Finish slide-in, enter dwell
    tnm.update(0.5f);
    TestRunner::assert_true(tnm.getToasts()[0].alpha == 1.0f, "Toast alpha reaches 1.0f at dwell");
    TestRunner::assert_true(tnm.getToasts()[0].yOffset == 0.0f, "Toast yOffset reaches 0.0f at dwell");

    // Enqueue second toast (stacking test)
    tnm.showToast("MEASUREMENTS SAVED", 0xFF00FFA6, 3.0f);
    TestRunner::assert_equal(tnm.getActiveToastCount(), static_cast<size_t>(2), "Two toasts active concurrently");

    // Advance to fade-out phase of first toast
    // First toast elapsed = 0.15 + 0.5 + 2.2 = 2.85s (duration 3.0s, last 0.5s is fade out)
    tnm.update(2.2f);
    TestRunner::assert_true(tnm.hasActiveToasts(), "Toasts still active");
    TestRunner::assert_true(tnm.getToasts()[0].alpha < 1.0f, "First toast fading out during final 0.5s");

    // Advance past first toast duration
    tnm.update(0.3f); // First toast elapsed = 3.15s, expired!
    TestRunner::assert_equal(tnm.getActiveToastCount(), static_cast<size_t>(1), "First toast expired and removed from queue");

    // Advance past second toast duration
    tnm.update(3.0f);
    TestRunner::assert_false(tnm.hasActiveToasts(), "All toasts expired after dwell time");
    TestRunner::assert_equal(tnm.getActiveToastCount(), static_cast<size_t>(0), "Toast count is 0");

    // 3. Render test (Headless ImGui context)
    ImGuiContext* ctx = ImGui::CreateContext();
    TestRunner::assert_true(ctx != nullptr, "ImGui context created for render verification");
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(1920.0f, 1080.0f);
    io.DeltaTime = 1.0f / 60.0f;
    unsigned char* tex_pixels = nullptr;
    int tex_w = 0, tex_h = 0;
    io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);
    ImGui::NewFrame();

    tnm.showToast("TEST RENDER TOAST", 0xFF00E6FF, 2.0f);
    tnm.update(0.1f);
    tnm.render(); // Should render without error to foreground draw list

    // Verify NeoWidgets::CalculateButtonSize enhancements
    ImVec2 nullSize = App::UI::NeoWidgets::CalculateButtonSize(nullptr);
    TestRunner::assert_true(nullSize.x >= 32.0f, "CalculateButtonSize(nullptr) handles null safely");
    TestRunner::assert_true(nullSize.y >= 36.0f, "CalculateButtonSize(nullptr) min height 36px");

    ImVec2 baseSize = App::UI::NeoWidgets::CalculateButtonSize("SAVE");
    ImVec2 hashSize = App::UI::NeoWidgets::CalculateButtonSize("SAVE##unique_id_123");
    TestRunner::assert_near(baseSize.x, hashSize.x, "CalculateButtonSize ignores text after ##");

    ImGui::DestroyContext(ctx);

    return TestRunner::summary();
}
