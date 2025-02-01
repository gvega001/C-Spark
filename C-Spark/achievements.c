#include "achievements.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ACH_FILENAME "achievements.dat"
#define XP_FILENAME "xp_data.dat"
#define LEVEL_UP_XP 100  // XP required to level up

// Predefined achievement descriptions
const char* achievement_descriptions[] = {
    "First Program",
    "First Function",
    "Complex Program"
};

// Initialize achievements
void initialize_achievements(Achievement* achievements) {
    for (int i = 0; i < ACH_MILESTONES_COUNT; i++) {
        achievements[i].type = i;
        achievements[i].unlocked = 0;
        achievements[i].description = (char*)achievement_descriptions[i];
    }
}

// Display all achievements
void display_achievements(const Achievement* achievements) {
    printf("=== Achievements ===\n");
    for (int i = 0; i < ACH_MILESTONES_COUNT; i++) {
        printf("[%s] %s\n", achievements[i].unlocked ? "Unlocked" : "Locked", achievements[i].description);
    }
}

// Unlock an achievement
void unlock_achievement(Achievement* achievements, AchievementType type) {
    if (type < 0 || type >= ACH_MILESTONES_COUNT) {
        fprintf(stderr, "Error: Invalid achievement type %d\n", type);
        return;
    }
    if (achievements[type].unlocked) {
        return;
    }
    achievements[type].unlocked = 1;
    printf("Achievement unlocked: %s\n", achievements[type].description);
    save_achievements(achievements, ACH_FILENAME);
}

// Save achievements
void save_achievements(const Achievement* achievements, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for saving\n", filename);
        perror("File Error"); // New: Print system error message
        return;  // Important: Do NOT continue if file opening fails
    }
    size_t written = fwrite(achievements, sizeof(Achievement), ACH_MILESTONES_COUNT, file);
    fclose(file);

    if (written != ACH_MILESTONES_COUNT) {
        fprintf(stderr, "Error: Could not write all achievements to file %s\n", filename);
    }
}

// Load achievements
void load_achievements(Achievement* achievements, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        initialize_achievements(achievements);
        return;
    }
    size_t read_count = fread(achievements, sizeof(Achievement), ACH_MILESTONES_COUNT, file);
    fclose(file);
    if (read_count != ACH_MILESTONES_COUNT) {
        initialize_achievements(achievements);
    }
}

// XP and Level System
void initialize_xp(PlayerXP* player) {
    player->xp = 0;
    player->level = 1;
}

void gain_xp(PlayerXP* player, int amount) {
    player->xp += amount;
    while (player->xp >= LEVEL_UP_XP) {
        player->xp -= LEVEL_UP_XP;
        player->level++;
        printf("Level Up! New Level: %d\n", player->level);
    }
    save_xp(player, XP_FILENAME);
}

void display_xp(const PlayerXP* player) {
    printf("Current Level: %d\n", player->level);
    printf("XP: %d/%d\n", player->xp, LEVEL_UP_XP);
}

// Save XP data
void save_xp(const PlayerXP* player, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for saving XP\n", filename);
        return;
    }
    fwrite(player, sizeof(PlayerXP), 1, file);
    fclose(file);
}

// Load XP data
void load_xp(PlayerXP* player, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        initialize_xp(player);
        return;
    }
    size_t read_count = fread(player, sizeof(PlayerXP), 1, file);
    fclose(file);
    if (read_count != 1) {
        initialize_xp(player);
    }
}
