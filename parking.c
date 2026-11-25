#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_SLOTS        10
#define RATE_PER_HOUR    20
#define MIN_DURATION     1
#define MAX_REG_LEN      20
#define MAX_TRANSACTIONS 100

typedef struct {
    int    slotNumber;
    char   regNumber[MAX_REG_LEN + 1];
    int    isOccupied;
    time_t entryTime;
} ParkingSlot;

typedef struct {
    char regNumber[MAX_REG_LEN + 1];
    int  slotNumber;
    int  billedHours;
    int  actualHours;   
    int  actualMinutes;  
    int  fee;
} Transaction;

ParkingSlot slots[MAX_SLOTS];
Transaction history[MAX_TRANSACTIONS];
int transactionCount = 0;
int totalRevenue     = 0;

void initializeSlots(void);
void displayMenu(void);
void printLine(char ch, int len);

void clearInputBuffer(void);
int  readMenuChoice(void);
int  readRegistration(char *reg);

int  validateRegistration(const char *reg);
void toUpperCase(char *str);
int  findVehicle(const char *reg);
int  findAvailableSlot(void);
int  countOccupied(void);

void vehicleEntry(void);
void vehicleExit(void);
void viewParkingStatus(void);
void searchVehicle(void);
void parkingSummary(void);
void viewTransactionHistory(void);
void printReceipt(const char *reg, int slot, int actualHrs, int actualMin, int billedHrs, int fee);

