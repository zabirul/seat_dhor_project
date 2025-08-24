#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 1000
#define MAX_SHUTTLES 100
#define MAX_BOOKINGS 1000

// User structure
struct User {
    char student_id[20];
    char password[20];
};

struct Admin {
    char username[20];
    char password[20];
    char passkey[20];
};

struct Shuttle {
    char bus_number[10];
    char route[50];
    char departure_time[10];
    char departure_date[12];
};

struct Booking {
    char student_id[20];
    char bus_number[10];
    char token[15];
    char date[12];
    char time[10];
};

// Function declarations
void showLoginMenu();
void userSignup();
void userLogin();
void forgotPassword();
void adminLogin();
void studentMenu(const char *student_id);
void adminMenu();
int isValidStudentID(const char *id);
int isValidPassword(const char *pass);
void viewAllShuttles();
void nextAvailableShuttle(const char *student_id);
void bookSeat(const char *student_id, const char *bus_num, const char *date, const char *time);
void addShuttle();
void editShuttle();
void removeShuttle();
void viewBookings();
void resetallinfo();
void changeAdminCredentials();
int generateToken(char *token);
int isBooked(const char *student_id);

// Utility: Clear input buffer
void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Utility: Wait for input to continue
void waitForInput() {
    printf("\nPress Enter to continue...");
    getchar();
}

// ===== Main Function =====
int main() {

    FILE *fp = fopen("admin.txt", "r");
    if (!fp) {
        fp = fopen("admin.txt", "w");
        fprintf(fp, "admin\nadmin123\nsecret123\n");  // default: user=admin, pass=admin123, key=secret123
        fclose(fp);
        printf("Admin file created with default credentials.\n");
    } else {
        fclose(fp);
    }

    // Create empty files if not exist
    fopen("users.txt", "a");
    fopen("shuttle.txt", "a");
    fopen("booking.txt", "a");

    showLoginMenu();
    return 0;
}

// ===== Menu Display =====
void showLoginMenu() {
    int choice;
    while (1) {
        printf("\n+---------------------------------------+\n");
        printf("|       Seat Dhor - Log In / Sign Up    |\n");
        printf("+---------------------------------------+\n");
        printf("|                                       |\n");
        printf("|                                       |\n");
        printf("| 1. Log In                             |\n");
        printf("| 2. Sign Up                            |\n");
        printf("| 3. Forgot Password                    |\n");
        printf("| 4. Login as Admin                     |\n");
        printf("| 5. Exit                               |\n");
        printf("|                                       |\n");
        printf("|                                       |\n");
        printf("+---------------------------------------+\n");
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Try again.\n");
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch (choice) {
            case 1: system("cls"); userLogin(); break;
            case 2: system("cls"); userSignup(); break;
            case 3: system("cls"); forgotPassword(); break;
            case 4: system("cls"); adminLogin(); break;
            case 5: printf(" Exiting...\n"); exit(0);
            default: system("cls"); printf(" Invalid choice. Try again.\n");
        }
    }
}

// ===== Validate Student ID =====
int isValidStudentID(const char *id) {
    int len = strlen(id);
    if (len != 7) return 0;

    int start;


    if (id[0] == 'C' || id[0] == 'T' || id[0] == 'E') {
        if (id[1] != '2') return 0;
        start = 2;
    }
    else if (id[0] == 'C' && id[1] == 'E') {
        if (id[2] != '2') return 0;
        start = 3;
    }
    else if (id[0] == 'T' && id[1] == 'E') {
        if (id[2] != '2') return 0;
        start = 3;
    }
    else {
        return 0;
    }


    if (id[start+0] < '0' || id[start+0] > '5') return 0;
    if (id[start+1] < '1' || id[start+1] > '2') return 0;
    if (id[start+2] < '0' || id[start+2] > '2') return 0;
    if (id[start+3] < '0' || id[start+3] > '9') return 0;
    if (id[start+4] < '0' || id[start+4] > '9') return 0;

    return 1;
}



// ===== Validate Password =====
int isValidPassword(const char *pass) {
    return strlen(pass) == 8;
}

