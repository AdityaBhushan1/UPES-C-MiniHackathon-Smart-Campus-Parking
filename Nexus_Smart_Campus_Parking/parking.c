/*
 * =============================================================
 *  Smart Campus Parking System
 *  Team: Nexus
 *  B.Tech First Year - C Programming Mini Hackathon (Problem 5)
 * =============================================================
 *
 *  Description:
 *      A menu-driven parking management system that tracks
 *      vehicle entry/exit, parking slot allocation, fee
 *      calculation, and transaction history.
 *
 *  Assumptions:
 *      - Total parking slots    : 10
 *      - Parking rate           : Rs.20 per hour
 *      - Minimum duration       : 1 hour
 *      - Duration input         : Whole hours only
 *      - Registration format    : Letters, digits, hyphens
 *      - Session-based history  : Data is not persisted to files
 *
 *  Compilation:
 *      gcc -o parking parking.c -Wall -Wextra
 *
 *  Execution:
 *      ./parking          (Linux / macOS)
 *      parking.exe        (Windows)
 * =============================================================
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ===================== CONFIGURATION ====================== */

#define MAX_SLOTS        10     /* Total number of parking slots      */
#define RATE_PER_HOUR    20     /* Parking charge per hour (Rs.)      */
#define MIN_DURATION     1      /* Minimum billable duration (hours)  */
#define MAX_REG_LEN      20     /* Max registration string length     */
#define MAX_TRANSACTIONS 100    /* Max completed transaction records  */

/* ===================== DATA STRUCTURES ==================== */

/*
 * ParkingSlot:
 *   Represents a single parking slot in the lot.
 *   - slotNumber : Slot ID from 1 to MAX_SLOTS.
 *   - regNumber  : Registration number of the parked vehicle,
 *                  or "---" if the slot is free.
 *   - isOccupied : 1 if occupied, 0 if available.
 */
typedef struct {
    int  slotNumber;
    char regNumber[MAX_REG_LEN + 1];
    int  isOccupied;
} ParkingSlot;

/*
 * Transaction:
 *   Represents a completed parking transaction recorded
 *   when a vehicle exits the parking lot.
 */
typedef struct {
    char regNumber[MAX_REG_LEN + 1];
    int  slotNumber;
    int  duration;   /* Parking duration in hours */
    int  fee;        /* Calculated fee in Rs.     */
} Transaction;

/* ===================== GLOBAL DATA ======================== */

ParkingSlot slots[MAX_SLOTS];            /* Array of parking slots   */
Transaction history[MAX_TRANSACTIONS];   /* Completed transactions   */
int transactionCount = 0;                /* Number of transactions   */
int totalRevenue     = 0;                /* Cumulative revenue (Rs.) */

/* ===================== FUNCTION PROTOTYPES ================ */

/* Initialization and display */
void initializeSlots(void);
void displayMenu(void);
void printLine(char ch, int len);

/* Input helpers */
void clearInputBuffer(void);
int  readMenuChoice(void);
int  readRegistration(char *reg);

/* Validation and search */
int  validateRegistration(const char *reg);
void toUpperCase(char *str);
int  findVehicle(const char *reg);
int  findAvailableSlot(void);
int  countOccupied(void);

/* Core features */
void vehicleEntry(void);
void vehicleExit(void);
void viewParkingStatus(void);
void searchVehicle(void);
void parkingSummary(void);
void viewTransactionHistory(void);
void printReceipt(const char *reg, int slot, int duration, int fee);

/* ===================== MAIN FUNCTION ====================== */

int main(void)
{
    int choice;

    /* Set up all parking slots as available */
    initializeSlots();

    /* Welcome banner */
    printf("\n");
    printLine('=', 50);
    printf("     SMART CAMPUS PARKING SYSTEM\n");
    printf("     Team Nexus | UPES Mini Hackathon\n");
    printLine('=', 50);

    /* Main menu loop */
    do {
        displayMenu();
        choice = readMenuChoice();

        switch (choice) {
            case 1:
                vehicleEntry();
                break;
            case 2:
                vehicleExit();
                break;
            case 3:
                viewParkingStatus();
                break;
            case 4:
                searchVehicle();
                break;
            case 5:
                parkingSummary();
                break;
            case 6:
                viewTransactionHistory();
                break;
            case 0:
                /* Goodbye message */
                printf("\n");
                printLine('=', 50);
                printf("  Thank you for using Smart Campus Parking!\n");
                printf("  Drive safely. Goodbye!\n");
                printLine('=', 50);
                printf("\n");
                break;
            default:
                printf("\n  [!] Invalid choice. Please select 0-6.\n");
                break;
        }

    } while (choice != 0);

    return 0;
}

/* ===================== INITIALIZATION ===================== */

