#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_SLOTS 10
#define RATE_PER_HOUR 20
#define MIN_DURATION 1
#define MAX_REG_LEN 20
#define MAX_TRANSACTIONS 100

typedef struct {
  int slotNumber;
  char regNumber[MAX_REG_LEN + 1];
  int isOccupied;
  time_t entryTime;
} ParkingSlot;

typedef struct {
  char regNumber[MAX_REG_LEN + 1];
  int slotNumber, billedHours, actualHours, actualMinutes, fee;
} Transaction;

ParkingSlot slots[MAX_SLOTS];
Transaction history[MAX_TRANSACTIONS];
int transactionCount = 0, totalRevenue = 0;

void printLine(char ch, int len) {
  int i;
  printf("  ");
  for (i = 0; i < len; i++)
    putchar(ch);
  printf("\n");
}

void heading(const char *title) {
  printf("\n");
  printLine('=', 50);
  printf("     %s\n", title);
  printLine('=', 50);
}

void clearInputBuffer(void) {
  int ch;
  while ((ch = getchar()) != '\n' && ch != EOF)
    ;
}

void toUpperCase(char *s) {
  int i;
  for (i = 0; s[i]; i++)
    s[i] = (char)toupper((unsigned char)s[i]);
}

int validateRegistration(const char *reg) {
  int i;
  if (!reg[0])
    return 0;
  for (i = 0; reg[i]; i++)
    if (!isalnum((unsigned char)reg[i]) && reg[i] != '-')
      return 0;
  return 1;
}

int readRegistration(char *reg) {
  char buf[MAX_REG_LEN + 10];
  printf("  Enter registration number: ");
  if (!fgets(buf, sizeof(buf), stdin)) {
    reg[0] = '\0';
    return 0;
  }
  buf[strcspn(buf, "\n")] = '\0';

  if (!buf[0]) {
    printf("\n  [!] Registration number cannot be empty.\n");
    return 0;
  }
  if (strlen(buf) > (size_t)MAX_REG_LEN) {
    printf("\n  [!] Registration too long (max %d chars).\n", MAX_REG_LEN);
    return 0;
  }
  strcpy(reg, buf);
  toUpperCase(reg);
  if (!validateRegistration(reg)) {
    printf(
        "\n  [!] Invalid registration. Use only letters, digits, hyphens.\n");
    return 0;
  }
  return 1;
}

int findVehicle(const char *reg) {
  int i;
  for (i = 0; i < MAX_SLOTS; i++)
    if (slots[i].isOccupied && strcmp(slots[i].regNumber, reg) == 0)
      return i;
  return -1;
}

int findAvailableSlot(void) {
  int i;
  for (i = 0; i < MAX_SLOTS; i++)
    if (!slots[i].isOccupied)
      return i;
  return -1;
}

int countOccupied(void) {
  int i, c = 0;
  for (i = 0; i < MAX_SLOTS; i++)
    if (slots[i].isOccupied)
      c++;
  return c;
}

void initializeSlots(void) {
  int i;
  for (i = 0; i < MAX_SLOTS; i++) {
    slots[i].slotNumber = i + 1;
    strcpy(slots[i].regNumber, "---");
    slots[i].isOccupied = 0;
  }
}

void displayMenu(void) {
  printf("\n");
  printLine('-', 50);
  printf("     MAIN MENU\n");
  printLine('-', 50);
  printf("   [1] Vehicle Entry\n"
         "   [2] Vehicle Exit\n"
         "   [3] View Parking Status\n"
         "   [4] Search Vehicle\n"
         "   [5] Parking Summary\n"
         "   [6] View Transaction History\n"
         "   [0] Exit Application\n");
  printLine('-', 50);
}

int readMenuChoice(void) {
  int choice;
  printf("\n  Enter your choice: ");
  if (scanf("%d", &choice) != 1) {
    clearInputBuffer();
    return -1;
  }
  clearInputBuffer();
  return choice;
}

void vehicleEntry(void) {
  char reg[MAX_REG_LEN + 1];
  int idx, occ;

  heading("VEHICLE ENTRY");
  if (!readRegistration(reg))
    return;
  if (findVehicle(reg) != -1) {
    printf("\n  [!] Vehicle '%s' is already parked.\n", reg);
    return;
  }
  idx = findAvailableSlot();
  if (idx == -1) {
    printf("\n  [!] Parking lot is FULL. No slots available.\n");
    return;
  }

  strcpy(slots[idx].regNumber, reg);
  slots[idx].isOccupied = 1;
  slots[idx].entryTime = time(NULL);

  occ = countOccupied();
  printf("\n  [OK] Vehicle parked successfully!\n");
  printf("  Vehicle       : %s\n", reg);
  printf("  Assigned Slot : %d\n", slots[idx].slotNumber);
  printf("  Entry Time    : %s", ctime(&slots[idx].entryTime));
  printf("\n  Occupied : %d / %d  |  Available : %d\n", occ, MAX_SLOTS,
         MAX_SLOTS - occ);
}