// ===== Sign Up =====
void userSignup() {
    struct User user;
    FILE *fp;

    printf("Enter Student ID (7 chars, format: C/TE/T/E/CE + 2 + digits): ");
    fgets(user.student_id, sizeof(user.student_id), stdin);
    user.student_id[strcspn(user.student_id, "\n")] = '\0';

    if (!isValidStudentID(user.student_id)) {
        printf("Invalid Student ID format.\n");
        waitForInput();
        return;
    }

    printf("Enter Password (exactly 8 characters): ");
    fgets(user.password, sizeof(user.password), stdin);
    user.password[strcspn(user.password, "\n")] = '\0';

    if (!isValidPassword(user.password)) {
        printf("Password must be exactly 8 characters.\n");
        waitForInput();
        return;
    }

    // Check if student ID already exists
    fp = fopen("users.txt", "r");
    if (fp) {
        struct User temp;
        while (fscanf(fp, "%[^,],%[^\n]\n", temp.student_id, temp.password) == 2) {
            if (strcmp(temp.student_id, user.student_id) == 0) {
                fclose(fp);
                printf(" Student ID already exists.\n");
                waitForInput();
                return;
            }
        }
        fclose(fp);
    }

    fp = fopen("users.txt", "a");
    fprintf(fp, "%s,%s\n", user.student_id, user.password);
    fclose(fp);

    printf("Signup Successful!\n");
    waitForInput();
}

// ===== Log In =====
void userLogin() {
    char id[20], pass[20];
    struct User user;
    int found = 0;

    printf("Enter Student ID: ");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = '\0';

    printf("Enter Password: ");
    fgets(pass, sizeof(pass), stdin);
    pass[strcspn(pass, "\n")] = '\0';

    FILE *fp = fopen("users.txt", "r");
    if (!fp) {
        printf("Error open user file.\n");
        waitForInput();
        return;
    }

    while (fscanf(fp, "%[^,],%[^\n]\n", user.student_id, user.password) == 2) {
        if (strcmp(id, user.student_id) == 0 && strcmp(pass, user.password) == 0) {
            found = 1;
            break;
        }
    }
    fclose(fp);

    if (found) {
        printf(" Login Successful!\n");
        waitForInput();
        studentMenu(id);
    } else {
        printf(" Invalid ID or Password.\n");
        waitForInput();
    }
}

// ===== Forgot Password =====
void forgotPassword() {
    char id[20], newpass[20];
    struct User users[MAX_USERS];
    int count = 0, found = 0;

    printf("Enter Student ID: ");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = '\0';

    FILE *fp = fopen("users.txt", "r");
    if (!fp) {
        printf(" File not found.\n");
        waitForInput();
        return;
    }

    while (fscanf(fp, "%[^,],%[^\n]\n", users[count].student_id, users[count].password) == 2) {
        count++;
    }
    fclose(fp);

    for (int i = 0; i < count; i++) {
        if (strcmp(users[i].student_id, id) == 0) {
            found = 1;
            printf("Enter new 8-character password: ");
            fgets(newpass, sizeof(newpass), stdin);
            newpass[strcspn(newpass, "\n")] = '\0';

            if (!isValidPassword(newpass)) {
                printf(" Password must be 8 characters.\n");
                waitForInput();
                return;
            }

            strcpy(users[i].password, newpass);
            break;
        }
    }

    if (!found) {
        printf(" Student ID not found.\n");
        waitForInput();
        return;
    }

    fp = fopen("users.txt", "w");
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s\n", users[i].student_id, users[i].password);
    }
    fclose(fp);

    printf(" Password updated successfully!\n");
    waitForInput();
}