int main(void)
{
    int choice;

    initializeSlots();

    printf("\n");
    printLine('=', 50);
    printf("     SMART CAMPUS PARKING SYSTEM\n");
    printf("     Team Nexus | UPES Mini Hackathon\n");
    printLine('=', 50);

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


void initializeSlots(void)
{
    int i;
    for (i = 0; i < MAX_SLOTS; i++) {
        slots[i].slotNumber = i + 1;
        strcpy(slots[i].regNumber, "---");
        slots[i].isOccupied = 0;
    }
}

void printLine(char ch, int len)
{
    int i;
    printf("  ");
    for (i = 0; i < len; i++) {
        putchar(ch);
    }
    printf("\n");
}

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

void clearInputBuffer(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

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

int readRegistration(char *reg)
{
    char buffer[MAX_REG_LEN + 10];

    printf("  Enter registration number: ");

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        reg[0] = '\0';
        return 0;
    }


    buffer[strcspn(buffer, "\n")] = '\0';


    if (strlen(buffer) == 0) {
        printf("\n  [!] Registration number cannot be empty.\n");
        reg[0] = '\0';
        return 0;
    }


    if (strlen(buffer) > (size_t)MAX_REG_LEN) {
        printf("\n  [!] Registration too long (max %d characters).\n",
               MAX_REG_LEN);
        reg[0] = '\0';
        return 0;
    }


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

void toUpperCase(char *str)
{
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        str[i] = (char)toupper((unsigned char)str[i]);
    }
}

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

void vehicleEntry(void)
{
    char reg[MAX_REG_LEN + 1];
    int  slotIndex;
    int  occupied, available;

    printf("\n");
    printLine('=', 50);
    printf("     VEHICLE ENTRY\n");
    printLine('=', 50);


    if (!readRegistration(reg)) {
        return;
    }


    if (findVehicle(reg) != -1) {
        printf("\n  [!] Vehicle '%s' is already parked.\n", reg);
        return;
    }


    slotIndex = findAvailableSlot();
    if (slotIndex == -1) {
        printf("\n  [!] Parking lot is FULL. No slots available.\n");
        return;
    }


    strcpy(slots[slotIndex].regNumber, reg);
    slots[slotIndex].isOccupied = 1;
    slots[slotIndex].entryTime  = time(NULL);  /* Record entry timestamp */


    occupied  = countOccupied();
    available = MAX_SLOTS - occupied;

    printf("\n  [OK] Vehicle parked successfully!\n");
    printf("  Vehicle       : %s\n", reg);
    printf("  Assigned Slot : %d\n", slots[slotIndex].slotNumber);
    printf("  Entry Time    : %s", ctime(&slots[slotIndex].entryTime));
    printf("\n  Occupied : %d / %d  |  Available : %d\n",
           occupied, MAX_SLOTS, available);
}

void vehicleExit(void)
{
    char   reg[MAX_REG_LEN + 1];
    int    slotIndex;
    int    fee;
    time_t exitTime;
    double diffSeconds;
    int    totalMinutes;   /* Total parking time in minutes     */
    int    actualHours;    /* Exact hours portion               */
    int    actualMinutes;  /* Exact minutes portion             */
    int    billedHours;    /* Hours charged (rounded up, min 1) */

    printf("\n");
    printLine('=', 50);
    printf("     VEHICLE EXIT\n");
    printLine('=', 50);


    if (!readRegistration(reg)) {
        return;
    }


    slotIndex = findVehicle(reg);
    if (slotIndex == -1) {
        printf("\n  [!] Vehicle '%s' is not found in the parking lot.\n", reg);
        return;
    }

    /* Automatically calculate duration from entry time */
    exitTime      = time(NULL);
    diffSeconds   = difftime(exitTime, slots[slotIndex].entryTime);
    totalMinutes  = (int)(diffSeconds / 60);     /* Total minutes parked */
    actualHours   = totalMinutes / 60;           /* Exact hours part     */
    actualMinutes = totalMinutes % 60;           /* Exact minutes part   */

    /* Bill by the hour: round up any partial hour, minimum 1 hour */
    billedHours = actualHours;
    if (actualMinutes > 0) {
        billedHours++;   /* Round up partial hour */
    }
    if (billedHours < MIN_DURATION) {
        billedHours = MIN_DURATION;
    }

    printf("  Entry Time   : %s", ctime(&slots[slotIndex].entryTime));
    printf("  Exit Time    : %s", ctime(&exitTime));
    printf("  Duration     : %d hr %d min\n", actualHours, actualMinutes);
    printf("  Billed Hours : %d hour(s)\n", billedHours);

    fee = billedHours * RATE_PER_HOUR;


    printReceipt(reg, slots[slotIndex].slotNumber,
                 actualHours, actualMinutes, billedHours, fee);


    if (transactionCount < MAX_TRANSACTIONS) {
        strcpy(history[transactionCount].regNumber, reg);
        history[transactionCount].slotNumber    = slots[slotIndex].slotNumber;
        history[transactionCount].billedHours    = billedHours;
        history[transactionCount].actualHours    = actualHours;
        history[transactionCount].actualMinutes  = actualMinutes;
        history[transactionCount].fee            = fee;
        transactionCount++;
    } else {
        printf("  [!] Transaction history is full. "
               "Record not saved.\n");
    }


    totalRevenue += fee;


    strcpy(slots[slotIndex].regNumber, "---");
    slots[slotIndex].isOccupied = 0;

    printf("  [OK] Slot %d is now available.\n",
           slots[slotIndex].slotNumber);
}

void printReceipt(const char *reg, int slot,
                  int actualHrs, int actualMin,
                  int billedHrs, int fee)
{
    printf("\n");
    printLine('=', 50);
    printf("            PARKING RECEIPT\n");
    printLine('=', 50);
    printf("  Vehicle      : %s\n",   reg);
    printf("  Slot         : %d\n",    slot);
    printf("  Duration     : %d hr %d min\n", actualHrs, actualMin);
    printf("  Billed Hours : %d hour(s)\n", billedHrs);
    printf("  Rate         : Rs.%d/hour\n", RATE_PER_HOUR);
    printLine('-', 50);
    printf("  TOTAL FEE    : Rs.%d\n", fee);
    printLine('=', 50);
    printf("  Thank you! Drive safely.\n");
    printLine('=', 50);
    printf("\n");
}

void viewParkingStatus(void)
{
    int   i;
    int   occupied, available;
    float occupancy;

    printf("\n");
    printLine('=', 50);
    printf("     PARKING STATUS\n");
    printLine('=', 50);


    printf("  +------+------------------+------------+\n");
    printf("  | Slot | Registration     | Status     |\n");
    printf("  +------+------------------+------------+\n");


    for (i = 0; i < MAX_SLOTS; i++) {
        printf("  | %4d | %-16s | %-10s |\n",
               slots[i].slotNumber,
               slots[i].regNumber,
               slots[i].isOccupied ? "Occupied" : "Available");
    }

    printf("  +------+------------------+------------+\n");


    occupied  = countOccupied();
    available = MAX_SLOTS - occupied;
    occupancy = (occupied * 100.0f) / MAX_SLOTS;

    printf("\n  Total Slots : %d\n",   MAX_SLOTS);
    printf("  Occupied    : %d\n",     occupied);
    printf("  Available   : %d\n",     available);
    printf("  Occupancy   : %.1f%%\n", occupancy);
}

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


    printf("  +-----+------------------+------+-------------+--------+----------+\n");
    printf("  | No. | Registration     | Slot | Duration    | Billed |   Fee    |\n");
    printf("  +-----+------------------+------+-------------+--------+----------+\n");


    for (i = 0; i < transactionCount; i++) {
        printf("  | %3d | %-16s | %4d | %3dhr %2dmin | %3d hr | Rs.%5d |\n",
               i + 1,
               history[i].regNumber,
               history[i].slotNumber,
               history[i].actualHours,
               history[i].actualMinutes,
               history[i].billedHours,
               history[i].fee);
    }

    printf("  +-----+------------------+------+-------------+--------+----------+\n");
    printf("\n  Total Revenue : Rs.%d\n", totalRevenue);
}
