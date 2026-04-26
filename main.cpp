/**
 * @file main.cpp
 * @brief Console-based Catering Reservation Management System.
 *
 * @details
 * This program allows users to:
 * - Create catering reservations
 * - Manage and update reservation status
 * - View reservations with sorting, filtering, and pagination
 * - Persist data using CSV file storage
 *
 * Core Features:
 * - Input validation for dates and guest limits
 * - Capacity management per day
 * - Revenue tracking (Confirmed vs Completed)
 * - ANSI-colored terminal UI feedback
 *
 * @author Jerlo F. De Leon
 * @date 2026
 *
 * @note This is a beginner-friendly implementation focusing on clarity,
 *       modularity, and practical file handling.
 */

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <locale>
using namespace std;

// ==========================================================
// ================ GLOBAL CONSTANTS & ENUMS =================
// ==========================================================

/**
 * @defgroup constraints System Constraints
 * @brief Operational limits and pricing tiers for the catering system.
 */
/// @{
const int MAX_EVENTS_PER_DAY = 3;     ///< The maximum number of separate bookings allowed on a single calendar date.
const int MAX_TOTAL_GUESTS = 250;     ///< The upper limit for the sum of guests across all events in one day.
const int MIN_GUESTS_PER_EVENT = 10;   ///< The minimum headcount required to accept a single reservation.

const double PRICE_BASIC = 250.0;     ///< Cost per head for the Basic package (Standard service).
const double PRICE_PREMIUM = 450.0;   ///< Cost per head for the Premium package (Enhanced menu).
const double PRICE_LUXURY = 600.0;    ///< Cost per head for the Luxury package (Full service & premium menu).
/// @}

// --- End of Global Constants ---

/**
 * @enum PackageType
 * @brief Types of catering packages available.
 *
 * @details
 *  Defines pricing tiers and service levels for reservations.
 * -BASIC: Affordable option with standard menu and service.
 * -PREMIUM: Mid-tier option with enhanced menu and service.
 * -LUXURY: High-end option with full-service and premium offerings.
 * @note This enum is used to categorize reservations and calculate total costs based on the selected package type. Each package type corresponds to a specific price per guest, which is defined in the global constants. The package type is an essential attribute of a reservation, as it directly impacts the total cost and the level of service provided to the customer.
 */
enum PackageType { 
    BASIC = 1,   ///< Basic package (affordable, standard service)
    PREMIUM,     ///< Premium package (enhanced menu and service)
    LUXURY       ///< Luxury package (full-service, high-end offering)
};


/**
 * @enum ReservationStatus
 * @brief Represents the current state of a reservation.
 * @note This enum is used to track the lifecycle of a reservation, allowing the system to differentiate between active bookings, completed events, and cancellations. It is essential for managing reservations effectively and providing accurate information to users when viewing or updating reservation details.
 */
enum ReservationStatus {
    CONFIRMED = 1,  ///< Reservation is confirmed and active
    COMPLETED,      ///< Event has been successfully completed
    CANCELLED       ///< Reservation has been cancelled
};

/**
 * @brief Simple structure to hold date components.
 */
struct Date {
    int year;
    int month;
    int day;
};


/**
 * @enum MenuOption
 * @brief Main menu navigation options.
 * @note Used for user interaction in the main menu of the application.
 */
enum MenuOption { 
    MAKE_RESERVATION = 1,  ///< Create a new reservation
    MANAGE_RESERVATIONS,   ///< View or modify existing reservations
    SALES_REPORT,
    EXIT                   ///< Exit the application
};

/**
 * @enum Color
 * @brief ANSI color codes for terminal output.
 *
 * @note Used for styling console text output.
 */
enum Color {
    RED = 31,       ///< Red text
    GREEN = 32,     ///< Green text
    YELLOW = 33,    ///< Yellow text
    BLUE = 34,      ///< Blue text
    MAGENTA = 35,   ///< Magenta text
    CYAN = 36,      ///< Cyan text
    WHITE = 37      ///< White text
};

// --- End of Global Constants & Enums ---

// --- Helper Functions ---

/**
 * @brief Determines if a given year is a leap year.
 * @param year The year to check for leap year status.
 * @details 
 * A leap year is defined as:
 * - It is divisible by 4 and not divisible by 100, OR
 * - It is divisible by 400.
 * This function returns true if the year is a leap year, and false otherwise.
 * @note This is used to accurately determine the number of days in February when validating dates for reservations.
 * @warning The function assumes that the input year is a positive integer. Negative years or zero are not valid inputs for leap year calculations.
 * @return bool True if the year is a leap year, false otherwise.
 */
