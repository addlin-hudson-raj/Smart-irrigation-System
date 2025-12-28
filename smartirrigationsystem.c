#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h" // Added Raylib support

struct User {
    char username[50];
    char password[50];
};

struct AppState {
    struct User users[100];
    int user_count;
    char logged_in_user[50];
};

struct SoilData {
    char soilType[20];
    float minTemp;
    float maxTemp;
    float minMoist;
    float maxMoist;
};

struct SoilSystem {
    char usernames[200][50];
    char soilTypes[200][50];
    float moisture[200];
    float temperature[200];
    int totalCount;
    char currentUser[50];
};

// =============Sign In & Sign Out=============

void load_users(struct AppState *state) {
    FILE *file = fopen("/Users/gavrielelijah/Documents/Karunya University/I Year/Odd Semester/25CS207-Programming_for_problem_Solving_in_C/MicroProject/MainCode/AllFinalInduvidualCodes/FinalCodes/users.txt", "r");
    if (file == NULL) return;

    while (fscanf(file, "%s %s",
                  (*state).users[(*state).user_count].username,
                  (*state).users[(*state).user_count].password) == 2) {

        (*state).user_count++;

        if ((*state).user_count >= 100)
            break;
    }

    fclose(file);
}

void save_users(struct AppState *state) {
    FILE *file = fopen("/Users/gavrielelijah/Documents/Karunya University/I Year/Odd Semester/25CS207-Programming_for_problem_Solving_in_C/MicroProject/MainCode/AllFinalInduvidualCodes/FinalCodes/users.txt", "w");
    if (file == NULL) {
        printf("Error saving users.\n");
        return;
    }

    for (int i = 0; i < (*state).user_count; i++) {
        fprintf(file, "%s %s\n",
                (*state).users[i].username,
                (*state).users[i].password);
    }

    fclose(file);
}

