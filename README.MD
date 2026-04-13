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
  - Mark bookings as Completed
  - Cancel reservations
  - Delete reservations
- **Persistent storage**
  - Data is saved to `reservations.csv`
  - Existing reservations are loaded automatically at startup

## � Variables Dictionary

| Variable | Type | Description |
| --- | --- | --- |
| `MAX_EVENTS_PER_DAY` | `int` | Maximum number of events allowed per date (3) |
| `MAX_TOTAL_GUESTS` | `int` | Maximum total guests allowed per date (250) |
| `MIN_GUESTS_PER_EVENT` | `int` | Minimum guests required per reservation (10) |
| `PRICE_BASIC` | `double` | Cost per guest for the Basic package (P250) |
| `PRICE_PREMIUM` | `double` | Cost per guest for the Premium package (P450) |
| `PRICE_LUXURY` | `double` | Cost per guest for the Luxury package (P600) |
| `PackageType` | `enum` | Package tier options: BASIC, PREMIUM, LUXURY |
| `ReservationStatus` | `enum` | Reservation lifecycle states: CONFIRMED, COMPLETED, CANCELLED |
| `MenuOption` | `enum` | Main menu actions: MAKE_RESERVATION, MANAGE_RESERVATIONS, EXIT |
## 🏷️ Class Summary

| Class | Description |
| --- | --- |
| `Reservation` | Represents a catering reservation and stores details such as customer name, date, package type, guest count, total cost, and status. |

### `Reservation` Methods

| Method | Return Type | Description |
| --- | --- | --- |
| `Reservation(int id, string name, int y, int m, int d, int pkg, int g)` | `Reservation` | Creates a reservation and calculates total cost. |
| `calculateTotal()` | `void` | Computes total cost using package pricing and guest count. |
| `getName()` | `string` | Returns customer name. |
| `getPackage()` | `int` | Returns selected package type. |
| `getGuests()` | `int` | Returns guest count. |
| `getTotal()` | `double` | Returns total reservation cost. |
| `getId()` | `int` | Returns reservation ID. |
| `getYear()` | `int` | Returns event year. |
| `getMonth()` | `int` | Returns event month. |
| `getDay()` | `int` | Returns event day. |
| `getStatus()` | `string` | Returns reservation status as text. |
| `setStatus(ReservationStatus newStatus)` | `void` | Updates reservation status. |
| `showReservation()` | `void` | Prints reservation details in table format. |

## 🔧 Function Summary

| Function | Return Type | Description |
| --- | --- | --- |
| `isLeapYear(unsigned int year)` | `bool` | Checks if a year is a leap year. |
| `getMaxDays(unsigned int month, unsigned int year)` | `int` | Returns days in a month, accounting for leap years. |
| `clearscreen()` | `void` | Clears the terminal display with ANSI codes. |
| `SUCCESS(string message)` | `void` | Displays a green success message. |
| `INFO(string message)` | `void` | Displays an informational message. |
| `WARNING(string message)` | `void` | Displays a yellow warning message. |
| `ERROR(string message)` | `void` | Displays a red error message. |
| `saveToFile()` | `void` | Writes reservations to `reservations.csv`. |
| `loadFromFile()` | `void` | Reads reservations from `reservations.csv`. |
| `displayMenu()` | `void` | Prints the main menu options. |
| `makeReservation()` | `void` | Collects user input and creates a new reservation. |
| `manageReservations()` | `void` | Shows reservation list and allows status updates, search, paging, and deletion. |
| `main()` | `int` | Program entry point; loads data and runs the main menu loop. |
## �📁 Files

- `main.cpp` — main application source code
- `reservations.csv` — stored reservation data (generated on save)
- `README.MD` — repository README file

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

## 👨‍💻 Author

Built from the provided `main.cpp` source code by Jerlo F. De Leon.