bool isLeapYear(unsigned int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/**
 * @brief Returns the maximum number of days in a given month and year.
 * @param month The month (1-12) for which to determine the maximum days.
 * @param year The year to consider for leap year calculations (affects February).
 * @details
 * The function uses a predefined array of days for each month, with February set to 28 days by default. If the month is February and the year is a leap year, it returns 29 days. For invalid month values (less than 1 or greater than 12), it returns 0 to indicate an error.
 * @note This function is essential for validating user input when creating reservations, ensuring that the day entered is valid for the specified month and year. It accounts for leap years to provide accurate validation for February.
 * 
 * @return int The maximum number of days in the specified month and year, or 0 if the month is invalid.
 */
int getMaxDays(unsigned int month, unsigned int year) {
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; ///< Array representing the number of days in each month, where index 1 corresponds to January, index 2 to February, and so on. The value for February is set to 28 by default and is adjusted to 29 if the year is a leap year.
    if (month == 2 && isLeapYear(year)) return 29;
    if (month < 1 || month > 12) return 0;
    return daysInMonth[month];
}

/**
 * @brief Clears the console screen and resets the cursor to the top-left corner.
 * @details
 * This function uses ANSI escape codes to clear the terminal screen and move the cursor to the home
 * position (1;1). It is a cross-platform method that works in most modern terminal environments, including Windows 10+ (with ANSI support enabled), Linux, and macOS. The function does not take any parameters and does not return any value. It is typically called before displaying new content to ensure a clean and organized user interface.
 * @note In environments that do not support ANSI escape codes, this function may not work as intended. In such cases, alternative methods may be required to clear the screen.
 * @warning This function should be used with caution in environments where ANSI codes are not supported, as it may result in unexpected output rather than clearing the screen.
 * @return void
 */
void clearscreen() {  cout << "\033[2J\033[1;1H"; }

/**
 * @brief Clears a specified number of lines from the console output.
 * 
 * @param n The number of lines to clear from the console.
 * @details
 * This function uses ANSI escape codes to move the cursor up and clear lines in the terminal. It iterates 'n' times, moving the cursor up one line and clearing that line each time. This is useful for removing previous output from the console, such as error messages or prompts, without clearing the entire screen. The function does not return any value and is intended to be used in scenarios where you want to update the console output dynamically while keeping the user interface clean.
 * 
 * @note In environments that do not support ANSI escape codes, this function may not work as intended. It is designed for use in modern terminal environments that support ANSI codes.
 */
void clearLines(int n) {
    for (int i = 0; i < n; ++i) {
        cout << "\033[1A\033[2K"; // Move cursor up and clear line
    }
}

// --- Colored Output Functions ---

/**
 * @brief Displays a success message in green color.
 * @param message The message to display. 
 * @details
 * The message is prefixed with "[SUCCESS]" to indicate that it is a successful operation or positive outcome. The text is styled in green to visually differentiate it from other types of messages, such as warnings or errors.
 * @note This function is intended to provide clear feedback to the user when an action has been completed successfully. It can be used throughout the application to maintain a consistent style for success messages.
 * @warning The function assumes that the terminal supports ANSI escape codes for coloring text. In environments that do not support ANSI codes, the color formatting may not work as intended, and the message will be displayed without color.
 * @return void
 */
void SUCCESS(string message) {cout << "\033[1;32m[SUCCESS] " << message << "\033[0m" << endl;}

/**
 * @brief Displays an informational message in blue color.
 * @param message The message to display. 
 * @details
 * The message is prefixed with "[INFO]" to indicate that it is informational in nature, providing context or details about the current state of the application or an operation.
 * @note This function is used to convey general information to the user, such as the current date, status updates, or other non-critical messages. The blue color helps to visually distinguish informational messages from warnings (yellow) and errors (red).
 */
void INFO(string message) {cout << "\033[1;34m[INFO] " << message << "\033[0m" << endl;}

/**
 * @brief Displays a warning message in yellow color.
 * @param message The message to display. 
 * @details
 * The message is prefixed with "[WARN]" to indicate that it is a warning.
 * @note This function is used to alert the user about potential issues, such as approaching capacity limits, invalid input that can be corrected, or other non-critical problems that require attention. The yellow color is chosen to draw attention without indicating a critical error.
 */
void WARNING(string message) {
    cout << "\033[1;33m[WARN] " << message << "\033[0m" << endl;
}

/**
 * @brief Displays an error message in red color.
 * @param message The message to display.
 * @details 
 * The message is prefixed with "[ERROR]" to indicate that it is an error.
 * @note This function is used to inform the user about critical issues, such as invalid input that cannot be processed, system errors, or other problems that require immediate attention. The red color is chosen to signify the severity of the message and to prompt the user to take corrective action.
 */
void ERROR(string message) {
    cout << "\033[1;31m[ERROR] " << message << "\033[0m" << endl;
}
// --- End of Colored Output Functions ---

/**
 * @brief Helper function to fetch the current system date.
 * @return A Date struct containing current year, month, and day.
 */
Date getCurrentDate() {
    time_t t = time(0);
    tm* now = localtime(&t);
    return { now->tm_year + 1900, now->tm_mon + 1, now->tm_mday };
}

/**
 * @brief Handles the entire error sequence: Resets input, alerts user, 
 * waits for Enter, and cleans up the UI.
 * @param message The error text to display.
 * @param linesToClear How many lines to wipe from the terminal (default is 3).
 */
void handleInputError(string message, int linesToClear = 3) {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    ERROR(message);
    cout << "Press Enter to try again...";
    cin.get(); 
    clearLines(linesToClear);
}


// --- End of Helper Functions ---

// --- The Reservation Class ---

/**
 * @class Reservation
 * @brief Represents a catering reservation.
 *
 * @details
 * The Reservation class encapsulates all details of a catering reservation,
 * including customer information, event date, package type, number of guests,
 * total cost, and reservation status.
 *
 * It provides methods for:
 * - Calculating total cost based on selected package
 * - Accessing reservation details (getters)
 * - Displaying formatted reservation data
 *
 * @note The class does not include any input validation or error handling. It assumes that all data provided to the constructor is valid and correctly formatted. It is the responsibility of the caller to ensure that the data is valid before creating a Reservation object.
 * @warning The class does not handle input validation internally; it assumes that all data provided to the constructor is valid. Validation should be performed before creating a Reservation object.
 */
class Reservation {
private:
    int id; ///< Unique reservation ID
    string customerName; ///< Customer name for the reservation
    int year, month, day; ///< Event date (year, month, day)
    int packageType; ///< Package type (BASIC, PREMIUM, LUXURY)
    int guests; ///< Number of guests for the reservation
    double totalCost; ///< Total cost of the reservation, calculated based on package type and number of guests
    ReservationStatus status; ///< Current status of the reservation (CONFIRMED, COMPLETED, CANCELLED)

public:
    /**
     * @brief Constructs a new Reservation object
     * 
     * @param resId Unique reservation ID
     * @param name Customer name
     * @param y Year of event
     * @param m Month of event
     * @param d Day of event
     * @param pkg Selected package type
     * @param g Number of guests
     * 
     * @details
     * Initializes a new reservation with the provided details and calculates the total cost based on the package type and number of guests. The reservation status is set to CONFIRMED by default.
     * @note The constructor does not perform any validation on the input parameters. It assumes that all provided data is valid and correctly formatted. Validation should be handled separately before creating a Reservation instance.
     * @warning If invalid data is passed to the constructor (e.g., negative guests, invalid package type), it may lead to incorrect total cost calculations or inconsistent reservation states. It is the responsibility of the caller to ensure that the data is valid before instantiating a Reservation object.   
     */
    Reservation(int resId, string name, int y, int m, int d, int pkg, int g) 
        : id(resId), customerName(name), year(y), month(m), day(d), 
          packageType(pkg), guests(g), status(CONFIRMED) {
        calculateTotal();
    }

    /**
     * @brief Calculates total reservation cost
     * 
     * Determines the rate based on package type and multiplies
     * it by the number of guests.
     * 
     * @note This function is called during the construction of a Reservation object to initialize the total cost. It uses the global constants for pricing based on the package type. If an invalid package type is provided, it defaults to a rate of 0.0, which may result in a total cost of 0.0. It is important to ensure that the package type is valid when creating a reservation to avoid incorrect cost calculations.
     */
    void calculateTotal() {
        double rate; ///< The cost per guest based on the selected package type. This variable is determined by a switch statement that checks the packageType field of the reservation. Depending on whether the package is BASIC, PREMIUM, or LUXURY, the rate is set to the corresponding price defined in the global constants. If an invalid package type is provided, the rate defaults to 0.0, which will result in a total cost of 0.0 when multiplied by the number of guests.
        switch (packageType) {
            case BASIC:   rate = PRICE_BASIC; break;
            case PREMIUM: rate = PRICE_PREMIUM; break;
            case LUXURY:  rate = PRICE_LUXURY; break;
            default:      rate = 0.0; break;
        }
        totalCost = rate * guests;
    }

    // Getters for File I/O
    string getName() const { return customerName; } ///< Returns the customer's name associated with the reservation.
    int getPackage() const { return packageType; } ///< Returns the package type associated with the reservation.
    int getGuests() const { return guests; } ///< Returns the number of guests for the reservation.
    double getTotal() const { return totalCost; } ///< Returns the total cost of the reservation.

    // Getters
    int getId() const { return id; } ///< Returns the unique ID of the reservation.
    int getYear() const { return year; } ///< Returns the year of the event.
    int getMonth() const { return month; } ///< Returns the month of the event.
    int getDay() const { return day; } ///< Returns the day of the event.
    
    /**
     * @brief Formats the customer name to fit within a specific UI column width.
     * * @details 
     * To prevent table misalignment in the dashboard, this method checks the length 
     * of the customer's name. If the name exceeds the provided width, it truncates 
     * the string and appends an ellipsis (...) to indicate that the text continues.
     * * @param width The maximum number of characters allowed for the column.
     * @return A string containing either the full name or a truncated version ending in "...".
     * * @example 
     * If customerName is "Mark Anthony Garcia" and width is 15, 
     * the output will be "Mark Anthony...".
     */
    string getTruncatedCustomerName(size_t width) const {
        if (customerName.length() > width) {
            // Substring to width-3 and add "..."
            return customerName.substr(0, width - 3) + "...";
        }
        return customerName;
    }

    int getStatus() const { return status;} ///< Returns the status of the event.
    
    /**
     * @brief Converts a ReservationStatus enum to a human-readable string.
     * @param status The status to convert.
     * @return A string representation ("Confirmed", "Completed", etc.)
     */
    string getStatusString(int status) const {
        switch(status) {
            case CONFIRMED: return "Confirmed";
            case COMPLETED: return "Completed";
            case CANCELLED: return "Cancelled";
            default:        return "Unknown";
        }
    }
    
    // Setters
    /**
     * @brief Updates the reservation status.
     * @param newStatus The new status to set for the reservation.
     * 
     * @details
     * This function allows updating the status of a reservation to reflect changes such as completion or cancellation. The status is updated based on the provided ReservationStatus enum value. It is important to ensure that the new status is valid and appropriate for the current state of the reservation (e.g., you should not mark a cancelled reservation as completed). The function does not perform any validation on the new status, so it is the responsibility of the caller to ensure that the status transition is logical and consistent with the reservation's lifecycle.
     * 
     * @note The function does not handle any side effects of changing the reservation status, such as updating revenue calculations or notifying users. It simply updates the status field of the Reservation object. Additional logic may be needed in the calling code to handle the implications of a status change.
     * @return void
     */
    void setStatus(ReservationStatus newStatus) { status = newStatus; }

    /**
     * @brief Displays reservation details in table format.
     * @details
     * This function outputs the reservation details in a structured format suitable for display in a list of reservations. It includes the reservation ID, customer name, event date, number of guests, status, and total price. The output is formatted with appropriate spacing for readability, and the status is displayed as a human-readable string based on the current status of the reservation. The total price is formatted to two decimal places and prefixed with "P" to indicate currency.
     * @note This function is used to display the reservation details in a formatted manner, typically as part of a list of reservations. It outputs the reservation ID, customer name, event date, number of guests, status, and total price in a structured format with appropriate spacing for readability. The status is displayed as a string (e.g., "Confirmed", "Completed", "Cancelled") based on the current status of the reservation. The total price is formatted to two decimal places and prefixed with "P" to indicate currency.
     * @return void
     */
    void showReservation() const {
        ostringstream date;
        date << year << "-" 
               << setfill('0') << setw(2) << month << "-" 
               << setfill('0') << setw(2) << day;
        cout.imbue(locale::classic());
        cout << left << setw(6) << id 
             << setw(18) << getTruncatedCustomerName(18) 
             << setw(15) << date.str()
             << setw(8) << guests 
             << setw(12) << getStatusString(getStatus()); // Display status as string
        cout.imbue(locale("en_US.UTF-8")); // Sets formatting to use commas
        cout << "P" << fixed << setprecision(2) << totalCost << endl;
        cout.imbue(locale::classic());
    }

    /**
     * @brief Checks if the reservation date is strictly before a given reference date.
     * * @param currentYear The year to compare against (YYYY).
     * @param currentMonth The month to compare against (1-12).
     * @param currentDay The day to compare against (1-31).
     * @return true if the reservation date has passed relative to the parameters.
     * @return false if the reservation date is today or in the future.
     */
    bool isPastDate(int currentYear, int currentMonth, int currentDay) const {
        if (year < currentYear) return true;
        if (year == currentYear && month < currentMonth) return true;
        if (year == currentYear && month == currentMonth && day < currentDay) return true;
        return false;
    }
    
};
// --- End of Reservation Class ---

/**
 * @brief Stores all reservations.
 *
 * @details
 * - Each entry corresponds to a unique reservation.
 * - No duplicate IDs are allowed.
 * - Used for file saving and management operations.
 * @note The reservation list is stored in memory and is loaded from a file at the start of the application. Any changes to reservations (additions, updates, deletions) should be followed by a call to saveToFile() to ensure that the data is persisted. The list is cleared and reloaded from the file when loadFromFile() is called, which can be used to refresh the data or reset the state of the application.
 */
vector<Reservation> reservationList;

/**
 * @brief Sorts a filtered list of reservations based on the active view and direction.
 * * @details This function implements a contextual sorting strategy.
 * - **Upcoming View:** Sorts by date in ascending order (earliest dates at the top) 
 * to highlight immediate tasks.
 * - **Archive View:** Sorts by date in descending order (most recent past events at the top) 
 * for better historical auditing.
 * - **Manual Override:** The isReverse flag allows the user to flip the logic in either view.
 * * @param filtered A reference to a vector of pointers to Reservation objects to be sorted.
 * @param showArchive A boolean flag indicating if the list contains past events (true) or upcoming events (false).
 * @param isReverse A boolean flag that, when true, inverts the default chronological order.
 */
void reservationListSort(vector<Reservation*>& filtered, bool showArchive, bool isReverse = false) {
    auto getVal = [](Reservation* r) { 
        return r->getYear() * 10000 + r->getMonth() * 100 + r->getDay(); 
    };

    sort(filtered.begin(), filtered.end(), [showArchive, isReverse, getVal](Reservation* a, Reservation* b) {
        long valA = getVal(a);
        long valB = getVal(b);

        bool condition;
        if (!showArchive) {
            // Upcoming: Default is Ascending (Soonest first)
            condition = valA < valB;
        } else {
            // Archive: Default is Descending (Recent history first)
            condition = valA > valB;
        }

        // If isReverse is true, flip the logic
        return isReverse ? !condition : condition;
    });
}

/**
 * @brief Filters the reservation list by customer name while respecting the current view.
 * * @details Performs a case-insensitive partial match search on customer names. It automatically
 * filters results to match the 'Upcoming' or 'Archive' context based on the current system date.
 * * @param fullList The master vector containing all reservation objects.
 * @param filtered A reference to the vector of pointers that will be populated with matching results.
 * @param showArchive Boolean determining whether to search within past records (true) or upcoming ones (false).
 * @param keyword The string used to filter customer names.
 * * @note This function uses `const_cast` to maintain pointer references to the original objects 
 * while searching a constant reference list.
 */
void handleSearch(const vector<Reservation>& fullList, vector<Reservation*>& filtered, bool showArchive, string keyword) {
    filtered.clear();
    Date today = getCurrentDate();
    
    // Convert keyword to lowercase for case-insensitive search
    transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);

    for (auto& res : const_cast<vector<Reservation>&>(fullList)) {
        string name = res.getName();
        transform(name.begin(), name.end(), name.begin(), ::tolower);

        bool isPast = res.isPastDate(today.year, today.month, today.day);
        
        // Match name AND Match the current View Toggle
        if (name.find(keyword) != string::npos && isPast == showArchive) {
            filtered.push_back(&res);
        }
    }
}

