# Catering Reservation Management System

A console-based C++ application for managing catering reservations with date validation, capacity control, status updates, and CSV persistence.

## ✨ Project Overview

This program lets users create and manage catering reservations through a menu-driven terminal interface. It supports:

- New reservations with date validation and package selection
- Capacity enforcement: maximum of 3 events per day and 250 total guests per day
- Reservation status management: Confirmed, Completed, Cancelled
- Search, sort, pagination, and delete operations for existing bookings
- Automatic save/load from `reservations.csv`

## ✅ Key Features

- **Smart booking limits**
    - Maximum 3 reservations per date
    - Maximum 250 guests per day
    - Minimum 10 guests per reservation
- **Date validation**
    - Rejects past dates
    - Correctly handles leap years
    - Validates month and day entry
- **Package pricing**
    - Basic: P250 per guest
    - Premium: P450 per guest
    - Luxury: P600 per guest
- **Reservation management**
    - View reservations with filtering and pagination
    - Search by Name, ID, or Date.
    - Toggle sorting between Date and ID (Ascending/Descending).
    - Mark bookings as Completed
    - Cancel reservations
    - Delete reservations
- **Persistent storage**
    - Data is saved to `reservations.csv`
    - Existing reservations are loaded automatically at startup
-  **Automated Maintenance**
    - Automatically detects past dates and updates `CONFIRMED` events to `COMPLETED` upon opening the management view.
- **Financial & Sales Reporting**
    - Aggregates revenue for `COMPLETED` events by month and year.
    - **Locale-Aware Formatting**: Displays currency with proper Philippine Peso formatting and thousands separators.
    - **Export**: Saves reports as human-readable `.txt` files.

## 📘 Variables Dictionary

| Variable / Symbol | Type | Description |
| --- | --- | --- |
| `MAX_EVENTS_PER_DAY` | `int` | Maximum bookings allowed per date (3). |
| `MAX_TOTAL_GUESTS` | `int` | Maximum total guests allowed per date (250). |
| `MIN_GUESTS_PER_EVENT` | `int` | Minimum guests required per reservation (10). |
| `PRICE_BASIC` | `double` | Cost per guest for the Basic package (P250). |
| `PRICE_PREMIUM` | `double` | Cost per guest for the Premium package (P450). |
| `PRICE_LUXURY` | `double` | Cost per guest for the Luxury package (P600). |
| `PackageType` | `enum` | Package tiers: `BASIC`, `PREMIUM`, `LUXURY`. |
| `ReservationStatus` | `enum` | Reservation states: `CONFIRMED`, `COMPLETED`, `CANCELLED`. |
| `MenuOption` | `enum` | Main menu options: `MAKE_RESERVATION`, `MANAGE_RESERVATIONS`, `SALES_REPORT`, `EXIT`. |
| `Color` | `enum` | ANSI text color codes used for terminal feedback. |
| `reservationList` | `vector<Reservation>` | In-memory list of all reservations. |
| `currentPage` | `int` | Current page index in the reservation management view. |
| `itemsPerPage` | `const int` | Number of reservations shown per page (10). |
| `searchQuery` | `string` | Current search filter used in reservation management. |

## 🏷️ Class Summary

| Class | Description |
| --- | --- |
| `Reservation` | Represents a catering reservation, including customer data, event date, package selection, guest count, total cost, and status. |

### `Reservation` Methods

| Method | Return Type | Description |
| --- | --- | --- |
| `Reservation(int resId, string name, int y, int m, int d, int pkg, int g)` | Constructor | Creates a reservation and calculates its total cost. |
| `calculateTotal()` | `void` | Computes the total reservation cost based on package rate and guest count. |
| `getName()` | `string` | Returns the customer's name. |
| `getPackage()` | `int` | Returns the package type value. |
| `getGuests()` | `int` | Returns the number of guests. |
| `getTotal()` | `double` | Returns the total cost. |
| `getId()` | `int` | Returns the reservation ID. |
| `getYear()` | `int` | Returns the event year. |
| `getMonth()` | `int` | Returns the event month. |
| `getDay()` | `int` | Returns the event day. |
| `getStatus()` | `int` | Returns the reservation status. |
| `getStatusString()` | `string` | Converts a ReservationStatus enum to a human-readable string. |
| `setStatus(ReservationStatus newStatus)` | `void` | Updates the reservation status. |
| `showReservation()` | `void` | Prints the reservation details in a formatted table row. |
| `isPastDate(y, m, d)` | `bool` | Compares the event date against a provided reference date. |
| `getTruncatedCustomerName(size_t)` | `string` | Appends `...` to names exceeding column width for UI stability. |

## 🔧 Function Summary

| Function | Return Type | Description |
| --- | --- | --- |
| `isLeapYear(unsigned int year)` | `bool` | Checks if a year is a leap year. |
| `getMaxDays(unsigned int month, unsigned int year)` | `int` | Returns the number of days in a given month, accounting for leap years. |
| `clearscreen()` | `void` | Clears the terminal screen using ANSI escape codes. |
| `clearLines(int n)` | `void` | Clears the last `n` console lines with ANSI codes. |
| `SUCCESS(string message)` | `void` | Displays a green success message. |
| `INFO(string message)` | `void` | Displays a blue informational message. |
| `WARNING(string message)` | `void` | Displays a yellow warning message. |
| `ERROR(string message)` | `void` | Displays a red error message. |
| `saveToFile()` | `void` | Saves the current reservation list to `reservations.csv`. |
| `loadFromFile()` | `void` | Loads reservations from `reservations.csv` into memory. |
| `displayMenu()` | `void` | Prints the main menu options to the console. |
| `makeReservation()` | `void` | Handles user input and validation to create a new reservation. |
| `manageReservations()` | `void` | Allows searching, pagination, status updates, cancellations, and deletion of reservations. |
| `generateSalesReport()` | `void` | Business Intelligence module with stringstream-based file export. |
| `main()` | `int` | Program entry point; initializes data and runs the main menu loop. |

## 📁 Files

- `main.cpp` — main application source code
- `reservations.csv` — stored reservation data (generated on save)
- `README.MD` — repository README file
- `Sales_Report_[Year].txt`: Generated when the user exports a financial report.

## 🛠️ Build Instructions

Use a C++ compiler such as `g++`, `clang++`, or MSVC.

### With `g++`

```bash
g++ main.cpp -o catering_system
```

### With MSVC (cl.exe)

```powershell
cl.exe /EHsc main.cpp /Fe:catering_system.exe
```

## ▶️ Run the App

### On Linux/macOS

```bash
./catering_system
```

### On Windows

```powershell
catering_system.exe
```

## 🧠 Usage Notes

- Enter `0` during reservation creation to cancel and return to the menu.
- Names cannot include commas to preserve CSV file integrity.
- The program uses ANSI color codes for success, warning, and error messages.
- Reservations are assigned IDs starting from `1001`.
- When navigating the **Manage Reservations** screen, use the single-letter commands displayed at the bottom:
  * **[N]**: Next Page.
  * **[P]**: Next Page.
  * **[S]**: Opens the search bar.
  * **[T]**: Flips the sort between Date and ID.
  * **[F]**: Marks an event as done (Completed).
  * **[D]**: Permanently removes a record.

## 👨‍💻 Author
- **Jerlo F. De Leon** 
- Computer Science Student, St. Clare College of Caloocan
-  Expected Graduation: 2026