/*
 * initializeSlots:
 *   Sets up each parking slot with a slot number (1 to MAX_SLOTS),
 *   marks it as available, and sets the registration to "---".
 */
void initializeSlots(void)
{
    int i;
    for (i = 0; i < MAX_SLOTS; i++) {
        slots[i].slotNumber = i + 1;
        strcpy(slots[i].regNumber, "---");
        slots[i].isOccupied = 0;
    }
}

/* ===================== DISPLAY HELPERS ==================== */

/*
 * printLine:
 *   Prints a horizontal line made of character 'ch',
 *   repeated 'len' times, indented with two spaces.
 */
void printLine(char ch, int len)
{
    int i;
    printf("  ");
    for (i = 0; i < len; i++) {
        putchar(ch);
    }
    printf("\n");
}

/*
 * displayMenu:
 *   Prints the main menu with all available options.
 */
void displayMenu(void)
{
    printf("\n");
    printLine('-', 50);
    printf("     MAIN MENU\n");
    printLine('-', 50);
    printf("   [1] Vehicle Entry\n");
    printf("   [2] Vehicle Exit\n");
    printf("   [3] View Parking Status\n");
    printf("   [4] Search Vehicle\n");
    printf("   [5] Parking Summary\n");
    printf("   [6] View Transaction History\n");
    printf("   [0] Exit Application\n");
    printLine('-', 50);
}

/* ===================== INPUT HELPERS ====================== */

/*
 * clearInputBuffer:
 *   Discards any remaining characters in stdin until a
 *   newline or EOF is reached. Prevents leftover input
 *   from affecting subsequent reads.
 */
void clearInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;  /* discard characters */
}

/*
 * readMenuChoice:
 *   Reads an integer choice from the user.
 *   Returns -1 if the input is not a valid integer,
 *   which is handled as an invalid menu selection.
 */
int readMenuChoice(void)
{
    int choice;

    printf("\n  Enter your choice: ");

    if (scanf("%d", &choice) != 1) {
        clearInputBuffer();
        return -1;
    }
    clearInputBuffer();
    return choice;
}

/*
 * readRegistration:
 *   Reads a vehicle registration number from the user.
 *   - Removes the trailing newline.
 *   - Rejects empty input and strings exceeding MAX_REG_LEN.
 *   - Converts to uppercase for consistent comparison.
 *   - Validates that only letters, digits, and hyphens are used.
 *   Returns 1 on success, 0 on failure.
 */
int readRegistration(char *reg)
{
    char buffer[MAX_REG_LEN + 10];  /* Extra room for safety */

    printf("  Enter registration number: ");

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        reg[0] = '\0';
        return 0;
    }

    /* Remove trailing newline */
    buffer[strcspn(buffer, "\n")] = '\0';

    /* Reject empty input */
    if (strlen(buffer) == 0) {
        printf("\n  [!] Registration number cannot be empty.\n");
        reg[0] = '\0';
        return 0;
    }

    /* Reject overly long input */
    if (strlen(buffer) > (size_t)MAX_REG_LEN) {
        printf("\n  [!] Registration too long (max %d characters).\n",
               MAX_REG_LEN);
        reg[0] = '\0';
        return 0;
    }

    /* Copy, convert to uppercase, and validate */
    strcpy(reg, buffer);
    toUpperCase(reg);

    if (!validateRegistration(reg)) {
        printf("\n  [!] Invalid registration. "
               "Use only letters, digits, and hyphens.\n");
        reg[0] = '\0';
        return 0;
    }

    return 1;
}

/* ===================== VALIDATION & SEARCH ================ */

/*
 * validateRegistration:
 *   Checks that every character in 'reg' is a letter,
 *   digit, or hyphen. Returns 1 if valid, 0 otherwise.
 */
int validateRegistration(const char *reg)
{
    int i;

    if (reg[0] == '\0')
        return 0;

    for (i = 0; reg[i] != '\0'; i++) {
        if (!isalnum((unsigned char)reg[i]) && reg[i] != '-') {
            return 0;
        }
    }
    return 1;
}

/*
 * toUpperCase:
 *   Converts all lowercase letters in a string to uppercase.
 *   Used to ensure registration comparisons are case-insensitive.
 */
void toUpperCase(char *str)
{
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        str[i] = (char)toupper((unsigned char)str[i]);
    }
}

/*
 * findVehicle:
 *   Searches all occupied slots for a vehicle with the given
 *   registration number.
 *   Returns the array index (0 to MAX_SLOTS-1) if found,
 *   or -1 if the vehicle is not currently parked.
 */
int findVehicle(const char *reg)
{
    int i;
    for (i = 0; i < MAX_SLOTS; i++) {
        if (slots[i].isOccupied && strcmp(slots[i].regNumber, reg) == 0) {
            return i;
        }
    }
    return -1;
}