// --- File I/O Functions ---

/**
 * @brief Saves all reservations to a CSV file.
 *
 * @details
 * Iterates through the reservation list and writes each reservation's
 * data into a file named "reservations.csv" in comma-separated format.
 *
 * Each row contains:
 * ID, Name, Year, Month, Day, Package, Guests, Status
 *
 * @note Existing file content will be overwritten.
 * @warning If the file cannot be opened, the function silently exits.
 * @return void
 *
 */
void saveToFile() {
    ofstream outFile("reservations.csv"); ///< Output file stream for writing reservation data to "reservations.csv". The file is opened in write mode, which will overwrite any existing content. If the file cannot be opened (e.g., due to permissions issues or disk errors), the function will exit without saving any data, and no error message will be displayed.
    if (!outFile) return;

    // Write header
    outFile << "ID,Name,Year,Month,Day,Package,Guests,Status" << endl;


    for (const auto& res : reservationList) {
        outFile << res.getId() << ","
                << res.getName() << ","
                << res.getYear() << ","
                << res.getMonth() << ","
                << res.getDay() << ","
                << res.getPackage() << ","
                << res.getGuests() << ","
                << res.getStatusString(res.getStatus()) << endl;
    }
    outFile.close();
}

/**
 * @brief Loads reservations from a CSV file into the reservation list.
 *
 * @details
 * Reads from "reservations.csv" and parses each line to create Reservation objects,
 * which are then added to the global reservation list.
 *
 * The function handles potential parsing errors gracefully by skipping malformed lines.
 *
 * @warning If the file cannot be opened, the function silently exits, leaving the reservation list empty.
 * @return void
 */