// ===== Admin Login =====
void adminLogin() {
    char user[20], pass[20];
    struct Admin admin;

    FILE *fp = fopen("admin.txt", "r");
    if (!fp) {
        printf(" Admin file missing.\n");
        waitForInput();
        return;
    }
    fgets(admin.username, sizeof(admin.username), fp);
    fgets(admin.password, sizeof(admin.password), fp);
    fgets(admin.passkey, sizeof(admin.passkey), fp);
    fclose(fp);

    admin.username[strcspn(admin.username, "\n")] = '\0';
    admin.password[strcspn(admin.password, "\n")] = '\0';
    admin.passkey[strcspn(admin.passkey, "\n")] = '\0';

    printf("Enter Admin Username: ");
    fgets(user, sizeof(user), stdin);
    user[strcspn(user, "\n")] = '\0';

    printf("Enter Password: ");
    fgets(pass, sizeof(pass), stdin);
    pass[strcspn(pass, "\n")] = '\0';

    if (strcmp(user, admin.username) == 0 && strcmp(pass, admin.password) == 0) {
        printf(" Admin Login Successful!\n");
        waitForInput();
        adminMenu();
    } else {
        printf(" Invalid admin credentials.\n");
        waitForInput();
    }
}

// ===== Student Menu =====
void studentMenu(const char *student_id) {
    int choice;
    while (1) {
        printf("\n+----------------------------------+\n");
        printf("|            Student Menu            |\n");
        printf("+------------------------------------+\n");
        printf("|                                    |\n");
        printf("| 1. View All Shuttle Schedules      |\n");
        printf("| 2. Next Available Shuttle          |\n");
        printf("| 3. Book a Seat                     |\n");
        printf("| 4. Logout                          |\n");
        printf("|                                    |\n");
        printf("+------------------------------------+\n");
        printf("Enter choice (4 to logout): ");
        if (scanf("%d", &choice) != 1) {
            printf(" Invalid input.\n");
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch (choice) {
            case 1:
                system("cls"); viewAllShuttles();
                break;
            case 2:
                system("cls"); nextAvailableShuttle(student_id);
                break;
            case 3: {
                system("cls");
                char bus[10], date[12], time[10];
                printf("Enter Bus Number: ");
                fgets(bus, sizeof(bus), stdin);
                bus[strcspn(bus, "\n")] = '\0';

                printf("Enter Date (DD-MM-YYYY): ");
                fgets(date, sizeof(date), stdin);
                date[strcspn(date, "\n")] = '\0';

                printf("Enter Time (HH:MM): ");
                fgets(time, sizeof(time), stdin);
                time[strcspn(time, "\n")] = '\0';

                bookSeat(student_id, bus, date, time);
                break;
            }
            case 4:
                system("cls");
                printf(" Logging out...\n");
                waitForInput();
                return;
            default:
                system("cls");
                printf(" Invalid choice.\n");
        }
    }
}

// ===== View All Shuttles =====
void viewAllShuttles() {
    struct Shuttle shuttle;
    FILE *fp = fopen("shuttle.txt", "r");
    if (!fp) {
        printf(" No shuttles available.\n");
        waitForInput();
        return;
    }

    printf("\n%-10s %-20s %-10s %-12s\n", "Bus No", "Route", "Time", "Date");
    printf("%-10s %-20s %-10s %-12s\n", "------", "-----", "----", "----");
    while (fscanf(fp, "%[^,],%[^,],%[^,],%[^\n]\n", shuttle.bus_number, shuttle.route,
                  shuttle.departure_time, shuttle.departure_date) == 4) {
        printf("%-10s %-20s %-10s %-12s\n", shuttle.bus_number, shuttle.route,
               shuttle.departure_time, shuttle.departure_date);
    }
    fclose(fp);
    waitForInput();
}

// ===== Next Available Shuttle =====
void nextAvailableShuttle(const char *student_id) {
    struct Shuttle shuttle;
    FILE *fp = fopen("shuttle.txt", "r");
    if (!fp) {
        printf(" No shuttles available.\n");
        waitForInput();
        return;
    }

    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char current_date[12], current_time[10];
    sprintf(current_date, "%02d-%02d-%d", local->tm_mday, local->tm_mon + 1, local->tm_year + 1900);
    sprintf(current_time, "%02d:%02d", local->tm_hour, local->tm_min);

    printf("\nNext Available Shuttle(s):\n");
    printf("%-10s %-20s %-10s %-12s\n", "Bus No", "Route", "Time", "Date");
    printf("%-10s %-20s %-10s %-12s\n", "------", "-----", "----", "----");

    int found = 0;
    while (fscanf(fp, "%[^,],%[^,],%[^,],%[^\n]\n", shuttle.bus_number, shuttle.route,
                  shuttle.departure_time, shuttle.departure_date) == 4) {
        if (strcmp(shuttle.departure_date, current_date) > 0 ||
            (strcmp(shuttle.departure_date, current_date) == 0 &&
             strcmp(shuttle.departure_time, current_time) >= 0)) {
            printf("%-10s %-20s %-10s %-12s\n", shuttle.bus_number, shuttle.route,
                   shuttle.departure_time, shuttle.departure_date);
            found = 1;
        }
    }
    fclose(fp);

    if (!found) {
        printf("No upcoming shuttles.\n");
    }
    waitForInput();
}

// ===== Book Seat =====
void bookSeat(const char *student_id, const char *bus_num, const char *date, const char *time) {
    if (isBooked(student_id)) {
        printf(" You can only book one seat.\n");
        waitForInput();
        return;
    }

    FILE *fp = fopen("booking.txt", "a");
    if (!fp) {
        printf(" Could not save booking.\n");
        return;
    }

    char token[15];
    generateToken(token);

    fprintf(fp, "%s,%s,%s,%s,%s\n", student_id, bus_num, token, date, time);
    fclose(fp);

    printf(" Seat booked successfully!\n");
    printf(" Confirmation Token: %s\n", token);
    printf("You can show this token as proof.\n");
    waitForInput();
}

// ===== Check if already booked =====
int isBooked(const char *student_id) {
    struct Booking b;
    FILE *fp = fopen("booking.txt", "r");
    if (!fp) return 0;

    while (fscanf(fp, "%[^,],%[^,],%[^,],%[^,],%[^\n]\n", b.student_id, b.bus_number,
                  b.token, b.date, b.time) == 5) {
        if (strcmp(b.student_id, student_id) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// ===== Generate Token (e.g., B123456) =====
int generateToken(char *token) {
    srand(time(NULL));
    sprintf(token, "B%06d", rand() % 900000 + 100000);
    return 1;
}

// ===== Admin Menu =====
void adminMenu() {
    int choice;
    while (1) {
        printf("\n+----------------------------+\n");
        printf("|         Admin Menu         |\n");
        printf("+----------------------------+\n");
        printf("| 1. Add New Shuttle         |\n");
        printf("| 2. Edit Shuttle            |\n");
        printf("| 3. Remove Shuttle          |\n");
        printf("| 4. View Bookings           |\n");
        printf("| 5. Change Admin Credentials|\n");
        printf("| 6. Reset All Info          |\n");
        printf("| 7. Logout                  |\n");
        printf("+----------------------------+\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
         if(choice==0 || choice>7){
            printf(" Invalid input.\n");
            clearBuffer();
            continue;
        }
        clearBuffer();

        switch (choice) {
            case 1: system("cls"); addShuttle(); break;
            case 2: system("cls"); editShuttle(); break;
            case 3: system("cls"); removeShuttle(); break;
            case 4: system("cls"); viewBookings(); break;
            case 5: system("cls"); changeAdminCredentials(); break;
            case 6: system("cls"); resetallinfo(); break;
            case 7:
                system("cls");
                printf(" Admin logged out.\n");
                waitForInput();
                return;
            default:
                system("cls");
                printf(" Invalid choice.\n");
        }
    }
}

// ===== Add Shuttle =====
void addShuttle() {
    struct Shuttle s;
    FILE *fp = fopen("shuttle.txt", "a");

    printf("Bus Number: ");
    fgets(s.bus_number, sizeof(s.bus_number), stdin);
    s.bus_number[strcspn(s.bus_number, "\n")] = '\0';

    printf("Route (e.g., Campus-Airport): ");
    fgets(s.route, sizeof(s.route), stdin);
    s.route[strcspn(s.route, "\n")] = '\0';

    printf("Departure Time (HH:MM): ");
    fgets(s.departure_time, sizeof(s.departure_time), stdin);
    s.departure_time[strcspn(s.departure_time, "\n")] = '\0';

    printf("Departure Date (DD-MM-YYYY): ");
    fgets(s.departure_date, sizeof(s.departure_date), stdin);
    s.departure_date[strcspn(s.departure_date, "\n")] = '\0';

    fprintf(fp, "%s,%s,%s,%s\n", s.bus_number, s.route, s.departure_time, s.departure_date);
    fclose(fp);
    printf(" Shuttle added.\n");
    waitForInput();
}

// ===== Edit Shuttle =====
void editShuttle() {
    struct Shuttle shuttles[MAX_SHUTTLES];
    int count = 0;
    char bus[10];

    FILE *fp = fopen("shuttle.txt", "r");
    if (!fp) {
        printf(" No shuttles to edit.\n");
        waitForInput();
        return;
    }

    while (fscanf(fp, "%[^,],%[^,],%[^,],%[^\n]\n", shuttles[count].bus_number,
                  shuttles[count].route, shuttles[count].departure_time,
                  shuttles[count].departure_date) == 4) {
        count++;
    }
    fclose(fp);

    printf("Enter Bus Number to edit: ");
    fgets(bus, sizeof(bus), stdin);
    bus[strcspn(bus, "\n")] = '\0';

    int found = -1;
    for (int i = 0; i < count; i++) {
        if (strcmp(shuttles[i].bus_number, bus) == 0) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf(" Bus not found.\n");
        waitForInput();
        return;
    }

    printf("New Route: ");
    fgets(shuttles[found].route, sizeof(shuttles[found].route), stdin);
    shuttles[found].route[strcspn(shuttles[found].route, "\n")] = '\0';

    printf("New Time (HH:MM): ");
    fgets(shuttles[found].departure_time, sizeof(shuttles[found].departure_time), stdin);
    shuttles[found].departure_time[strcspn(shuttles[found].departure_time, "\n")] = '\0';

    printf("New Date (DD-MM-YYYY): ");
    fgets(shuttles[found].departure_date, sizeof(shuttles[found].departure_date), stdin);
    shuttles[found].departure_date[strcspn(shuttles[found].departure_date, "\n")] = '\0';

    fp = fopen("shuttle.txt", "w");
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%s,%s,%s,%s\n", shuttles[i].bus_number, shuttles[i].route,
                shuttles[i].departure_time, shuttles[i].departure_date);
    }
    fclose(fp);

    printf(" Shuttle updated.\n");
    waitForInput();
}

// ===== Remove Shuttle =====
void removeShuttle() {
    struct Shuttle shuttles[MAX_SHUTTLES];
    int count = 0;
    char bus[10];

    FILE *fp = fopen("shuttle.txt", "r");
    if (!fp) {
        printf(" No shuttles.\n");
        waitForInput();
        return;
    }

    while (fscanf(fp, "%[^,],%[^,],%[^,],%[^\n]\n", shuttles[count].bus_number,
                  shuttles[count].route, shuttles[count].departure_time,
                  shuttles[count].departure_date) == 4) {
        count++;
    }
    fclose(fp);

    printf("Enter Bus Number to remove: ");
    fgets(bus, sizeof(bus), stdin);
    bus[strcspn(bus, "\n")] = '\0';

    fp = fopen("shuttle.txt", "w");
    int removed = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(shuttles[i].bus_number, bus) != 0) {
            fprintf(fp, "%s,%s,%s,%s\n", shuttles[i].bus_number, shuttles[i].route,
                    shuttles[i].departure_time, shuttles[i].departure_date);
        } else {
            removed = 1;
        }
    }
    fclose(fp);

    if (removed) {
        printf(" Shuttle removed.\n");
    } else {
        printf(" Bus not found.\n");
    }
    waitForInput();
}

// ===== View Bookings =====
void viewBookings() {
    struct Booking b;
    FILE *fp = fopen("booking.txt", "r");
    if (!fp) {
        printf(" No bookings yet.\n");
        waitForInput();
        return;
    }

    printf("\n%-12s %-10s %-12s %-12s %-10s\n", "Student ID", "Bus", "Token", "Date", "Time");
    printf("%-12s %-10s %-12s %-12s %-10s\n", "----------", "---", "-----", "----", "----");
    while (fscanf(fp, "%[^,],%[^,],%[^,],%[^,],%[^\n]\n", b.student_id, b.bus_number,
                  b.token, b.date, b.time) == 5) {
        printf("%-12s %-10s %-12s %-12s %-10s\n", b.student_id, b.bus_number,
               b.token, b.date, b.time);
    }
    fclose(fp);
    waitForInput();
}

// ===== Reset All Info (With Passkey) =====
void resetallinfo() {
    struct Admin admin;
    char passkey[20];

    // Read admin passkey
    FILE *fp = fopen("admin.txt", "r");
    if (!fp) {
        printf(" Admin file missing.\n");
        waitForInput();
        return;
    }

    // Skip username and password, read passkey
    char temp[50];
    fgets(temp, sizeof(temp), fp); // username
    fgets(temp, sizeof(temp), fp); // password
    fgets(admin.passkey, sizeof(admin.passkey), fp);
    fclose(fp);

    admin.passkey[strcspn(admin.passkey, "\n")] = '\0';

    printf("WARNING: This will delete ALL data (shuttles, bookings).\n");
    printf("Enter Secret Passkey to confirm reset: ");
    fgets(passkey, sizeof(passkey), stdin);
    passkey[strcspn(passkey, "\n")] = '\0';

    if (strcmp(passkey, admin.passkey) != 0) {
        printf(" Wrong passkey. Reset aborted.\n");
        waitForInput();
        return;
    }

    // Confirm action
    char confirm[10];
    printf("Type 'YES' to confirm: ");
    fgets(confirm, sizeof(confirm), stdin);
    confirm[strcspn(confirm, "\n")] = '\0';

    if (strcmp(confirm, "YES") != 0) {
        printf(" Reset canceled.\n");
        waitForInput();
        return;
    }

    // Clear all data files accept user


    fp = fopen("shuttle.txt", "w");
    if (fp) fclose(fp);

    fp = fopen("booking.txt", "w");
    if (fp) fclose(fp);



    printf(" All data has been reset!\n");

    waitForInput();
}

// ===== Change Admin Credentials =====
void changeAdminCredentials() {
    struct Admin admin;
    char passkey[20];

    FILE *fp = fopen("admin.txt", "r");
    if (!fp) {
        printf(" Admin file missing.\n");
        waitForInput();
        return;
    }
    fgets(admin.username, sizeof(admin.username), fp);
    fgets(admin.password, sizeof(admin.password), fp);
    fgets(admin.passkey, sizeof(admin.passkey), fp);
    fclose(fp);

    admin.username[strcspn(admin.username, "\n")] = '\0';
    admin.password[strcspn(admin.password, "\n")] = '\0';
    admin.passkey[strcspn(admin.passkey, "\n")] = '\0';

    printf("Enter Secret Passkey: ");
    fgets(passkey, sizeof(passkey), stdin);
    passkey[strcspn(passkey, "\n")] = '\0';

    if (strcmp(passkey, admin.passkey) != 0) {
        printf(" Wrong passkey.\n");
        waitForInput();
        return;
    }

    printf("New Username: ");
    fgets(admin.username, sizeof(admin.username), stdin);
    admin.username[strcspn(admin.username, "\n")] = '\0';

    printf("New Password: ");
    fgets(admin.password, sizeof(admin.password), stdin);
    admin.password[strcspn(admin.password, "\n")] = '\0';

    fp = fopen("admin.txt", "w");
    fprintf(fp, "%s\n%s\n%s\n", admin.username, admin.password, admin.passkey);
    fclose(fp);

    printf(" Admin credentials updated.\n");
    waitForInput();
}
