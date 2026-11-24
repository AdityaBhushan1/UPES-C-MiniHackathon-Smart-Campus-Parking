# Smart Campus Parking System

> **C Programming Mini Hackathon — Problem 5**

---

## Team

| Role   | Name                     | SAP ID       |
| ------ | ------------------------ | ------------ |
| Member | *(Add your name here)*   | *(SAP ID)*   |
| Member | *(Add your name here)*   | *(SAP ID)*   |
| Member | *(Add your name here)*   | *(SAP ID)*   |
| Member | *(Add your name here)*   | *(SAP ID)*   |

**Team Name:** Nexus  
**University:** UPES  
**Course:** B.Tech — First Year  
**Subject:** C Programming

---

## Problem Statement

Design and implement a command-line **Smart Campus Parking System** using C programming that manages vehicle entry, exit, parking slot allocation, fee calculation, and transaction history within a fixed-capacity parking lot.

---

## Project Objectives

1. Maintain a fixed number of parking spaces (configurable).
2. Record vehicle registration numbers on entry.
3. Automatically allocate the first available parking slot.
4. Calculate parking charges based on duration in whole hours.
5. Prevent duplicate active vehicle entries.
6. Detect and report when the parking lot is full.
7. Maintain a session-based log of all completed parking transactions.

---

## Features

| # | Feature                | Description |
|---|------------------------|-------------|
| 1 | **Vehicle Entry**      | Accepts and validates a registration number, checks for duplicates, allocates the first available slot, records entry timestamp, and displays confirmation. |
| 2 | **Vehicle Exit**       | Locates the vehicle, automatically calculates parking duration from entry/exit timestamps, calculates the fee, prints a receipt, records the transaction, and releases the slot. |
| 3 | **Parking Status**     | Displays all 10 slots in a formatted table with occupancy statistics (total, occupied, available, occupancy %). |
| 4 | **Search Vehicle**     | Searches by registration number and reports the slot and parked status. |
| 5 | **Parking Summary**    | Shows total revenue, vehicles currently parked, available spaces, completed transactions, and occupancy percentage. |
| 6 | **Transaction History**| Lists all completed exit transactions with registration, slot, duration, and fee in a formatted table. |
| 7 | **Input Validation**   | Rejects empty input, special characters, overly long strings, non-integer menu selections, and invalid parking durations. |
| 8 | **Duplicate Detection**| Prevents the same vehicle from being registered twice while it is still parked. |
| 9 | **Full Lot Detection** | Displays a clear error when all parking slots are occupied. |

---

## Technical Assumptions

| Parameter              | Value                              |
|------------------------|------------------------------------|
| Total parking slots    | 10 (configurable via `MAX_SLOTS`)  |
| Parking rate           | Rs.20/hour (`RATE_PER_HOUR`)       |
| Minimum duration       | 1 hour (`MIN_DURATION`)            |
| Duration calculation   | Automatic from entry/exit timestamps (rounded to whole hours, min 1 hr) |
| Registration format    | Letters (A–Z), digits (0–9), hyphens (-) |
| Max registration length| 20 characters (`MAX_REG_LEN`)      |
| Max transaction records| 100 per session (`MAX_TRANSACTIONS`)|
| Data persistence       | Session-only (not saved to files)  |

All values are defined as `#define` macros at the top of `parking.c` and can be changed before compilation.

---

## C Programming Concepts Used

| Concept                | Where Used |
|------------------------|------------|
| **Variables & Data Types** | `int`, `char`, `float`, `time_t`, `double` for slot data, fees, timestamps, occupancy |
| **Operators**          | Arithmetic (`*`, `/`), relational (`==`, `!=`, `<`), logical (`!`, `&&`) |
| **Conditional Statements** | `if-else` for validation, duplicate checks, full-lot detection |
| **Loops**              | `for` loops for iterating over slots and transactions; `do-while` for menu |
| **Arrays**             | `slots[]` for parking slots, `history[]` for transactions |
| **Strings**            | `strcmp`, `strcpy`, `strlen`, `strcspn`, `fgets` for registration handling |
| **Structures**         | `ParkingSlot` and `Transaction` to group related data |
| **User-defined Functions** | 17 functions, each handling a single responsibility |
| **Switch Statement**   | Menu dispatch in `main()` |
| **Input Validation**   | `scanf` return check, `fgets` with length limits, character validation |
| **Macros / Constants** | `#define` for all configurable parameters |
| **Type Casting**       | `(unsigned char)` for `ctype.h` functions, `(char)` for `toupper` |

---

## Functions and Their Responsibilities