void loadFromFile() {
    ifstream inFile("reservations.csv"); ///< Input file stream for reading reservation data from "reservations.csv". The file is opened in read mode. If the file cannot be opened (e.g., if it does not exist, or due to permissions issues), the function will exit without loading any data, and the reservation list will remain empty.
    if (!inFile) return;

    reservationList.clear();
    string line; ///< Temporary variable to hold each line read from the CSV file. This variable is used in the while loop to read the file line by line. Each line is expected to contain the reservation data in a comma-separated format, which will be parsed to create Reservation objects. If a line is malformed or cannot be parsed correctly, it will be skipped, and the function will continue reading the next line until the end of the file is reached.
    getline(inFile, line); // Skip the header row
    while (getline(inFile, line)) {
        stringstream ss(line); 
        string temp, name, status; ///< Temporary variables for parsing reservation data from the CSV file. The 'temp' variable is used for intermediate string storage when parsing integer values, while 'name' and 'status' are used to store the customer's name and reservation status as strings. The 'id', 'y', 'm', 'd', 'pkg', and 'guests' variables are used to hold the parsed integer values for the reservation's ID, year, month, day, package type, and number of guests, respectively. These variables are essential for creating a Reservation object after successfully parsing a line from the CSV file.
        int id, y, m, d, pkg, guests; ///< Variables to hold parsed data for each reservation. These variables are used to temporarily store the values read from the CSV file before creating a Reservation object. The 'temp' variable is used for intermediate string storage when parsing integer values, while 'name' and 'status' are used to store the customer's name and reservation status as strings.

        try {
            int statusInt; ///< Temporary variable to hold the integer representation of the reservation status parsed from the CSV file. This variable is used to convert the string representation of the status back into the ReservationStatus enum type when creating a Reservation object. The function expects the status in the CSV file to be stored as an integer corresponding to the enum values (e.g., 1 for CONFIRMED, 2 for COMPLETED, 3 for CANCELLED). If the status cannot be parsed correctly, it may lead to an invalid ReservationStatus value being set for the reservation.

            getline(ss, temp, ','); id = stoi(temp);
            getline(ss, name, ',');
            getline(ss, temp, ','); y = stoi(temp);
            getline(ss, temp, ','); m = stoi(temp);
            getline(ss, temp, ','); d = stoi(temp);
            getline(ss, temp, ','); pkg = stoi(temp);
            getline(ss, temp, ','); guests = stoi(temp);
            getline(ss, status); statusInt = "Confirmed" == status ? 1 : ("Completed" == status ? 2 : ("Cancelled" == status ? 3 : 0));

            Reservation res(id, name, y, m, d, pkg, guests);
            res.setStatus(static_cast<ReservationStatus>(statusInt));
            reservationList.push_back(res);
        } catch (const exception& e) { continue; }
    }
    inFile.close();
}
//--- End of File I/O Functions ---

/**
 * @brief Automatically marks past confirmed reservations as completed.
 *
 * @details
 * This function checks all reservations stored in the reservation list
 * and compares each event date against the current system date.
 *
 * A reservation is automatically updated only if:
 * - Its status is currently CONFIRMED
 * - Its event date has already passed
 *
 * Reservations marked as COMPLETED or CANCELLED are not modified.
 *
 * If one or more reservations are updated:
 * - Their status is changed from CONFIRMED to COMPLETED
 * - The changes are saved to the CSV file
 * - An informational message is displayed to notify the user
 *
 * This feature helps maintain data consistency by preventing outdated
 * reservations from remaining in an active confirmed state.
 *
 * @note
 * This function should be called before displaying reservation records,
 * such as at the beginning of manageReservations(), so expired events
 * are updated automatically before management operations begin.
 *
 * @warning
 * This function relies on the system date and assumes reservation dates
 * stored in the database are valid and correctly formatted.
 *
 * @return int
 */
int autoCompletePastReservations() {
    Date currentDate = getCurrentDate();

    int counter = 0; 
    for (auto& r : reservationList) {
        if (r.getStatus() == CONFIRMED && r.isPastDate(currentDate.year, currentDate.month, currentDate.day)) {
            r.setStatus(COMPLETED);
            counter++;
        }
    }
    if (counter > 0) saveToFile();
    return counter;
}

// --- UI Functions ---
/**
 * @brief Displays the central navigation hub for the Catering Reservation System.
 *
 * @details
 * This function renders the primary Command Line Interface (CLI) menu. It serves as the 
 * entry point for all system modules, categorized into four strategic operations:
 * * 1. **New Reservation**: Input module for gathering customer and event details.
 * 2. **Manage & View Reservations**: Operational dashboard with sorting, searching, and pagination.
 * 3. **Financial & Sales Report**: Business Intelligence module for annual revenue analysis.
 * 4. **Exit System**: Gracefully terminates the application and closes file streams.
 *
 * @note This function is the root of the application's control flow. It should be called 
 * within a `while(true)` loop in `main()` to ensure the user returns to this hub after 
 * completing any specific task.
 * * @return void
 */
