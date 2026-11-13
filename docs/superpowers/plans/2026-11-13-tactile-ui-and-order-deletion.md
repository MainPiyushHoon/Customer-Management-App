# Tactile Neo-Brutalist UI & Order Deletion Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Transform the application into an interactive, tactile Neo-Brutalist desktop experience featuring physical button press animations, view cross-fades, action toasts, complete order deletion with confirmation modals, anti-overflow dynamic typography sizing, a high-luminance palette, and a clean end-user dashboard with Recent Orders.

**Architecture:** C++20 object-oriented architecture. Lightweight animation state manager using Dear ImGui immediate-mode delta time for coordinate displacement, alpha cross-fading, and toast queues without heavy external dependencies. Cascading order deletion across `Customer` and `OrderManager` with instant dual JSON/CSV persistence. Dynamic text bounding calculations in `NeoWidgets` to mathematically prevent button and card overflow.

**Tech Stack:** C++20, Dear ImGui v1.91.5, GLFW 3.4, OpenGL3, CMake, MinGW-w64 GCC 14.2.0.

## Global Constraints
- Native Windows C++20 application only (no web technologies, Electron, or external heavy animation engines).
- Neo-Brutalist aesthetic: 0 rounding, thick solid borders (2.5px), hard 4px drop shadows (`0xFF000000`).
- Strict typography readability: All backgrounds paired with black font MUST have luminance > 0.70 (no dark saturated magentas or purples behind black text).
- Zero developer noise for end users: No persistence engine boxes or disk file paths on the dashboard.
- Dual persistence: All order mutations must synchronize to both `data/*.json` and `data/*.csv`.

---

### Task 1: High-Luminance Neo-Brutalist Palette & Anti-Overflow Dynamic Sizing

**Files:**
- Modify: `include/ui/ThemeManager.hpp`
- Modify: `src/ui/ThemeManager.cpp`
- Modify: `include/ui/NeoWidgets.hpp`
- Modify: `src/ui/NeoWidgets.cpp`
- Test: `tests/test_theme.cpp`

**Interfaces:**
- Consumes: `ImGui::CalcTextSize()`
- Produces: `NeoWidgets::BrutalistButton(label, requestedWidth, requestedHeight, accentColor)` with guaranteed padding and no text overflow.
- Produces: Refined `ThemeManager::getPalette()` with high-luminance tints: Electric Lemon (`0xFF00E6FF`), Pastel Mint (`0xFF9DFF00` / `#A6FF00`), Sky Ice (`0xFFE5E7A0`), Soft Lavender (`0xFFFDB7D7`), Soft Coral (`0xFFBCAEFF`).

- [ ] **Step 1: Write test in `tests/test_theme.cpp` verifying high-luminance palette values and dynamic button sizing**

```cpp
// tests/test_theme.cpp addition
assert_true(((palette.accentYellow & 0xFF) > 200), "Yellow R component is vibrant");
assert_true(palette.accentPink != 0xFF7A00FF, "Dark magenta removed from dark mode");
```

- [ ] **Step 2: Run test to verify it fails on old palette values**

Run: `g++ -std=c++20 -Iinclude -Ilibs/imgui tests/test_theme.cpp src/ui/ThemeManager.cpp libs/imgui/imgui*.cpp -o build/test_theme.exe; .\build\test_theme.exe`
Expected: FAIL

- [ ] **Step 3: Update `ThemeManager` palette and `NeoWidgets::BrutalistButton` dynamic sizing**

In `src/ui/ThemeManager.cpp`: replace dark magenta `#FF007A` with Soft Coral `#FFAEBC` (`0xFFBCAEFF`) or Soft Lavender `#D7B7FD` (`0xFFFDB7D7`).
In `src/ui/NeoWidgets.cpp`:
```cpp
ImVec2 textSize = ImGui::CalcTextSize(label);
float minWidth = textSize.x + 32.0f; // 16px horizontal padding each side
float actualWidth = (width > 0.0f) ? std::max(width, minWidth) : minWidth;
float actualHeight = (height > 0.0f) ? std::max(height, 36.0f) : 36.0f;
```

- [ ] **Step 4: Run test to verify it passes**

Run: `.\build\test_theme.exe`
Expected: PASS

- [ ] **Step 5: Commit changes**

```bash
git add include/ui/ThemeManager.hpp src/ui/ThemeManager.cpp include/ui/NeoWidgets.hpp src/ui/NeoWidgets.cpp tests/test_theme.cpp
git commit -m "feat(ui): refine high-luminance palette and implement anti-overflow dynamic button sizing"
```

---

### Task 2: Mechanical Tactile Button Press & Animation Engine

**Files:**
- Create: `include/ui/AnimationManager.hpp`
- Create: `src/ui/AnimationManager.cpp`
- Modify: `src/ui/NeoWidgets.cpp`
- Modify: `CMakeLists.txt`
- Test: `tests/test_animations.cpp`

