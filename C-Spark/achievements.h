#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

#include <stdio.h>
#include <stdlib.h>

#define ACH_MILESTONES_COUNT 3

// Achievement types
typedef enum {
    ACH_FIRST_PROGRAM,
    ACH_FIRST_FUNCTION,
    ACH_COMPLEX_PROGRAM
} AchievementType;

// Achievement structure
typedef struct {
    AchievementType type;
    int unlocked;
    char* description;
} Achievement;

// Player XP structure
typedef struct {
    int xp;
    int level;
} PlayerXP;

// Achievement functions
void initialize_achievements(Achievement* achievements);
void display_achievements(const Achievement* achievements);
void unlock_achievement(Achievement* achievements, AchievementType type);
void save_achievements(const Achievement* achievements, const char* filename);
void load_achievements(Achievement* achievements, const char* filename);

// XP functions
void initialize_xp(PlayerXP* player);
void gain_xp(PlayerXP* player, int amount);
void display_xp(const PlayerXP* player);
void save_xp(const PlayerXP* player, const char* filename);
void load_xp(PlayerXP* player, const char* filename);

#endif // ACHIEVEMENTS_H