void displayMenu() {
    cout << "\n========================================" << endl;
    cout << "     CATERING RESERVATION SYSTEM        " << endl;
    cout << "========================================" << endl;
    cout << "1. New Reservation" << endl;
    cout << "2. Manage & View Reservations" << endl;
    cout << "3. Financial & Sales Report" << endl;
    cout << "4. Exit System" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Select an option: ";
}

//--- Make Reservation Function with Enhanced Validation ---

/**
 * @brief Allows the user to make a new catering reservation.
 *
 * @details
 * Collects all necessary information from the user and validates it before creating the reservation.
 * The function ensures that the date is valid, the customer name is acceptable, and the reservation does not exceed capacity limits.
 * It also checks that the selected date is not in the past.
 * @note The function uses a loop to continuously prompt the user for input until valid data is provided or the user chooses to cancel by entering '0'. It provides feedback for invalid inputs and guides the user through the reservation process. Upon successful creation of a reservation, it saves the data to the file and displays a confirmation message.
 * @return void
 */
void makeReservation() {
    Date currentDate = getCurrentDate(); // Fetch Today's Date

    while (true) {
        clearscreen();
        string name; ///< Stores the customer's name for the reservation.
        int y, m, d, pkg, guests; ///< Variables to store the year, month, day, package type, and number of guests for the reservation.

        cout << "========================================" << endl;
        cout << "     NEW CATERING RESERVATION           " << endl;
        cout << "   (Type '0' at any time to cancel)     " << endl;
        cout << "========================================" << endl;
        INFO("Today's Date: " + to_string(currentDate.year) + "-" + to_string(currentDate.month) + "-" + to_string(currentDate.day));
        
        // --- 1.Customer Name ---
        customername:
            cout << "\nCustomer Name: ";
            getline(cin >> ws, name);
            if (name == "0") return;

            // --- Name Validation ---
            if (name.find(',') != string::npos) {
                ERROR("Commas are not allowed in names for database safety.");
                cout << "Press Enter to try again..."; cin.get();
                clearLines(4); // Clear the error message and prompt
                goto customername;
            }

        // --- 2. Year Validation ---
        year:
            cout << "Year (YYYY): "; 

            while (!(cin >> y) || cin.peek() != '\n') { 
                handleInputError("Numeric only! Enter year (e.g., 2026)");
                cout << "Year (YYYY): ";
            }

            if (y == 0) return; //Allow user to cancel by entering '0' for year.

            if (y < currentDate.year) {
                handleInputError("Invalid Year: Past years are not allowed.");
                goto year;
            }
        
        // --- 3. Month Validation ---
        month:
            cout << "Month (1-12): "; 
            while (!(cin >> m) || m < 0 || m > 12 || cin.peek() != '\n' ) { 
                handleInputError("Invalid! Enter month (1-12)");
                cout << "Month (1-12): ";
            }

            if (m == 0) return; // Allow user to cancel by entering '0' for month as well.

            if (y == currentDate.year && m < currentDate.month) {
                handleInputError("Invalid Month: Past month are not allowed.");
                goto month;
            }

        // --- 4. Day Validation ---
        day:
            int maxDays = getMaxDays(m, y); ///< Calculates the maximum number of days for the given month and year, accounting for leap years.
            cout << "Day (1-" << maxDays << "): "; 
            while (!(cin >> d) || d < 0 || d > maxDays || cin.peek() != '\n') { 
                handleInputError("Invalid! Enter valid day for this month");
                cout << "Day (1-" << maxDays << "): ";
            }
            
            if (d == 0) return; // Allow user to cancel by entering '0' for day as well.

            if (y == currentDate.year && m == currentDate.month && d < currentDate.day) {
                handleInputError("Invalid Day: Past Day are not allowed.");    
                goto day;
            }

        // --- 5. Capacity Check ---
        int eventCount = 0; ///< Counts the number of existing reservations for the selected date.
        int currentDailyGuests = 0; ///< Tracks the total number of guests already booked for the selected date. This is used to ensure that adding a new reservation does not exceed the maximum daily guest capacity.

        // Count existing events and guests for the selected date
        for (const auto& res : reservationList) {
            if (res.getYear() == y && res.getMonth() == m && res.getDay() == d && res.getStatus() != CANCELLED) {
                eventCount++;
                currentDailyGuests += res.getGuests();
            }
        }

        // Check if we already have too many bookings
        if (eventCount >= MAX_EVENTS_PER_DAY) {
            ERROR("Fully Booked: We cannot accept more than " + to_string(MAX_EVENTS_PER_DAY) + " events on this day.");
            cout << "Press Enter to try again..."; cin.ignore(); cin.get();
            continue;
        }

        // --- 6. Package & Guests ---
        cout << "\nAvailable Packages:\n";
        cout << "1. Basic   (P" << PRICE_BASIC << "/head)\n";
        cout << "2. Premium (P" << PRICE_PREMIUM << "/head)\n";
        cout << "3. Luxury  (P" << PRICE_LUXURY << "/head)\n";
        cout << "Select Package: "; 
        
        while (!(cin >> pkg) || pkg < 0 || pkg > 3 || cin.peek() != '\n') {
            handleInputError("Invalid! Select package 1, 2, or 3");
            cout << "Select Package: ";
        }

        if (pkg == 0) return; // Allow user to cancel by entering '0' for package selection as well.
        
        guest:
            cout << "Number of Guests: "; 
            while(!(cin >> guests) || guests < 0 || cin.peek() != '\n') {
                handleInputError("Numeric only! Enter number of guests");
                cout << "Number of Guests: ";
            }
            if (guests == 0) return; // Allow user to cancel by entering '0' for number of guest as well.
            
            // Validation for Guest Limits
            if (guests < MIN_GUESTS_PER_EVENT) {
                handleInputError("Order too small! Minimum guests required: " + to_string(MIN_GUESTS_PER_EVENT));
                goto guest;
            }

        // Check if adding this reservation would exceed total daily capacity
        if (currentDailyGuests + guests > MAX_TOTAL_GUESTS) {
            int remaining = MAX_TOTAL_GUESTS - currentDailyGuests; ///< Calculates the remaining guest capacity for the selected date by subtracting the current number of guests already booked from the maximum total guests allowed per day. This value is used to inform the user how many more guests can be accommodated if they choose to proceed with their reservation.
            ERROR("Capacity Exceeded! We can only accommodate " + to_string(remaining) + " more guests today.");
            WARNING("Current bookings for this day already have " + to_string(currentDailyGuests) + " guests.");
            cout << "Press Enter to try again..."; cin.ignore(); cin.get();
            goto guest;
        }

        // --- Success Path ---
        /** @brief Unique Identifier Generation Logic */
        int newId = 1001; ///< Starting ID for the first reservation ever created.

        // --- Success Path: Dynamic ID Assignment ---
        /** * @details
         * This loop implements an "Auto-Increment" behavior. It scans the entire 
         * reservationList to find the highest existing ID and sets the newId to 
         * (Highest ID + 1). This ensures that even if records are deleted, the 
         * system never duplicates an ID number.
         */
        for(const auto& r : reservationList){
            if(r.getId() >= newId)
            newId = r.getId()+1; ///< Increments to the next available unique integer.
        }

         ///< Generates a new unique reservation ID by checking if the reservation list is empty. If it is empty, it starts with 1001; otherwise, it takes the ID of the last reservation in the list and increments it by 1 to ensure uniqueness.
        reservationList.push_back(Reservation(newId, name, y, m, d, pkg, guests));
        saveToFile(); 

        // --- Confirmation Message ---
        SUCCESS("Reservation ID " + to_string(newId) + " successfully created!");
        cout << "\nPress Enter to return to menu...";
        cin.ignore(); cin.get();
        break; 
    }
}

