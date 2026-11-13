# BRUTAL // OPS — Customer & Order Management Desktop App

A high-performance, object-oriented desktop application programmed in **C++20** featuring a bold **Neo-Brutalist** visual style (with both Light and Dark Cyber modes), dynamic customer measurements, detailed order line-item tracking, and dual **JSON & Excel** persistence with 60-second background auto-save.

---

## 📁 Modular Architecture & Structure

```
d:/Games/Programs/Cpp Projects/App/
├── CMakeLists.txt                  # Build configuration (C++20, MinGW / MSYS2)
├── README.md                       # Documentation & build instructions
├── data/                           # Auto-saved JSON & Excel persistence files
│   ├── customers.json
│   ├── orders.json
│   └── records_export.csv
├── docs/
│   └── superpowers/specs/         # Design specification document
│       └── 2026-11-03-customer-order-management-design.md
├── include/
│   ├── core/
│   │   ├── Config.hpp              # App defaults, paths, constants
│   │   └── Types.hpp               # IDs, enums (OrderStatus), helpers
│   ├── models/
│   │   ├── Customer.hpp            # Customer entity (Name, Phone, Measurements, Orders)
│   │   ├── Measurement.hpp         # Dynamic key-value measurement entity (editable)
│   │   ├── Order.hpp               # Order entity (Date, line items, totals, status)
│   │   └── OrderItem.hpp           # Order line item entity (Name, Qty, Price)
│   ├── services/
│   │   ├── CustomerManager.hpp     # Customer CRUD and search
│   │   ├── OrderManager.hpp        # Order CRUD and status filtering
│   │   └── DashboardStats.hpp      # Live KPI metrics calculation
│   ├── storage/
│   │   ├── IStorage.hpp            # Storage interface definition
│   │   ├── JsonStorage.hpp         # JSON serialization & deserialization
│   │   ├── ExcelStorage.hpp        # Excel (.xlsx/CSV) exporter
│   │   └── AutoSaveCoordinator.hpp # 60s periodic timer & on-close flush
│   └── ui/
│       ├── ThemeManager.hpp        # Neo-brutalist Light & Dark themes
│       ├── NeoWidgets.hpp          # Custom brutalist buttons, cards, drop-shadows
│       ├── DashboardView.hpp       # Top header, KPI cards, auto-save countdown
│       ├── CustomerListView.hpp    # Customer search & table
│       ├── CustomerDetailView.hpp  # Editable measurements & customer orders
│       └── OrderModalView.hpp      # Modal dialog for creating/editing orders
├── src/                            # Implementation files
└── third_party/                    # Dear ImGui, GLFW, and JSON library
```

---

## ⚡ Key Features

1. **Customer Management:**
   - Store Customer Name, Phone Number, and associated Orders.
   - **Dynamic Measurements:** Fully editable key-value pairs (e.g. Chest: 38", Waist: 32", Inseam: 30", or custom labels). Add, edit, or delete measurements on the fly.
2. **Order Management:**
   - Link orders to individual customers.
   - Store Order Date, detailed line items (Item Name, Quantity, Unit Price, and Auto-calculated total), Order Status (`Pending`, `In Progress`, `Completed`, `Delivered`), and Notes.
3. **Neo-Brutalist Theme:**
   - High-contrast, sharp aesthetic with 3px solid black borders and 4px hard drop shadows.
   - Instant toggle between **Light Neo-Brutalism** and **Cyber Dark Neo-Brutalism**.
4. **Dual Persistence & Auto-Save:**
   - Auto-saves every 60 seconds (with live status countdown) to both JSON and Excel format.
   - Manual instant save on button click.
   - Guaranteed flush to Excel and JSON on application exit.