**Interfaces:**
- Produces: `class ViewTransitionManager` tracking tab cross-fades ($150\text{ms}$).
- Produces: `class ToastNotificationManager` for floating animated action alerts (`showToast(message, color)`).
- Produces: `NeoWidgets::BrutalistButton` with physical depression `(+3px, +3px)` on click and shadow collapse.

- [ ] **Step 1: Write test in `tests/test_animations.cpp` for ViewTransitionManager and ToastNotificationManager**

```cpp
#include "ui/AnimationManager.hpp"
#include "../tests/TestRunner.hpp"

int main() {
    App::UI::ViewTransitionManager vtm;
    vtm.triggerTransition();
    assert_false(vtm.isFinished(), "Transition starts active");
    vtm.update(0.15f);
    assert_true(vtm.isFinished(), "Transition finishes after 150ms");
    assert_equal(vtm.getAlpha(), 1.0f, "Alpha reaches 1.0f");

    App::UI::ToastNotificationManager tnm;
    tnm.showToast("ORDER DELETED", 0xFFBCAEFF);
    assert_true(tnm.hasActiveToasts(), "Toast is active");
    tnm.update(4.0f);
    assert_false(tnm.hasActiveToasts(), "Toast expires after dwell time");
    return 0;
}
```

- [ ] **Step 2: Run test to verify it fails (file not created yet)**

Run: `g++ -std=c++20 -Iinclude tests/test_animations.cpp -o build/test_animations.exe`
Expected: FAIL

- [ ] **Step 3: Implement `AnimationManager.hpp`, `AnimationManager.cpp`, and mechanical button displacement**

In `NeoWidgets::BrutalistButton`:
```cpp
bool clicked = ImGui::Button(label, ImVec2(actualWidth, actualHeight));
bool isActive = ImGui::IsItemActive();
bool isHovered = ImGui::IsItemHovered();

float shadowOffset = isActive ? 1.0f : (isHovered ? 5.0f : 4.0f);
ImVec2 renderOffset = isActive ? ImVec2(3.0f, 3.0f) : ImVec2(0.0f, 0.0f);
// Render tactile drop shadow and button frame using calculated offsets
```

- [ ] **Step 4: Run test to verify it passes**

Run: `g++ -std=c++20 -Iinclude -Ilibs/imgui tests/test_animations.cpp src/ui/AnimationManager.cpp libs/imgui/imgui*.cpp -o build/test_animations.exe; .\build\test_animations.exe`
Expected: PASS

- [ ] **Step 5: Commit changes**

```bash
git add include/ui/AnimationManager.hpp src/ui/AnimationManager.cpp src/ui/NeoWidgets.cpp CMakeLists.txt tests/test_animations.cpp
git commit -m "feat(ui): add mechanical button press displacement and animation management engine"
```

---

### Task 3: Order Deletion Modal, Cascading Sync & View Deletion Triggers

**Files:**
- Modify: `include/models/Customer.hpp`
- Create: `include/ui/DeleteOrderModalView.hpp`
- Create: `src/ui/DeleteOrderModalView.cpp`
- Modify: `include/ui/OrdersView.hpp`
- Modify: `src/ui/OrdersView.cpp`
- Modify: `include/ui/CustomerDetailView.hpp`
- Modify: `src/ui/CustomerDetailView.cpp`
- Modify: `CMakeLists.txt`
- Test: `tests/test_services.cpp`
- Test: `tests/test_views.cpp`

**Interfaces:**
- Produces: `Customer::removeOrderId(Core::OrderId id)`
- Produces: `class DeleteOrderModalView` with `open(orderId, customerName, totalAmount)` and callbacks `onConfirmDelete(orderId)`, `onCancel()`.
- Produces: `OrdersView::onDeleteOrderClick` callback and `[DELETE]` button in orders table.
- Produces: `CustomerDetailView::onDeleteOrderClick` callback and `[DELETE]` button in customer orders table.

- [ ] **Step 1: Write test for `Customer::removeOrderId` in `tests/test_models.cpp` and DeleteOrderModalView in `tests/test_views.cpp`**

```cpp
customer.addOrderId(5001);
customer.addOrderId(5002);
assert_true(customer.removeOrderId(5001), "Order 5001 removed from customer");
assert_equal(customer.getOrderIds().size(), static_cast<size_t>(1), "1 order remains");
```

- [ ] **Step 2: Run test to verify it fails**

Run: `.\build\test_models.exe`
Expected: FAIL

- [ ] **Step 3: Implement `removeOrderId` in `Customer.hpp`, `DeleteOrderModalView`, and update `OrdersView` & `CustomerDetailView`**

