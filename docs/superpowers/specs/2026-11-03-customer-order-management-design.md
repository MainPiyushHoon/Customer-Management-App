# Design Specification: Neo-Brutalist Customer & Order Management Desktop App (C++)

- **Date:** 2026-11-03
- **Language / Standard:** C++20
- **Compiler:** GCC 14.2.0 (MSYS2 UCRT64) / MinGW-w64
- **UI Framework:** Dear ImGui with GLFW + OpenGL3 backend
- **Theme:** Dual Neo-Brutalist (Light Neo-Brutalism & Dark Cyber Neo-Brutalism) with runtime toggle
- **Persistence:** Local JSON files + Native Excel (.xlsx / CSV) workbooks with 60-second periodic auto-save and on-close flush

---

## 1. System Overview & Key Requirements

The application is a standalone, high-performance C++ desktop application designed with a bold Neo-Brutalist aesthetic. It manages customers, editable dynamic measurements, and customer order line items.

### Core Modules
1. **Customer Management:**
   - Unique Customer ID
   - Full Name
   - Phone Number (with format validation)
   - Dynamic Measurements: Key-value pairs (e.g., Chest: 38", Waist: 32", Inseam: 30", or custom user-defined labels). Fully editable: add, modify, or delete measurement rows on the fly.
   - Associated Orders collection.
2. **Order Management:**
   - Order ID (auto-incremented or formatted `ORD-XXXXX`)
   - Customer Association (Customer ID & Name)
   - Order Date & Timestamp
   - Line Items: List of items with `Item Name`, `Quantity`, and `Unit Price`
   - Order Total (computed automatically)
   - Order Status (`Pending`, `In Progress`, `Completed`, `Delivered`)
   - Notes / Special Instructions
3. **Neo-Brutalist UI & Theme Engine:**
   - High-contrast, punchy visual style: 3px solid borders, 0 corner radius (pure rectangular geometry), and hard 4px drop-shadow offsets (`#000000`).
   - Dual theme support:
     - **Light Neo-Brutalism:** Canvas `#FFFDF8`, Cards `#FFFFFF`, Electric Yellow `#FFE600`, Hot Pink `#FF6EA7`, Cyber Cyan `#00F0FF`, Pitch Black borders `#000000`.
     - **Dark Neo-Brutalism:** Canvas `#121214`, Cards `#1A1A1E`, Neon Lime `#00FF85`, Vivid Magenta `#FF007A`, Electric Yellow `#FFEA00`, Crisp White/Light borders `#FFFFFF`.
4. **Data Persistence & Auto-Save:**
   - Dual output: Structured JSON (`data/customers.json`, `data/orders.json`) and Excel sheets (`data/records.xlsx` / `records_export.csv`).
   - Background periodic auto-save every 60 seconds with on-screen visual countdown / status pill.
   - Instant save on user action and graceful flush on application exit.

---

## 2. Modular Architecture & Directory Structure

```
d:/Games/Programs/Cpp Projects/App/
├── CMakeLists.txt                  # CMake build configuration
├── README.md                       # Project overview and build instructions
├── assets/                         # Fonts, icons, and theme assets
├── data/                           # Auto-saved JSON & Excel files
│   ├── customers.json
│   ├── orders.json
│   └── records_export.csv
├── docs/
│   └── superpowers/specs/
│       └── 2026-11-03-customer-order-management-design.md
├── include/
│   ├── core/
│   │   ├── Config.hpp              # Global constants, colors, versioning
│   │   └── Types.hpp               # Common typedefs, timestamps, enums
│   ├── models/
│   │   ├── Customer.hpp            # Customer entity
│   │   ├── Measurement.hpp         # Dynamic key-value measurement entity
│   │   ├── Order.hpp               # Order aggregate root
│   │   └── OrderItem.hpp           # Order line item entity
│   ├── services/
│   │   ├── CustomerManager.hpp     # CRUD operations for Customers
│   │   ├── OrderManager.hpp        # CRUD operations for Orders
│   │   └── DashboardStats.hpp      # Aggregate metrics computation
│   ├── storage/
│   │   ├── IStorage.hpp            # Storage interface definition
│   │   ├── JsonStorage.hpp         # JSON serialization & deserialization
│   │   ├── ExcelStorage.hpp        # Excel / CSV workbook generator
│   │   └── AutoSaveCoordinator.hpp # 60s timer + manual + exit flush
│   └── ui/
│       ├── ThemeManager.hpp        # Neo-Brutalist color palettes & style vars
│       ├── NeoWidgets.hpp          # Custom brutalist buttons, cards, drop-shadow helpers
│       ├── DashboardView.hpp       # Summary metrics header cards
│       ├── CustomerListView.hpp    # Search, filter, and customer table
│       ├── CustomerDetailView.hpp  # Customer profile, dynamic measurements editor
│       └── OrderModalView.hpp      # Create / Edit order modal with line items
├── src/
│   ├── main.cpp                    # Application entry point, GLFW + ImGui setup
│   ├── models/
│   │   ├── Customer.cpp
│   │   ├── Measurement.cpp
│   │   ├── Order.cpp
│   │   └── OrderItem.cpp
│   ├── services/
│   │   ├── CustomerManager.cpp
│   │   ├── OrderManager.cpp
│   │   └── DashboardStats.cpp
│   ├── storage/
│   │   ├── JsonStorage.cpp
│   │   ├── ExcelStorage.cpp
│   │   └── AutoSaveCoordinator.cpp
│   └── ui/
│       ├── ThemeManager.cpp
│       ├── NeoWidgets.cpp
│       ├── DashboardView.cpp
│       ├── CustomerListView.cpp
│       ├── CustomerDetailView.cpp
│       └── OrderModalView.cpp
└── third_party/                    # Dear ImGui, GLFW, and single-header JSON
    ├── imgui/
    ├── glfw/
    └── json.hpp
```

---

## 3. Data Models Specification

### 3.1. `Measurement`
- `std::string key`: Name of measurement (e.g., "Chest", "Waist", "Inseam", "Shoulder Width", "Sleeve")
- `std::string value`: Formatted value (e.g., `38.5"`, `82 cm`, `Regular`)
- `std::string unit`: Optional unit label (e.g., `in`, `cm`)

### 3.2. `Customer`
- `uint64_t id`: Unique customer ID
- `std::string name`: Customer full name
- `std::string phone`: Customer phone number
- `std::vector<Measurement> measurements`: Dynamic editable key-value measurements list
- `std::vector<uint64_t> orderIds`: References to placed orders
- `std::string createdAt`: ISO-8601 creation timestamp
- `std::string updatedAt`: ISO-8601 last modified timestamp

### 3.3. `OrderItem`
- `std::string itemName`: Item title / description
- `int quantity`: Quantity ordered (default: 1)
- `double unitPrice`: Price per unit
- `double getTotalPrice() const`: `quantity * unitPrice`

### 3.4. `Order`
- `uint64_t id`: Unique order ID
- `uint64_t customerId`: Associated customer ID
- `std::string customerName`: Cached customer name for fast lookup
- `std::string orderDate`: Date string (YYYY-MM-DD HH:MM)
- `std::vector<OrderItem> items`: Detailed line items
- `OrderStatus status`: `Pending`, `InProgress`, `Completed`, `Delivered`, `Cancelled`
- `std::string notes`: Free-text notes or special tailoring/delivery instructions
- `double getTotalAmount() const`: Sum of item totals

---

## 4. UI & Neo-Brutalist Design System

### 4.1. Neo-Brutalism Principles
- **Geometry:** 0.0f rounding everywhere. Hard, unapologetic corners.
- **Borders:** 2.5f to 3.0f thick pitch-black (`#000000`) or crisp white borders.
- **Drop Shadows:** Custom offset filled rectangles (4px X-offset, 4px Y-offset) rendered behind cards, buttons, and modals without blur.
- **Micro-Interactions:** When clicked, buttons shift +2px right and +2px down to simulate a tactile physical press.

### 4.2. Palette Specifications
| Element | Light Neo-Brutalist | Dark Cyber Brutalist |
|---|---|---|
| Background Canvas | `#FFFDF8` (Off-white / Sand) | `#101114` (Deep Charcoal) |
| Card / Panel Surface | `#FFFFFF` (Pure White) | `#18191E` (Dark Surface) |
| Primary Accent | `#FFE600` (Electric Yellow) | `#FFE600` (Electric Neon Yellow) |
| Secondary Accent | `#FF6EA7` (Neo Hot Pink) | `#FF007A` (Cyber Magenta) |
| Action / Success | `#00E599` (Electric Mint) | `#00FF85` (Neon Green) |
| Info / Accent | `#00D8F6` (Cyber Cyan) | `#00E5FF` (Vivid Cyan) |
| Text Primary | `#000000` (Deep Black) | `#F5F5F7` (Crisp Off-White) |
| Border | `#000000` (Solid Black) | `#E0E0E6` (Crisp Light Border) |
| Shadow Color | `#000000` (Solid Black) | `#000000` (Solid Black) |

---

## 5. Storage & Auto-Save Specifications

1. **JSON Storage:**
   - Saved under `data/customers.json` and `data/orders.json`.
   - Serialized cleanly using standard JSON format with 2-space indentation.
2. **Excel Storage:**
   - Written to `data/records_export.csv` and XML-based `.xlsx` format.
   - Sheet 1: Customers (Columns: Customer ID, Name, Phone, Measurements Summary, Orders Count, Created At).
   - Sheet 2: Orders (Columns: Order ID, Customer ID, Customer Name, Date, Items Summary, Total Amount, Status, Notes).
3. **Auto-Save Engine:**
   - Evaluates elapsed time on every frame; triggers serialization asynchronously or non-blockingly every 60 seconds.
   - Provides a visual status badge: `● AUTO-SAVED (48s)`.
   - Manual "SAVE NOW" button forces immediate disk synchronization.
   - Window close event interceptor guarantees zero data loss on exit.

---

## 6. Verification and Build Plan

- Setup CMake 3.20+ with MinGW / GCC 14.2.0.
- Vendored Dear ImGui (v1.91+) with standard GLFW/OpenGL3 bindings.
- Validate compilation and launch directly on the user's Windows environment.