int find_user(struct AppState *state, const char *username) {
    for (int i = 0; i < (*state).user_count; i++) {
        if (strcmp((*state).users[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

void sign_up(struct AppState *state) {
    char username[50], password[50];

    printf("Enter username: ");
    scanf("%s", username);

    if (find_user(state, username) != -1) {
        printf("Username already exists.\n");
        return;
    }

    printf("Enter password: ");
    scanf("%s", password);

    strcpy((*state).users[(*state).user_count].username, username);
    strcpy((*state).users[(*state).user_count].password, password);

    (*state).user_count++;

    save_users(state);

    printf("Sign up successful.\n");
}

int sign_in(struct AppState *state) {
    char username[50], password[50];

    printf("Enter username: ");
    scanf("%s", username);

    int index = find_user(state, username);
    if (index == -1) {
        printf("Username not found.\n");
        return 0;
    }

    printf("Enter password: ");
    scanf("%s", password);

    if (strcmp((*state).users[index].password, password) == 0) {
        strcpy((*state).logged_in_user, username);
        printf("Welcome, %s!\n", username);
        return 1;
    }

    printf("Incorrect password.\n");
    return 0;
}

void menu(struct AppState *state) {
    int choice;

    while (1) {
        printf("\n1. Sign Up\n2. Sign In\n3. Exit\nChoose an option: ");
        scanf("%d", &choice);

        if (choice == 1) {
            sign_up(state);
        }
        else if (choice == 2) {
            if (sign_in(state)) return;
        }
        else if (choice == 3) {
            return;
        }
        else {
            printf("Invalid option.\n");
        }
    }
}


// =============Decision Making=============
void decisionMaking(char inputType[], float temp, float moist) {
    FILE *file = fopen("/Users/gavrielelijah/Documents/Karunya University/I Year/Odd Semester/25CS207-Programming_for_problem_Solving_in_C/MicroProject/MainCode/AllFinalInduvidualCodes/FinalCodes/ideal_soil_ranges.txt", "r");
    if (!file) {
        printf("Error: Could not open text file.\n");
        return;
    }

    struct SoilData soils[10];
    int count = 0;
    char buffer[100];

    //Skipping Header file safely
    fgets(buffer,sizeof(buffer),file);

    // Read each line from the text file
    while (fscanf(file, "%s %f %f %f %f",
                  soils[count].soilType,
                  &soils[count].minTemp,
                  &soils[count].maxTemp,
                  &soils[count].minMoist,
                  &soils[count].maxMoist) == 5) {
        count++;
    }

    fclose(file);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (strcasecmp(soils[i].soilType, inputType) == 0) {
            found = 1;
            printf("\nChecking ideal ranges for %s soil...\n", soils[i].soilType);

            if (moist < soils[i].minMoist) {
                printf("Moisture below ideal → Switch ON the PUMP\n");
            }
            else if (moist > soils[i].maxMoist) {
                printf("Moisture above ideal → Switch OFF the PUMP (Soil is overwatered)\n");
            }
            else if (temp > soils[i].maxTemp) {
                printf("WARNING: Temperature is too high!\n");

                if (moist < (soils[i].minMoist + soils[i].maxMoist) / 2) {
                    printf("Soil drying fast → Switch ON the PUMP\n");
                } else {
                    printf("Moisture OK → Keep PUMP OFF, monitor conditions\n");
                }
            }
            else if (temp < soils[i].minTemp) {
                printf("Temperature too low → Watering should be reduced\n");

                if (moist < soils[i].minMoist) {
                    printf("But soil is dry → Switch ON the PUMP briefly\n");
                } else {
                    printf("Moisture sufficient → Keep PUMP OFF\n");
                }
            }
            else {
                printf("All conditions are within ideal range → Keep PUMP OFF\n");
            }

            break;
        }
    }

    if (!found)
        printf("Error: Soil type not found in data.\n");
}


// =============Getting Input and Bubble Sort=============

// Print valid soil types from the file
void printValidSoilTypes() {
    FILE *fp = fopen("/Users/gavrielelijah/Documents/Karunya University/I Year/Odd Semester/25CS207-Programming_for_problem_Solving_in_C/MicroProject/MainCode/AllFinalInduvidualCodes/FinalCodes/ideal_soil_ranges.txt", "r");
    if (fp == NULL) return;

    char line[200], firstWord[50];
    int isHeader = 1;

    printf("\nValid soil types are:\n");
    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "%s", firstWord) == 1) {
            if (isHeader) { isHeader = 0; continue; }
            printf("\t%s\n", firstWord);
        }
    }
    fclose(fp);
}

// Check if soil type is valid
int soilTypeValid(char soil[]) {
    FILE *fp = fopen("/Users/gavrielelijah/Documents/Karunya University/I Year/Odd Semester/25CS207-Programming_for_problem_Solving_in_C/MicroProject/MainCode/AllFinalInduvidualCodes/FinalCodes/ideal_soil_ranges.txt", "r");
    if (fp == NULL) return 0;

    char line[200], firstWord[50];
    int found = 0, isHeader = 1;

    while (fgets(line, sizeof(line), fp) != NULL) {
        if (sscanf(line, "%s", firstWord) == 1) {
            if (isHeader) { isHeader = 0; continue; }
            if (strcmp(soil, firstWord) == 0) found = 1;
        }
    }

    fclose(fp);
    return found;
}

// Load existing entries + add new ones
void loadAndAddEntries(struct SoilSystem *sys, struct AppState *state) {
    FILE *fp;
    int n, i;

    // Load existing file data
    fp = fopen("/Users/gavrielelijah/Documents/Karunya University/I Year/Odd Semester/25CS207-Programming_for_problem_Solving_in_C/MicroProject/MainCode/AllFinalInduvidualCodes/FinalCodes/soil_type.txt", "r");
    if (fp != NULL) {
        while (fscanf(fp, "%s %s %f %f",
                      (*sys).usernames[(*sys).totalCount],
                      (*sys).soilTypes[(*sys).totalCount],
                      &(*sys).moisture[(*sys).totalCount],
                      &(*sys).temperature[(*sys).totalCount]) == 4) {

            (*sys).totalCount++;
        }
        fclose(fp);
    }

    // Get username
    strcpy((*sys).currentUser, (*state).logged_in_user);

    printf("\n=== User: %s ===\n", (*sys).currentUser);
    printf("Enter number of new soil entries to add: ");
    scanf("%d", &n);
    if (n < 0) {
        printf("Invalid number of entries.\n");
        return;
    }

    if ((*sys).totalCount + n >= 200) {
        printf("Error: Maximum soil entry limit (200) reached.\n");
        return;
    }


    // Add new entries
    for (i = (*sys).totalCount; i < (*sys).totalCount + n; i++) {

        strcpy((*sys).usernames[i], (*sys).currentUser);

        printf("\n--- Soil Entry %d ---\n", i - (*sys).totalCount + 1);
        
        // Print valid soil types first
        printValidSoilTypes();

        // Soil with validation
        do {
            printf("Enter soil type: ");
            scanf("%s", (*sys).soilTypes[i]);

            if (!soilTypeValid((*sys).soilTypes[i])) {
                printf("Invalid soil type! Try again.\n");
            }

        } while (!soilTypeValid((*sys).soilTypes[i]));

        printf("Enter moisture (%%): ");
        if (scanf("%f", &(*sys).moisture[i]) != 1) {
            printf("Invalid moisture value (non-numeric). Setting moisture = 0.\n");
            (*sys).moisture[i] = 0;
            while (getchar() != '\n');
        }
        else if ((*sys).moisture[i] < 0) {
            printf("Moisture cannot be negative. Setting to 0.\n");
            (*sys).moisture[i] = 0;
        }
        else if ((*sys).moisture[i] > 100) {
            printf("Moisture too high (must be 0–100). Capping at 100.\n");
            (*sys).moisture[i] = 100;
        }

        printf("Enter temperature (Celsius): ");
        if (scanf("%f", &(*sys).temperature[i]) != 1) {
            printf("Invalid temperature (non-numeric). Setting temperature = 0.\n");
            (*sys).temperature[i] = 0;
            while (getchar() != '\n');
        }
        else if ((*sys).temperature[i] < -10) {
            printf("Temperature too low (must be >= -10). Setting to -10.\n");
            (*sys).temperature[i] = -10;
        }
        else if ((*sys).temperature[i] > 60) {
            printf("Temperature too high (must be <= 60). Setting to 60.\n");
            (*sys).temperature[i] = 60;
        }

        decisionMaking((*sys).soilTypes[i], (*sys).temperature[i], (*sys).moisture[i]);

    }

    (*sys).totalCount += n;

    // Write back to file
    fp = fopen("/Users/gavrielelijah/Documents/Karunya University/I Year/Odd Semester/25CS207-Programming_for_problem_Solving_in_C/MicroProject/MainCode/AllFinalInduvidualCodes/FinalCodes/soil_type.txt", "w");
    if (fp == NULL) {
        printf("Error writing file.\n");
        return;
    }

    for (i = 0; i < (*sys).totalCount; i++) {
        fprintf(fp, "%s %s %.2f %.2f\n",
                (*sys).usernames[i],
                (*sys).soilTypes[i],
                (*sys).moisture[i],
                (*sys).temperature[i]);
    }

    fclose(fp);
}

// Show current user's data sorted by moisture
void displaySortedData(struct SoilSystem *sys) {

    int i, j, count = 0;
    char userSoil[200][50], tempName[50];
    float userMoist[200], userTemp[200];
    float t1, t2;

    // Extract entries belonging to current user
    for (i = 0; i < (*sys).totalCount; i++) {
        if (strcmp((*sys).usernames[i], (*sys).currentUser) == 0) {

            strcpy(userSoil[count], (*sys).soilTypes[i]);
            userMoist[count] = (*sys).moisture[i];
            userTemp[count] = (*sys).temperature[i];
            count++;
        }
    }

    // Bubble sort by moisture
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (userMoist[j] > userMoist[j + 1]) {

                t1 = userMoist[j];
                userMoist[j] = userMoist[j + 1];
                userMoist[j + 1] = t1;

                t2 = userTemp[j];
                userTemp[j] = userTemp[j + 1];
                userTemp[j + 1] = t2;

                strcpy(tempName, userSoil[j]);
                strcpy(userSoil[j], userSoil[j + 1]);
                strcpy(userSoil[j + 1], tempName);
            }
        }
    }

    // Print sorted results
    printf("\n=== Data for %s Sorted by Moisture ===\n", (*sys).currentUser);
    for (i = 0; i < count; i++) {
        printf("%s   Moisture: %.2f%%   Temp: %.2f°C\n",
               userSoil[i], userMoist[i], userTemp[i]);
    }
}