/*
 * findAvailableSlot:
 *   Returns the index of the first unoccupied slot,
 *   or -1 if all slots are taken.
 */
int findAvailableSlot(void)
{
    int i;
    for (i = 0; i < MAX_SLOTS; i++) {
        if (!slots[i].isOccupied) {
            return i;
        }
    }
    return -1;
}

/*
 * countOccupied:
 *   Counts and returns the number of currently occupied slots.
 */
int countOccupied(void)
{
    int count = 0;
    int i;
    for (i = 0; i < MAX_SLOTS; i++) {
        if (slots[i].isOccupied) {
            count++;
        }
    }
    return count;
}

/* ===================== CORE FEATURES ====================== */

/*
 * vehicleEntry:
 *   Handles the entry of a new vehicle:
 *   1. Reads and validates the registration number.
 *   2. Checks for duplicate active registrations.
 *   3. Finds the first available slot.
 *   4. Allocates the slot and displays confirmation.
 */
void vehicleEntry(void)
{
    char reg[MAX_REG_LEN + 1];
    int  slotIndex;
    int  occupied, available;

    printf("\n");
    printLine('=', 50);
    printf("     VEHICLE ENTRY\n");
    printLine('=', 50);

    /* Read and validate registration */
    if (!readRegistration(reg)) {
        return;
    }

    /* Reject duplicate active registration */
    if (findVehicle(reg) != -1) {
        printf("\n  [!] Vehicle '%s' is already parked.\n", reg);
        return;
    }

    /* Find first available slot */
    slotIndex = findAvailableSlot();
    if (slotIndex == -1) {
        printf("\n  [!] Parking lot is FULL. No slots available.\n");
        return;
    }

    /* Allocate the slot */
    strcpy(slots[slotIndex].regNumber, reg);
    slots[slotIndex].isOccupied = 1;

    /* Display confirmation */
    occupied  = countOccupied();
    available = MAX_SLOTS - occupied;

    printf("\n  [OK] Vehicle parked successfully!\n");
    printf("  Vehicle       : %s\n", reg);
    printf("  Assigned Slot : %d\n", slots[slotIndex].slotNumber);
    printf("\n  Occupied : %d / %d  |  Available : %d\n",
           occupied, MAX_SLOTS, available);
}

/*
 * vehicleExit:
 *   Handles the exit of a parked vehicle:
 *   1. Reads the registration and locates the vehicle.
 *   2. Accepts parking duration in whole hours.
 *   3. Calculates the fee and prints a receipt.
 *   4. Records the transaction and releases the slot.
 */
void vehicleExit(void)
{
    char reg[MAX_REG_LEN + 1];
    int  slotIndex;
    int  duration;
    int  fee;

    printf("\n");
    printLine('=', 50);
    printf("     VEHICLE EXIT\n");
    printLine('=', 50);

    /* Read and validate registration */
    if (!readRegistration(reg)) {
        return;
    }

    /* Find the vehicle */
    slotIndex = findVehicle(reg);
    if (slotIndex == -1) {
        printf("\n  [!] Vehicle '%s' is not found in the parking lot.\n", reg);
        return;
    }

    /* Read parking duration */
    printf("  Enter parking duration (whole hours, min %d): ", MIN_DURATION);
    if (scanf("%d", &duration) != 1) {
        clearInputBuffer();
        printf("\n  [!] Invalid input. Please enter a whole number.\n");
        return;
    }
    clearInputBuffer();

    if (duration < MIN_DURATION) {
        printf("\n  [!] Duration must be at least %d hour(s).\n",
               MIN_DURATION);
        return;
    }

    /* Calculate parking fee */
    fee = duration * RATE_PER_HOUR;

    /* Print the parking receipt */
    printReceipt(reg, slots[slotIndex].slotNumber, duration, fee);

    /* Record the completed transaction */
    if (transactionCount < MAX_TRANSACTIONS) {
        strcpy(history[transactionCount].regNumber, reg);
        history[transactionCount].slotNumber = slots[slotIndex].slotNumber;
        history[transactionCount].duration   = duration;
        history[transactionCount].fee        = fee;
        transactionCount++;
    } else {
        printf("  [!] Transaction history is full. "
               "Record not saved.\n");
    }

    /* Update total revenue */
    totalRevenue += fee;

    /* Release the parking slot */
    strcpy(slots[slotIndex].regNumber, "---");
    slots[slotIndex].isOccupied = 0;

    printf("  [OK] Slot %d is now available.\n",
           slots[slotIndex].slotNumber);
}

/*
 * printReceipt:
 *   Displays a formatted parking receipt showing
 *   vehicle details, duration, rate, and total fee.
 */