void vehicleExit(void) {
  char reg[MAX_REG_LEN + 1];
  int idx, totalMin, aHrs, aMin, bHrs, fee;
  time_t exitTime;

  heading("VEHICLE EXIT");
  if (!readRegistration(reg))
    return;
  idx = findVehicle(reg);
  if (idx == -1) {
    printf("\n  [!] Vehicle '%s' is not found in the parking lot.\n", reg);
    return;
  }

  exitTime = time(NULL);
  totalMin = (int)(difftime(exitTime, slots[idx].entryTime) / 60);
  aHrs = totalMin / 60;
  aMin = totalMin % 60;
  bHrs = aHrs + (aMin > 0 ? 1 : 0);
  if (bHrs < MIN_DURATION)
    bHrs = MIN_DURATION;
  fee = bHrs * RATE_PER_HOUR;

  printf("  Entry Time   : %s", ctime(&slots[idx].entryTime));
  printf("  Exit Time    : %s", ctime(&exitTime));
  printf("  Duration     : %d hr %d min\n", aHrs, aMin);
  printf("  Billed Hours : %d hour(s)\n", bHrs);

  printLine('=', 50);
  printf("            PARKING RECEIPT\n");
  printLine('=', 50);
  printf("  Vehicle      : %s\n", reg);
  printf("  Slot         : %d\n", slots[idx].slotNumber);
  printf("  Duration     : %d hr %d min\n", aHrs, aMin);
  printf("  Billed Hours : %d hour(s)\n", bHrs);
  printf("  Rate         : Rs.%d/hour\n", RATE_PER_HOUR);
  printLine('-', 50);
  printf("  TOTAL FEE    : Rs.%d\n", fee);
  printLine('=', 50);
  printf("  Thank you! Drive safely.\n");
  printLine('=', 50);

  if (transactionCount < MAX_TRANSACTIONS) {
    strcpy(history[transactionCount].regNumber, reg);
    history[transactionCount].slotNumber = slots[idx].slotNumber;
    history[transactionCount].billedHours = bHrs;
    history[transactionCount].actualHours = aHrs;
    history[transactionCount].actualMinutes = aMin;
    history[transactionCount].fee = fee;
    transactionCount++;
  }
  totalRevenue += fee;

  strcpy(slots[idx].regNumber, "---");
  slots[idx].isOccupied = 0;
  printf("\n  [OK] Slot %d is now available.\n", slots[idx].slotNumber);
}

void viewParkingStatus(void) {
  int i, occ;
  heading("PARKING STATUS");
  printf("  +------+------------------+------------+\n");
  printf("  | Slot | Registration     | Status     |\n");
  printf("  +------+------------------+------------+\n");
  for (i = 0; i < MAX_SLOTS; i++)
    printf("  | %4d | %-16s | %-10s |\n", slots[i].slotNumber,
           slots[i].regNumber, slots[i].isOccupied ? "Occupied" : "Available");
  printf("  +------+------------------+------------+\n");
  occ = countOccupied();
  printf("\n  Total Slots : %d\n  Occupied    : %d\n  Available   : %d\n"
         "  Occupancy   : %.1f%%\n",
         MAX_SLOTS, occ, MAX_SLOTS - occ, (occ * 100.0f) / MAX_SLOTS);
}

void searchVehicle(void) {
  char reg[MAX_REG_LEN + 1];
  int idx;
  heading("SEARCH VEHICLE");
  if (!readRegistration(reg))
    return;
  idx = findVehicle(reg);
  if (idx == -1)
    printf("\n  [!] Vehicle '%s' is NOT currently parked.\n", reg);
  else {
    printf("\n  [OK] Vehicle found!\n");
    printf("  Registration : %s\n  Slot         : %d\n"
           "  Status       : Currently Parked\n",
           reg, slots[idx].slotNumber);
  }
}

void parkingSummary(void) {
  int occ = countOccupied();
  heading("PARKING SUMMARY");
  printf("  Total Revenue             : Rs.%d\n"
         "  Vehicles Currently Parked : %d\n"
         "  Available Spaces          : %d\n"
         "  Completed Transactions    : %d\n"
         "  Parking Occupancy         : %.1f%%\n",
         totalRevenue, occ, MAX_SLOTS - occ, transactionCount,
         (occ * 100.0f) / MAX_SLOTS);
  printLine('=', 50);
}

void viewTransactionHistory(void) {
  int i;
  heading("TRANSACTION HISTORY");
  if (transactionCount == 0) {
    printf("\n  No transactions recorded yet.\n");
    return;
  }
  printf(
      "  "
      "+-----+------------------+------+-------------+--------+----------+\n");
  printf("  | No. | Registration     | Slot | Duration    | Billed |   Fee    "
         "|\n");
  printf(
      "  "
      "+-----+------------------+------+-------------+--------+----------+\n");
  for (i = 0; i < transactionCount; i++)
    printf("  | %3d | %-16s | %4d | %3dhr %2dmin | %3d hr | Rs.%5d |\n", i + 1,
           history[i].regNumber, history[i].slotNumber, history[i].actualHours,
           history[i].actualMinutes, history[i].billedHours, history[i].fee);
  printf(
      "  "
      "+-----+------------------+------+-------------+--------+----------+\n");
  printf("\n  Total Revenue : Rs.%d\n", totalRevenue);
}

int main(void) {
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
    }
  } while (choice != 0);

  return 0;
}