// ---------------- Raylib Scatter Plot Function ----------------
// ---------------- Raylib Scatter Plot Function with Axes ----------------
void showScatterPlot(struct SoilSystem sys) {
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Moisture vs Temperature");

    int marginLeft = 50;
    int marginBottom = 50;
    int marginTop = 50;
    int marginRight = 50;

    int plotWidth = screenWidth - marginLeft - marginRight;
    int plotHeight = screenHeight - marginTop - marginBottom;

    // Determine max temp and moisture to scale points
    float maxTemp = -1000, maxMoist = -1000;
    for (int i = 0; i < sys.totalCount; i++) {
        if (strcmp(sys.usernames[i], sys.currentUser) == 0) {
            if (sys.temperature[i] > maxTemp) maxTemp = sys.temperature[i];
            if (sys.moisture[i] > maxMoist) maxMoist = sys.moisture[i];
        }
    }
    if (maxTemp < 10) maxTemp = 10;
    if (maxMoist < 10) maxMoist = 10;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw axes
        DrawLine(marginLeft, screenHeight - marginBottom, screenWidth - marginRight, screenHeight - marginBottom, WHITE); // X-axis
        DrawLine(marginLeft, marginTop, marginLeft, screenHeight - marginBottom, WHITE); // Y-axis

        // Draw axis labels
        DrawText("Temperature (°C)", screenWidth/2 - 50, screenHeight - 30, 15, WHITE);
        DrawText("Moisture (%)", 5, marginTop - 20, 15, WHITE);

        // Draw X-axis ticks and numbers
        for (int t = 0; t <= (int)maxTemp; t += 5) {
            int x = marginLeft + (int)(plotWidth * t / maxTemp);
            DrawLine(x, screenHeight - marginBottom - 5, x, screenHeight - marginBottom + 5, WHITE);
            char tempLabel[10];
            sprintf(tempLabel, "%d", t);
            DrawText(tempLabel, x - 10, screenHeight - marginBottom + 8, 10, WHITE);
        }

        // Draw Y-axis ticks and numbers
        for (int m = 0; m <= (int)maxMoist; m += 5) {
            int y = screenHeight - marginBottom - (int)(plotHeight * m / maxMoist);
            DrawLine(marginLeft - 5, y, marginLeft + 5, y, WHITE);
            char moistLabel[10];
            sprintf(moistLabel, "%d", m);
            DrawText(moistLabel, 10, y - 5, 10, WHITE);
        }

        // Plot data points
        for (int i = 0; i < sys.totalCount; i++) {
            if (strcmp(sys.usernames[i], sys.currentUser) == 0) {
                int x = marginLeft + (int)(plotWidth * sys.temperature[i] / maxTemp);
                int y = screenHeight - marginBottom - (int)(plotHeight * sys.moisture[i] / maxMoist);
                DrawCircle(x, y, 5, GREEN);
                DrawText(sys.soilTypes[i], x + 8, y - 8, 10, WHITE);
            }
        }

        EndDrawing();
    }

    CloseWindow();
}