Implement `Customer::removeOrderId`:
```cpp
bool removeOrderId(Core::OrderId id) {
    auto it = std::find(m_orderIds.begin(), m_orderIds.end(), id);
    if (it != m_orderIds.end()) {
        m_orderIds.erase(it);
        return true;
    }
    return false;
}
```
Implement `DeleteOrderModalView`:
Modal with confirmation dialog, stark Neo-Brutalist border and shadow, Soft Coral `[CONFIRM DELETE]` and `[CANCEL]`.
In `OrdersView` and `CustomerDetailView`:
Widen action column to 160px; render `[EDIT]` and `[DELETE]` side-by-side using `BrutalistButton`.

- [ ] **Step 4: Run tests to verify all pass**

Run: `.\build\test_models.exe; .\build\test_views.exe`
Expected: PASS

- [ ] **Step 5: Commit changes**

```bash
git add include/models/Customer.hpp include/ui/DeleteOrderModalView.hpp src/ui/DeleteOrderModalView.cpp include/ui/OrdersView.hpp src/ui/OrdersView.cpp include/ui/CustomerDetailView.hpp src/ui/CustomerDetailView.cpp CMakeLists.txt tests/test_models.cpp tests/test_views.cpp
git commit -m "feat(orders): implement order deletion modal, customer synchronization, and UI triggers"
```

---

### Task 4: End-User Focused Dashboard Redesign

**Files:**
- Modify: `include/ui/DashboardView.hpp`
- Modify: `src/ui/DashboardView.cpp`
- Test: `tests/test_views.cpp`

**Interfaces:**
- Consumes: `Services::OrderManager`, `Services::DashboardMetrics`
- Produces: `DashboardView::render(...)` with 4 KPI cards, Primary Action Shortcuts, and Live "Recent Orders (Latest 5)" table. Developer persistence card completely removed.

- [ ] **Step 1: Write test in `tests/test_views.cpp` asserting Recent Orders table renders and no persistence string exists**

```cpp
// Assert dashboard renders recent orders
dashboardView.render(metrics, orderMgr, ...);
```

- [ ] **Step 2: Run test to verify failure with old DashboardView signature**

Run: `.\build\test_views.exe`
Expected: FAIL

- [ ] **Step 3: Rewrite `DashboardView.cpp` for end users**

- Remove lines 89-102 (`DATA STORAGE & PERSISTENCE ENGINE`).
- Render 4 large KPI stat cards (`TOTAL CUSTOMERS`, `ACTIVE ORDERS`, `COMPLETED REVENUE`, `PENDING PIPELINE`).
- Render prominent action buttons: `[+ NEW CUSTOMER]` and `[+ PLACE NEW ORDER]` with mechanical feedback.
- Render "RECENT ORDERS" table:
  - Fetches all orders, sorts/reverses to show the 5 most recent.
  - Columns: `#ID`, `Customer`, `Date`, `Items`, `Total`, `Status Badge`, `Actions (View/Edit)`.

- [ ] **Step 4: Run test to verify it passes**

Run: `.\build\test_views.exe`
Expected: PASS

- [ ] **Step 5: Commit changes**

```bash
git add include/ui/DashboardView.hpp src/ui/DashboardView.cpp tests/test_views.cpp
git commit -m "feat(dashboard): replace developer persistence box with live recent orders feed and quick actions"
```

---

### Task 5: App Integration, Toast Wiring, Full Regression & Final Verification

**Files:**
- Modify: `src/main.cpp`
- Test: All 5 test binaries (`test_models`, `test_services`, `test_storage`, `test_theme`, `test_views`, `test_animations`)
- Binary: `build/BrutalOps.exe`

**Interfaces:**
- Wires `DeleteOrderModalView`, `ViewTransitionManager`, `ToastNotificationManager`, order deletion with cascading sync and immediate disk auto-save.

- [ ] **Step 1: Wire modal, animations, and toast triggers in `src/main.cpp`**

- In order deletion callback: call `customer.removeOrderId(id)`, `orderMgr.deleteOrder(id)`, `autoSave.markDirty()`, `autoSave.forceSave()`, and trigger `toastMgr.showToast("ORDER #<ID> DELETED", 0xFFBCAEFF)`.
- Wrap view rendering in `viewTransition.getAlpha()` style variable.
- Render `toastMgr.render()` at top level.

- [ ] **Step 2: Compile application binary and run all test suites**

Run:
```powershell
cmake -B build -G "MinGW Makefiles"
cmake --build build
.\build\test_models.exe
.\build\test_services.exe
.\build\test_storage.exe
.\build\test_theme.exe
.\build\test_views.exe
.\build\test_animations.exe
```
Expected: All suites PASS with 0 errors.

- [ ] **Step 3: Verify binary execution**

Verify `build/BrutalOps.exe` launches cleanly.

- [ ] **Step 4: Commit and push changes**

```bash
git add src/main.cpp CMakeLists.txt
git commit -m "feat: integrate tactile animations, order deletion, and end-user dashboard into main app"
git push origin main
```