| Function                 | Responsibility |
|--------------------------|----------------|
| `main()`                 | Program entry point; runs the menu loop with a `do-while` and `switch`. |
| `initializeSlots()`      | Sets up all parking slots as available with slot numbers 1–10. |
| `displayMenu()`          | Prints the main menu with options 0–6. |
| `printLine(ch, len)`     | Prints a decorative line of character `ch` repeated `len` times. |
| `clearInputBuffer()`     | Discards leftover characters in `stdin` to prevent input corruption. |
| `readMenuChoice()`       | Reads an integer menu choice; returns -1 for non-integer input. |
| `readRegistration(reg)`  | Reads a registration string using `fgets`, trims newline, validates format. |
| `validateRegistration(reg)` | Checks that every character is alphanumeric or a hyphen. |
| `toUpperCase(str)`       | Converts a string to uppercase for case-insensitive comparison. |
| `findVehicle(reg)`       | Linear search through occupied slots; returns index or -1. |
| `findAvailableSlot()`    | Returns the index of the first free slot, or -1 if full. |
| `countOccupied()`        | Counts and returns the number of occupied slots. |
| `vehicleEntry()`         | Full entry workflow: validate → duplicate check → allocate → record timestamp → confirm. |
| `vehicleExit()`          | Full exit workflow: find → auto-calculate duration from timestamps → fee → receipt → record → release. |
| `viewParkingStatus()`    | Displays the slot table and occupancy summary. |
| `searchVehicle()`        | Searches for a vehicle and displays its location or a not-found message. |
| `parkingSummary()`       | Displays revenue, occupancy, and transaction statistics. |
| `viewTransactionHistory()` | Displays all completed transactions in a formatted table. |
| `printReceipt(reg, slot, duration, fee)` | Formats and prints a parking receipt. |

---

## Algorithm / Pseudocode

```
BEGIN
    Initialize all 10 slots as Available
    Display welcome banner

    REPEAT
        Display main menu (options 1-6, 0 to exit)
        Read user choice

        SWITCH choice:
            CASE 1 (Vehicle Entry):
                Read registration number
                IF invalid format → show error, return
                IF duplicate found → show error, return
                IF no available slot → show "lot full", return
                Allocate first available slot
                Record entry timestamp using time()
                Display confirmation with slot number and entry time

            CASE 2 (Vehicle Exit):
                Read registration number
                IF not found → show error, return
                Record exit timestamp
                Calculate duration = (exitTime - entryTime) in hours
                IF duration < 1 → set duration = 1 (minimum billing)
                Calculate fee = duration × Rs.20
                Print parking receipt
                Record transaction in history[]
                Add fee to total revenue
                Release the slot

            CASE 3 (Parking Status):
                Print table of all slots (number, registration, status)
                Print occupancy statistics

            CASE 4 (Search Vehicle):
                Read registration number
                Search occupied slots
                Display result (found with slot, or not found)

            CASE 5 (Parking Summary):
                Display revenue, parked count, available, transactions, occupancy %

            CASE 6 (Transaction History):
                IF no transactions → show "no records"
                ELSE print table of all transactions

            CASE 0:
                Display goodbye message

            DEFAULT:
                Show "invalid choice"

    UNTIL choice == 0
END
```

---

## Compilation and Execution Instructions

### Prerequisites

- GCC compiler (MinGW on Windows, or gcc on Linux/macOS).
- Any terminal or command prompt.

### Compile

```bash
gcc -o parking parking.c -Wall -Wextra
```

The `-Wall -Wextra` flags enable all warnings. The program compiles with **zero warnings**.

### Run

**Windows:**
```bash
parking.exe
```

**Linux / macOS:**
```bash
./parking
```

---

## Sample Inputs and Outputs

### Vehicle Entry (Success)

```
  Enter your choice: 1

  ==================================================
     VEHICLE ENTRY
  ==================================================
  Enter registration number: UK07AB1234

  [OK] Vehicle parked successfully!
  Vehicle       : UK07AB1234
  Assigned Slot : 1
  Entry Time    : Mon Nov 24 14:30:00 2026

  Occupied : 1 / 10  |  Available : 9
```

### Duplicate Entry (Error)

```
  Enter your choice: 1

  ==================================================
     VEHICLE ENTRY
  ==================================================
  Enter registration number: UK07AB1234

  [!] Vehicle 'UK07AB1234' is already parked.
```

### Vehicle Exit with Receipt

```
  Enter your choice: 2

  ==================================================
     VEHICLE EXIT
  ==================================================
  Enter registration number: UK07AB1234
  Entry Time   : Mon Nov 24 14:30:00 2026
  Exit Time    : Mon Nov 24 17:30:00 2026
  Duration     : 3 hour(s)

  ==================================================
            PARKING RECEIPT
  ==================================================
  Vehicle      : UK07AB1234
  Slot         : 1
  Duration     : 3 hour(s)
  Rate         : Rs.20/hour
  --------------------------------------------------
  TOTAL FEE    : Rs.60
  ==================================================
  Thank you! Drive safely.
  ==================================================

  [OK] Slot 1 is now available.
```