// ---------------- Updated displaySortedData ----------------
void displaySortedDataWithPlot(struct SoilSystem sys){
    int i, j, count = 0;
    char userSoil[200][50], tempName[50];
    float userMoist[200], userTemp[200];
    float t1, t2;

    // Extract entries belonging to current user
    for (i = 0; i < sys.totalCount; i++) {
        if (strcmp(sys.usernames[i], sys.currentUser) == 0) {
            strcpy(userSoil[count], sys.soilTypes[i]);
            userMoist[count] = sys.moisture[i];
            userTemp[count] = sys.temperature[i];
            count++;
        }
    }

    // Bubble sort by moisture
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (userMoist[j] > userMoist[j + 1]) {
                t1 = userMoist[j]; userMoist[j] = userMoist[j + 1]; userMoist[j + 1] = t1;
                t2 = userTemp[j];  userTemp[j] = userTemp[j + 1];  userTemp[j + 1] = t2;
                strcpy(tempName, userSoil[j]); strcpy(userSoil[j], userSoil[j + 1]); strcpy(userSoil[j + 1], tempName);
            }
        }
    }

    // Print sorted results
    printf("\n=== Data for %s Sorted by Moisture ===\n", sys.currentUser);
    for (i = 0; i < count; i++)
        printf("%s   Moisture: %.2f%%   Temp: %.2f°C\n", userSoil[i], userMoist[i], userTemp[i]);

    // Show scatter plot using Raylib
    showScatterPlot(sys);
}

// ========================== Main ==========================
int main() {
    struct AppState state;
    state.user_count = 0;
    strcpy(state.logged_in_user, "");

    struct SoilSystem sys;
    sys.totalCount = 0;

    load_users(&state);
    menu(&state);

    if (strlen(state.logged_in_user) != 0) {
        printf("Now running the next part of your program for %s...\n", state.logged_in_user);
        loadAndAddEntries(&sys, &state);

        // Use new function to show sorted data + plot
        displaySortedDataWithPlot(sys);
    }

    printf("\n\n==================Thank You==================");

    return 0;
}