void printReceipt(const char *reg, int slot, int duration, int fee)
{
    printf("\n");
    printLine('=', 50);
    printf("            PARKING RECEIPT\n");
    printLine('=', 50);
    printf("  Vehicle      : %s\n",   reg);
    printf("  Slot         : %d\n",    slot);
    printf("  Duration     : %d hour(s)\n", duration);
    printf("  Rate         : Rs.%d/hour\n", RATE_PER_HOUR);
    printLine('-', 50);
    printf("  TOTAL FEE    : Rs.%d\n", fee);
    printLine('=', 50);
    printf("  Thank you! Drive safely.\n");
    printLine('=', 50);
    printf("\n");
}

/*
 * viewParkingStatus:
 *   Displays all parking slots in a formatted table,
 *   followed by occupancy statistics.
 */
void viewParkingStatus(void)
{
    int   i;
    int   occupied, available;
    float occupancy;

    printf("\n");
    printLine('=', 50);
    printf("     PARKING STATUS\n");
    printLine('=', 50);

    /* Table header */
    printf("  +------+------------------+------------+\n");
    printf("  | Slot | Registration     | Status     |\n");
    printf("  +------+------------------+------------+\n");

    /* Table rows */
    for (i = 0; i < MAX_SLOTS; i++) {
        printf("  | %4d | %-16s | %-10s |\n",
               slots[i].slotNumber,
               slots[i].regNumber,
               slots[i].isOccupied ? "Occupied" : "Available");
    }

    printf("  +------+------------------+------------+\n");

    /* Summary statistics */
    occupied  = countOccupied();
    available = MAX_SLOTS - occupied;
    occupancy = (occupied * 100.0f) / MAX_SLOTS;

    printf("\n  Total Slots : %d\n",   MAX_SLOTS);
    printf("  Occupied    : %d\n",     occupied);
    printf("  Available   : %d\n",     available);
    printf("  Occupancy   : %.1f%%\n", occupancy);
}

/*
 * searchVehicle:
 *   Searches for a vehicle by its registration number
 *   and displays whether it is currently parked and
 *   which slot it occupies.
 */
void searchVehicle(void)
{
    char reg[MAX_REG_LEN + 1];
    int  slotIndex;

    printf("\n");
    printLine('=', 50);
    printf("     SEARCH VEHICLE\n");
    printLine('=', 50);

    if (!readRegistration(reg)) {
        return;
    }

    slotIndex = findVehicle(reg);

    if (slotIndex == -1) {
        printf("\n  [!] Vehicle '%s' is NOT currently parked.\n", reg);
    } else {
        printf("\n  [OK] Vehicle found!\n");
        printf("  Registration : %s\n", reg);
        printf("  Slot         : %d\n", slots[slotIndex].slotNumber);
        printf("  Status       : Currently Parked\n");
    }
}

/*
 * parkingSummary:
 *   Displays a summary of parking operations including
 *   revenue, current occupancy, and transaction count.
 */
void parkingSummary(void)
{
    int   occupied  = countOccupied();
    int   available = MAX_SLOTS - occupied;
    float occupancy = (occupied * 100.0f) / MAX_SLOTS;

    printf("\n");
    printLine('=', 50);
    printf("     PARKING SUMMARY\n");
    printLine('=', 50);
    printf("  Total Revenue             : Rs.%d\n",  totalRevenue);
    printf("  Vehicles Currently Parked : %d\n",      occupied);
    printf("  Available Spaces          : %d\n",      available);
    printf("  Completed Transactions    : %d\n",      transactionCount);
    printf("  Parking Occupancy         : %.1f%%\n",  occupancy);
    printLine('=', 50);
}

/*
 * viewTransactionHistory:
 *   Displays all completed parking transactions in a
 *   formatted table with totals.
 */
void viewTransactionHistory(void)
{
    int i;

    printf("\n");
    printLine('=', 50);
    printf("     TRANSACTION HISTORY\n");
    printLine('=', 50);

    if (transactionCount == 0) {
        printf("\n  No transactions recorded yet.\n");
        return;
    }

    /* Table header */
    printf("  +-----+------------------+------+----------+----------+\n");
    printf("  | No. | Registration     | Slot | Duration |   Fee    |\n");
    printf("  +-----+------------------+------+----------+----------+\n");

    /* Table rows */
    for (i = 0; i < transactionCount; i++) {
        printf("  | %3d | %-16s | %4d | %5d hr | Rs.%5d |\n",
               i + 1,
               history[i].regNumber,
               history[i].slotNumber,
               history[i].duration,
               history[i].fee);
    }

    printf("  +-----+------------------+------+----------+----------+\n");
    printf("\n  Total Revenue : Rs.%d\n", totalRevenue);
}

/* ===================== END OF PROGRAM ===================== */
