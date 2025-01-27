#include "achievements.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Update the UI to display rewards
void update_rewards_ui(const Achievement* achievements) {
    printf("=== Rewards Center ===\n");
    for (int i = 0; i < ACH_MILESTONES_COUNT; i++) {
        printf("[%s] %s\n",
            achievements[i].unlocked ? "Unlocked" : "Locked",
            achievements[i].description);
    }
}

// Unlock a specific achievement
void unlock_achievement(Achievement* achievements, AchievementType type) {
    if (type < 0 || type >= ACH_MILESTONES_COUNT) {
        fprintf(stderr, "Error: Invalid achievement type %d\n", type);
        return;
    }
    if (achievements[type].unlocked) {
        fprintf(stderr, "Warning: Achievement %d already unlocked\n", type);
        return;
    }
    achievements[type].unlocked = 1;
    printf("Achievement unlocked: %s\n", achievements[type].description);

    // Optionally, auto-save achievements when one is unlocked
    save_achievements(achievements, "achievements.dat");
}

// Initialize the achievements with default descriptions and locked states
void initialize_achievements(Achievement* achievements) {
    for (int i = 0; i < ACH_MILESTONES_COUNT; i++) {
        achievements[i].type = i;
        achievements[i].unlocked = 0;
        achievements[i].description = (char*)malloc(128 * sizeof(char)); // Allocate memory
    }

    // Use strncpy_s for secure string copying
    strncpy_s(achievements[ACH_FIRST_PROGRAM].description, 128, "First Program", _TRUNCATE);
    strncpy_s(achievements[ACH_FIRST_FUNCTION].description, 128, "First Function", _TRUNCATE);
    strncpy_s(achievements[ACH_COMPLEX_PROGRAM].description, 128, "Complex Program", _TRUNCATE);
}


// Free dynamically allocated memory for descriptions
void free_achievements(Achievement* achievements) {
    for (int i = 0; i < ACH_MILESTONES_COUNT; i++) {
        free(achievements[i].description);
    }
}

// Display all achievements and their statuses
void display_achievements(const Achievement* achievements) {
    printf("=== Achievements ===\n");
    for (int i = 0; i < ACH_MILESTONES_COUNT; i++) {
        printf("[%s] %s\n",
            achievements[i].unlocked ? "Unlocked" : "Locked",
            achievements[i].description);
    }
}

// Save achievements to a file
void save_achievements(const Achievement* achievements, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for saving\n", filename);
        return;
    }
    fwrite(achievements, sizeof(Achievement), ACH_MILESTONES_COUNT, file);
    fclose(file);
    printf("Achievements saved to %s\n", filename);
}

// Load achievements from a file
void load_achievements(Achievement* achievements, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for loading\n", filename);
        return;
    }
    fread(achievements, sizeof(Achievement), ACH_MILESTONES_COUNT, file);
    fclose(file);
    printf("Achievements loaded from %s\n", filename);
}