//--- Unified Reservation Management UI ---

/**
 * @brief Provides a comprehensive interface for managing and viewing reservations.
 *
 * @details
 * The interface includes features such as filtering, sorting, pagination, and actions to update reservation status or delete reservations.
 * It allows users to navigate through pages of reservations, search for specific entries, and perform actions like marking events as completed, cancelling, or deleting reservations.
 * The function handles user input for navigation and actions, and updates the reservation list accordingly, ensuring that changes are saved to the file.
 * It also includes error handling for invalid inputs and edge cases.
 * @note This function is designed to provide a user-friendly interface for managing reservations, with clear feedback and options for users to interact with the reservation data. It is called when the user selects the "Manage & View Reservations" option from the main menu. The function will continue to run until the user chooses to return to the main menu by entering 'B' or 'b'.
 * @return void
 */
void manageReservations() {
    /** @brief Result of the automatic system maintenance. */
    int updatedCount = autoCompletePastReservations(); ///< Tracks how many 'CONFIRMED' events were automatically moved to 'COMPLETED' based on the system date.

    int currentPage = 0; ///< Tracks the current page number for pagination. It is initialized to 0, which corresponds to the first page of reservations. The currentPage variable is updated based on user input for navigating through pages (e.g., next, previous) and is used to calculate which reservations to display on the current page.
    const int itemsPerPage = 10; ///< Defines the number of reservations to display per page in the reservation management interface. This constant is used in pagination calculations to determine how many reservations to show on each page and how many pages are needed to display all reservations. It can be adjusted to show more or fewer reservations per page based on user preference or screen size.
    string searchQuery = ""; ///< Stores the current search query entered by the user for filtering reservations. It is initialized as an empty string, which means that by default, no filtering is applied and all reservations are shown. When the user enters a search query, this variable is updated and used to filter the reservation list based on matching criteria (e.g., customer name, reservation ID, event date) before displaying the results.
    
    // Sort states
    bool ascending = true; ///< Indicates the current sorting order (ascending or descending). It is initialized to true, meaning that the default sorting order is ascending. The user can toggle this state to switch between ascending and descending order for the current sorting criteria (date or ID).
    bool isReverse = false; // Indicates Ascending/Descending List
    bool showArchive = false; // Default to showing upcoming events
    bool sortByDate = true; ///< Indicates whether the reservations are currently sorted by date. It is initialized to true, meaning that the default sorting is by date. The user can toggle this state to switch between sorting by date and sorting by reservation ID.
    
    while (true) {
        clearscreen();
        if (updatedCount > 0) {
            INFO("System Maintenance: " + to_string(updatedCount) + " past events marked as COMPLETED.");
        }

        // 1. Create Filtered View & Calculate Dashboard Stats
        vector<Reservation*> filtered; ///< Stores pointers to reservations that match the current search query. This allows for efficient filtering without modifying the original reservation list. The filtered vector is populated based on the search criteria entered by the user, and it is used for sorting and pagination in the display.
        Date today = getCurrentDate();
        long todayVal = today.year * 10000 + today.month * 100 + today.day;
        double confirmedRevenue = 0;  ///< Tracks the total revenue from reservations that are currently confirmed. This variable is updated as the reservation list is filtered, summing the total cost of all reservations with a status of "Confirmed". It is displayed in the dashboard to provide insight into potential upcoming revenue based on current bookings.
        double completedRevenue = 0; ///< Tracks the total revenue from reservations that have been completed. This variable is updated as the reservation list is filtered, summing the total cost of all reservations with a status of "Completed". It is displayed in the dashboard to provide insight into revenue that has already been realized from completed events.
        int activeCount = 0; ///< Counts the number of active reservations (those that are currently confirmed but not yet completed or cancelled). This variable is updated as the reservation list is filtered, incrementing for each reservation with a status of "Confirmed". It is displayed in the dashboard to provide insight into how many upcoming events are currently active.

        // If there is no search, show everything in the current view
        if (searchQuery == "") {
            for (auto& res : reservationList) {
                if (res.isPastDate(today.year, today.month, today.day) == showArchive)
                    filtered.push_back(&res);
            }
        } else {
            // If searching, use the helper
            handleSearch(reservationList, filtered, showArchive, searchQuery);
        }

        if (reservationList.empty()) {
            cout << "\n[!] Database is empty. Press Enter to return...";
            cin.ignore(); cin.get(); return;
        }
        
        // 2. Sort the  Reservations.
        reservationListSort(filtered, showArchive, isReverse);
        

        // 3. Pagination
        int totalPages = (filtered.empty()) ? 1 : (filtered.size() + itemsPerPage - 1) / itemsPerPage; ///< Calculates the total number of pages needed to display all filtered reservations based on the defined itemsPerPage. If there are no filtered results, it defaults to 1 page to avoid division by zero and to maintain a consistent user interface. The totalPages variable is used in the pagination logic to determine how many pages of results there are and to control navigation through the pages.
        if (currentPage >= totalPages) currentPage = totalPages - 1;
        if (currentPage < 0) currentPage = 0;
        int startIdx = currentPage * itemsPerPage; ///< Calculates the starting index for the current page of reservations to display. This is determined by multiplying the currentPage number by the itemsPerPage constant. The startIdx variable is used to determine which subset of the filtered reservations should be displayed on the current page, allowing for proper pagination of results.

        // 3. Prevent Pagination Ghosting
        if (startIdx >= filtered.size() && !filtered.empty()) {
            startIdx = 0; 
        }

        // 4. UI Header & Dashboard Display
        cout << "================================================================================" << endl;
        cout << "   MANAGE RESERVATIONS | " << (sortByDate ? "Sorting: DATE" : "Sorting: ID") 
             << " [" << (ascending ? "ASC" : "DESC") << "]" << "| Active: " << activeCount << endl;  
        cout << "   Page " << currentPage + 1 << " of " << totalPages << " (" << filtered.size() << " results) | "; 
        cout << " VIEW: " << (showArchive ? "ARCHIVE" : "UPCOMING");
        if (!searchQuery.empty()) cout << " | SEARCH: '" << searchQuery << "'";
        cout << endl;
        cout << "================================================================================" << endl;
        
        cout << left << setw(6) << "ID" << setw(18) << "Customer" << setw(15) << "Event Date" 
             << setw(8) << "Guests" << setw(12) << "Status" << "Total Price" << endl;
        cout << "--------------------------------------------------------------------------------" << endl;

        // 5. Display Rows
        if (filtered.empty()) {
            cout << "\n\t\t [!] No matching records found.\n" << endl;
        } else {
            for (int i = startIdx; i < startIdx + itemsPerPage && i < (int)filtered.size(); i++) {
                filtered[i]->showReservation();
            }
        }

        // 6. Navigation & Control Menu
        cout << "--------------------------------------------------------------------------------" << endl;
        cout << "[N] Next | [P] Prev | [S] Search | [T] Toggle Sort | [R] Reverse Sort" << endl;
        cout << "[F] Finish Event | [C] Cancel | [D] Delete | [B] Back to Menu: ";
        
        string cmd;
        cin >> cmd;

        // Handle commands
        if (cmd == "b" || cmd == "B") break;
        else if (cmd == "n" || cmd == "N") { if (currentPage < totalPages - 1) currentPage++; }
        else if (cmd == "p" || cmd == "P") { if (currentPage > 0) currentPage--; }
        
        else if (cmd == "t" || cmd == "T") { 
            showArchive = !showArchive; 
            isReverse = false; 
            startIdx = 0; 
            reservationListSort(filtered, showArchive);
        }

        else if (cmd == "r" || cmd == "R") { 
            isReverse = !isReverse; 
            startIdx = 0; 
        }
        
        else if (cmd == "s" || cmd == "S") {
            cout << "Enter Search Query: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, searchQuery);
            startIdx = 0;
        }
        // Finish Event Action
        else if (cmd == "f" || cmd == "F") {
            int fid; ///< Temporary variable to hold the reservation ID entered by the user for marking as completed. This variable is used to identify which reservation the user wants to mark as completed, and it is validated to ensure that it corresponds to an existing reservation before allowing the status change to "Completed". The function checks if the reservation is currently confirmed and provides appropriate feedback to the user based on whether the status change was successful or if the specified ID was not found or cannot be marked as completed.
            
            while(true) {
                cout << "Enter ID to mark as COMPLETED (Enter '0' to cancel): ";
                
                // Validate that it's actually a number
                if (!(cin >> fid) || cin.peek() != '\n') {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    ERROR("Invalid ID format. Numeric only.");
                    cout << "Press Enter to try again...";
                    cin.get();
                    clearLines(3);
                    continue; // Prompt again after handling the invalid input
                }

                if (fid == 0) break; // Allow user to cancel by entering '0' for ID.

                bool found = false; ///< Flag to indicate whether the reservation with the specified ID was found in the reservation list. This variable is used to track whether a matching reservation was located during the search process, allowing the function to provide appropriate feedback to the user (e.g., confirming completion or displaying an error message if the ID is not found).

                /// Search for the reservation by ID
                for(auto& r : reservationList) {
                    if(r.getId() == fid) {
                        found = true;
                        if(r.getStatus() == CONFIRMED) {
                            r.setStatus(COMPLETED);
                            saveToFile();
                            SUCCESS("ID " + to_string(fid) + " marked as Completed!");
                        } else {
                            WARNING("Only 'Confirmed' bookings can be marked as 'Completed'.");
                        }
                        break;
                    }
                }

                // If not found, show error message
                if(!found) ERROR("ID " + to_string(fid) + " not found.");
                cout << "Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                clearLines(3); // Clear the error message and prompts before asking again
                continue; // Prompt again after handling the input
            }
        }
        // --- Cancel Action ---
        else if (cmd == "c" || cmd == "C") {
            int cid; ///< Temporary variable to hold the reservation ID entered by the user for cancellation. This variable is used to identify which reservation the user wants to cancel, and it is validated to ensure that it corresponds to an existing reservation before allowing the status change to "Cancelled". The function checks if the reservation is already cancelled and provides appropriate feedback to the user.
            
            while(true){
                cout << "Enter ID to Cancel (Enter '0' to cancel): ";

                // Validate that it's actually a number
                if (!(cin >> cid) || cin.peek() != '\n') {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    ERROR("Numeric only! Enter ID: ");
                    cout << "Press Enter to try again...";
                    cin.get();
                    clearLines(3); // Clears the prompt, error, and "Press Enter" lines
                    continue;
                }

                if (cid == 0) break; // Allow user to cancel by entering '0' for ID.

                bool found = false; ///< Flag to indicate whether the reservation with the specified ID was found in the reservation list. This variable is used to track whether a matching reservation was located during the search process, allowing the function to provide appropriate feedback to the user (e.g., confirming cancellation or displaying an error message if the ID is not found).
                
                // Search for the reservation by ID
                for(auto& r : reservationList) {
                    if(r.getId() == cid) {
                        found = true;
                        if(r.getStatus() != CANCELLED) {
                            r.setStatus(CANCELLED);
                            saveToFile();
                            SUCCESS("ID " + to_string(cid) + " is now Cancelled.");
                        } else {
                            WARNING("ID " + to_string(cid) + " was already cancelled.");
                        }
                        break;
                    }
                }
                
                // If not found, show error message and prompt again
                if(!found) ERROR("ID " + to_string(cid) + " not found in database.");
                
                // After handling the input, prompt again for cancellation or allow exit with '0' to go back to the normal mode.
                cout << "Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                clearLines(3); // Clear the error message and prompts before asking again
                continue; // Prompt again after handling the input
            } 
        }

        // --- Delete Action ---
        else if (cmd == "d" || cmd == "D") {
            int did; ///< Temporary variable to hold the reservation ID entered by the user for deletion. This variable is used to identify which reservation the user wants to delete, and it is validated to ensure that it corresponds to an existing reservation before allowing the deletion. The function includes a confirmation step to prevent accidental deletions, and it provides feedback to the user based on whether the deletion was successful or if the specified ID was not found.
            
            while(true) {
                cout << "Enter ID to DELETE (Enter '0' to cancel): ";

                if (!(cin >> did) || cin.peek() != '\n') {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    ERROR("Numeric only! Enter ID to DELETE: ");
                    cout << "Press Enter to try again...";
                    cin.get();
                    clearLines(3);
                    continue;
                }

                if (did == 0) break; // Allow user to cancel by entering '0' for ID.

                // Find the reservation by ID
                auto it = find_if(reservationList.begin(), reservationList.end(), [&](const Reservation& r) {
                    return r.getId() == did;
                });

                // if found, confirm deletion and delete or show error if not found
                if (it != reservationList.end()) {
                    char confirm;
                    WARNING("Are you sure you want to PERMANENTLY delete ID " + to_string(did) + "? (y/n): ");
                    cin >> confirm;

                    if (tolower(confirm) == 'y') {
                        reservationList.erase(it);
                        saveToFile();
                        SUCCESS("ID " + to_string(did) + " deleted from database.");
                        
                    } else {
                        INFO("Deletion cancelled.");
                        // After handling the input, prompt again for deletion or allow exit with '0' to go back to the normal mode.
                        cout << "Press Enter to continue...";
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cin.get();
                        clearLines(5); // Clear the error message and prompts before asking again.
                        continue; // Prompt again after handling the input
                    }
                } else {
                    ERROR("ID " + to_string(did) + " not found.");
                }
                
                // After handling the input, prompt again for deletion or allow exit with '0' to go back to the normal mode.
                cout << "Press Enter to continue...";
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cin.get();
                clearLines(3); // Clear the error message and prompts before asking again.
                continue; // Prompt again after handling the input
            }
            
        }
    }
}