### Parking Status Table

```
  +------+------------------+------------+
  | Slot | Registration     | Status     |
  +------+------------------+------------+
  |    1 | UK07AB1234       | Occupied   |
  |    2 | UK07CD5678       | Occupied   |
  |    3 | DL01XY9999       | Occupied   |
  |    4 | ---              | Available  |
  |    5 | ---              | Available  |
  ...
  +------+------------------+------------+

  Total Slots : 10
  Occupied    : 3
  Available   : 7
  Occupancy   : 30.0%
```

### Transaction History

```
  +-----+------------------+------+----------+----------+
  | No. | Registration     | Slot | Duration |   Fee    |
  +-----+------------------+------+----------+----------+
  |   1 | UK07AB1234       |    1 |     3 hr | Rs.   60 |
  |   2 | UK07CD5678       |    2 |     2 hr | Rs.   40 |
  |   3 | HR26DK8899       |    1 |     1 hr | Rs.   20 |
  +-----+------------------+------+----------+----------+

  Total Revenue : Rs.120
```

---

## Test Cases

| # | Test Case                             | Input                  | Expected Result                              | Status |
|---|---------------------------------------|------------------------|----------------------------------------------|--------|
| 1 | Successful vehicle entry              | Reg: `UK07AB1234`      | Assigned to Slot 1, Available: 9             | ✅ Pass |
| 2 | Duplicate vehicle registration        | Reg: `UK07AB1234` (again) | Error: "already parked"                   | ✅ Pass |
| 3 | Successful vehicle exit               | Reg: `UK07AB1234`, 3 hrs | Receipt: Rs.60, Slot 1 released            | ✅ Pass |
| 4 | Search for existing vehicle           | Reg: `UK07AB1234`      | Found at Slot 1, Currently Parked            | ✅ Pass |
| 5 | Search for nonexistent vehicle        | Reg: `XX00ZZZZZZ`      | Error: "NOT currently parked"                | ✅ Pass |
| 6 | Entry when lot is full                | Fill all 10 slots, try 11th | Error: "Parking lot is FULL"            | ✅ Pass |
| 7 | Invalid registration (special chars)  | Reg: `!@#$%`           | Error: "Use only letters, digits, hyphens"   | ✅ Pass |
| 8 | Invalid menu input (non-integer)      | Input: `abc`           | Error: "Invalid choice"                      | ✅ Pass |
| 9 | Empty registration                    | Reg: *(empty)*         | Error: "cannot be empty"                     | ✅ Pass |
| 10 | Correct fee calculation              | 3 hours × Rs.20        | Rs.60                                        | ✅ Pass |
| 11 | Correct revenue after multiple exits | Rs.60 + Rs.40 + Rs.20  | Total: Rs.120                                | ✅ Pass |
| 12 | Slot available after exit            | Exit from Slot 1       | Slot 1 shows "Available" in status table     | ✅ Pass |

**All 12 test cases pass with zero compilation warnings.**

---

## Limitations

1. **No data persistence** — All data is lost when the application exits. No file I/O is used.
2. **Session-based history** — Transaction history is capped at 100 records per session.
3. **Manual duration input** — Parking duration is entered manually; the system does not track real-time entry/exit timestamps.
4. **Single parking lot** — The system manages only one lot; there is no multi-lot or multi-floor support.
5. **No vehicle type support** — All vehicles are charged the same rate regardless of type (car, bike, etc.).
6. **No user authentication** — There is no login or role-based access control.

## Future Improvements

1. **File I/O** — Save and load parking data to/from files for persistence across sessions.
2. **Timestamp tracking** — Use `time.h` to record actual entry/exit times and auto-calculate duration.
3. **Vehicle categories** — Support different rates for cars, bikes, and commercial vehicles.
4. **Dynamic capacity** — Allow configuring the number of slots at runtime.
5. **Search enhancements** — Search by partial registration or by slot number.
6. **Sorting and filtering** — Sort transaction history by date, fee, or duration.
7. **Admin features** — Password-protected settings and revenue reports.
8. **Multi-lot support** — Manage multiple parking areas from a single application.

---

## Project Structure

```
Nexus_Smart_Campus_Parking/
├── parking.c              # Complete C source code
├── README.md              # This documentation
├── flowchart.mmd          # Mermaid flowchart of program logic
├── presentation.pptx      # Academic presentation
└── screenshots/
    └── sample_output.png  # Sample terminal output screenshot
```

---

## License

This project was developed as part of the UPES B.Tech C Programming Mini Hackathon and is intended for academic purposes.

---

*Developed by Team Nexus*
