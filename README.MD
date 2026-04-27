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
- **Context-Aware Dashboard**
    - **Upcoming View**: Automatically prioritizes the soonest events to help management prepare for immediate tasks.
    - **Archive View**: Isolates past reservations for financial auditing and historical lookups.
    - **Smart Sorting**: Uses a "Pivot Logic" to sort future dates ascending and historical dates descending by default.
**Advanced Management Tools**
    - **View Toggle**: Instantly switch between active bookings and archived history.
    - **Reverse Sort**: A manual override to flip chronological order in any view.
    - **Contextual Search**: Perform case-insensitive searches that respect the current view (Upcoming/Archive).
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
| `eventCount` | `int` | Number of events for a selected date (capacity check). |
| `currentDailyGuests` | `int` | Total guests already booked for a selected date. |
| `remaining` | `int` | Remaining guest capacity for a date. |
| `newId` | `int` | Auto-incremented reservation ID. |
| `confirmedRevenue` | `double` | Revenue from confirmed reservations (dashboard/statistics). |
| `completedRevenue` | `double` | Revenue from completed reservations (dashboard/statistics). |
| `monthlyRevenue` | `double[13]` | Monthly revenue for sales report (index 1-12). |
| `monthlyCount` | `int[13]` | Monthly completed event count for sales report (index 1-12). |
| `totalYearly` | `double` | Total annual revenue for sales report. |
| `totalEvents` | `int` | Total completed events in a year (sales report). |
| `todayVal` | `long` | Numeric representation of today’s date for sorting. |
| `sortByDate` | `bool` | Whether sorting is by date or by reservation ID. |
| `reservationList` | `vector<Reservation>` | In-memory list of all reservations. |
| `currentPage` | `int` | Current page index in the reservation management view. |
| `itemsPerPage` | `const int` | Number of reservations shown per page (10). |
| `searchQuery` | `string` | Current search filter used in reservation management. |
| `showArchive` | `bool` | Toggle state for viewing past history vs. upcoming events. |
| `isReverse` | `bool` | Toggle state for reversing the chronological sort order. |
| `startIdx` | `int` | Pointer for pagination logic (tracks the first item displayed on the current page). |

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
| `getStatusString(int status)` | `string` | Converts a ReservationStatus enum value to a human-readable string. |
| `setStatus(ReservationStatus newStatus)` | `void` | Updates the reservation status. |
| `showReservation()` | `void` | Prints the reservation details in a formatted table row. |
| `isPastDate(int currentYear, int currentMonth, int currentDay)` | `bool` | Returns true if the reservation date is before the given reference date. |
| `getTruncatedCustomerName(size_t width)` | `string` | Returns the customer name, truncated with `...` if it exceeds the specified width. |

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
| `handleInputError(msg, n)` | `void` | Unified utility to reset buffer, show error, and clear lines. |
| `reservationListSort(...)` | `void` | Context-aware sorting engine for Upcoming/Archive views. |
| `handleSearch(...)` | `void` | View-locked search engine for case-insensitive name filtering. |
| `handleFinishEvent()` | `void` | Marks a reservation as completed by ID, with input validation and confirmation. |
| `handleCancelEvent()` | `void` | Cancels a reservation by ID, with input validation and confirmation. |
| `handleDeleteEvent()` | `void` | Permanently deletes a reservation by ID, with input validation and confirmation. |
| `autoCompletePastReservations()` | `int` | Automatically marks past confirmed reservations as completed based on the current date. Returns the number of updated reservations. |
| `displayMenu()` | `void` | Displays the main menu and navigation options for the system. |

### 📂 Core Modules
| Function | Return Type | Description |
| --- | --- | --- |
| `saveToFile()` | `void` | Serializes the vector to `reservations.csv`. |
| `loadFromFile()` | `void` | Deserializes CSV data into the application memory on startup. |
| `makeReservation()` | `void` | Logic-gate for new bookings (Date, Capacity, and Guest validation). |
| `manageReservations()` | `void` | The main dashboard controller; manages view states and user commands. |
| `generateSalesReport()` | `void` | Business Intelligence module that exports financial performance to `.txt`. |

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
  * **[N/P]**: Page Navigation.
  * **[S]**: Search (Filter names within the current view).
  * **[T]**: Toggle View (Switch between Upcoming and Archive)
  * **[R]**: Reverse Sort (Flip Ascending/Descending).
  * **[F]**: Marks an event as done (Completed).
  * **[D]**: Permanently removes a record.
  * **[B]**: Back to Main Menu.

## 👨‍💻 Author
- **Jerlo F. De Leon** 
- Computer Science Student, St. Clare College of Caloocan
-  Expected Graduation: 2026