/**
 * @brief Generates an annual financial sales report with an optional file export.
 * * @details
 * This function performs a single-pass aggregation of all COMPLETED reservations 
 * for a target year. It displays the results in a formatted table on the console 
 * and provides the user an option to save a human-readable text file (e.g., Sales_Report_2026.txt) 
 * for physical record-keeping.
 * * @note The export feature uses std::stringstream to ensure the file content matches 
 * the console output exactly.
 */
void generateSalesReport() {
    clearscreen();
    int targetYear; ///< The year requested by the user for financial analysis.
    cout << "====================================================" << endl;
    cout << "           FINANCIAL & SALES REPORT                " << endl;
    cout << "====================================================" << endl;
    cout << "Enter Year to analyze: ";
    
    if (!(cin >> targetYear)) {
        handleInputError("Invalid input. Numeric year only.", 0);
        return;
    }
    cout.imbue(locale::classic());
    
    /** @brief Accumulators for monthly and annual data */
    double monthlyRevenue[13] = {0.0}; ///< Array storing revenue totals indexed by month (1-12).
    int monthlyCount[13] = {0};       ///< Array storing count of completed events per month.
    double totalYearly = 0;           ///< Running total of revenue for the entire year.
    int totalEvents = 0;              ///< Running count of all completed events in the year.

    /** @brief Formatting and Buffer utilities */
    stringstream ss;    ///< Buffer to capture the report for dual output (Console and File).
    
    /// Names of months for tabular display; index 0 is empty to align with 1-based months.
    const string monthNames[] = {"", "January", "February", "March", "April", "May", "June", 
    "July", "August", "September", "October", "November", "December"};
                                
    // Header Construction
    ss.imbue(locale::classic()); 
    ss << "====================================================\n";
    ss << "           FINANCIAL & SALES REPORT - " << targetYear << "\n"; // Year: 2026
    ss << "====================================================\n";
    ss << left << setw(15) << "MONTH" << setw(12) << "EVENTS" << "REVENUE" << "\n";
    ss << "----------------------------------------------------\n";

    // Aggregate data
    for (const auto& res : reservationList) {
        if (res.getYear() == targetYear && res.getStatus() == COMPLETED) {
            int m = res.getMonth();
            if (m >= 1 && m <= 12) {
                monthlyRevenue[m] += res.getTotal();
                monthlyCount[m]++;
                totalYearly += res.getTotal();
                totalEvents++;
            }
        }
    }

    
    // Report Generation
    for (int i = 1; i <= 12; i++) {
        if (monthlyCount[i] > 0) {
            // Standard columns
            ss.imbue(locale::classic());
            ss << left << setw(15) << monthNames[i] 
               << setw(12) << monthlyCount[i] << "P";
            
            // MONEY ONLY: Switch to Comma Locale
            ss.imbue(locale("en_US.UTF-8"));
            ss << fixed << setprecision(2) << monthlyRevenue[i] << "\n";
        }
    }
    
    // Footer Construction
    ss.imbue(locale::classic());
    ss << "----------------------------------------------------\n";
    ss << "TOTAL COMPLETED EVENTS: " << totalEvents << "\n";
    ss << "NET ANNUAL REVENUE:     P";
    
    // Switch for the Grand Total
    ss.imbue(locale("en_US.UTF-8"));
    ss << totalYearly << "\n";
    
    ss.imbue(locale::classic());
    ss << "====================================================\n";
    
    // Display captured report to console
    cout << "\n" << ss.str();
    cout.imbue(locale::classic());
    // Export Option
    cout << "\nWould you like to export this report to a text file? (y/n): ";
    char choice; ///< User's decision for file export.
    cin >> choice; 

    if (tolower(choice) == 'y') {
        string fileName = "Sales_Report_" + to_string(targetYear) + ".txt";
        ofstream reportFile(fileName);
        if (reportFile.is_open()) {
            reportFile.imbue(locale("en_US.UTF-8"));
            reportFile << ss.str();
            reportFile.close();
            SUCCESS("Report saved as " + fileName);
        } else {
            ERROR("Could not create the report file.");
        }
    }

    cout << "\nPress Enter to return to Main Menu...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// --- End of UI Functions ---

/**
 * @brief The main function serves as the entry point for the Catering Reservation System.
 *
 * @details It initializes the system by loading existing reservations from a file, and then enters a loop to display the main menu and handle user input for making new reservations, managing existing reservations, or exiting the system.
 * The function includes input validation to ensure that the user selects valid menu options, and it calls the appropriate functions based on the user's choice. The loop continues until the user chooses to exit the system, at which point a goodbye message is displayed.
 *
 * @return int The exit status of the program.
 */
int main() {
    loadFromFile();
    
    int choiceInput; ///< Temporary variable to hold the user's menu choice input. This variable is used to read the user's selection from the main menu and is then cast to the MenuOption enum type for processing in the switch statement. It is declared as an integer to allow for input validation, ensuring that the user enters a numeric value corresponding to a valid menu option.
    bool running = true; ///< Controls the main loop of the application. It is initialized to true, allowing the loop to run until the user chooses to exit the system. When the user selects the "Exit System" option from the menu, this variable is set to false, which will terminate the loop and end the program.

    while (running) {
        displayMenu();
        
        // Input validation for non-numeric entries
        if (!(cin >> choiceInput)) {
            ERROR("Invalid input! Please enter a number.");
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
            clearscreen();
            continue;
        }
        
        MenuOption choice = static_cast<MenuOption>(choiceInput); ///< Casts the user's menu choice input from an integer to the MenuOption enum type. This allows the program to use a switch statement to handle the user's selection based on the defined menu options

        switch (choice) {
            case MAKE_RESERVATION:
                makeReservation();
                clearscreen();
                break;

            case MANAGE_RESERVATIONS:
                manageReservations();
                clearscreen();
                break;

            case SALES_REPORT:
                generateSalesReport();
                clearscreen();
            break;
            
            case EXIT:
                SUCCESS("Thank you for using the Catering Reservation System. Goodbye!");
                running = false;
                break;

            default:
                ERROR("Invalid option! Please select a valid menu number.");
                cin.clear(); 
                cin.ignore(100, '\n');
                cin.get();
                clearscreen();
                break;
        }
    }

    return 0;
}