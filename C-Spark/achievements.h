#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

typedef enum {
    ACH_FIRST_PROGRAM,
    ACH_FIRST_FUNCTION,
    ACH_COMPLEX_PROGRAM,
    ACH_MILESTONES_COUNT
} AchievementType;

typedef struct {
    AchievementType type;
    const char* description;
    int unlocked;  // 0 = locked, 1 = unlocked
} Achievement;

// Initialize achievement list
void initialize_achievements(Achievement* achievements);

// Unlock achievement
void unlock_achievement(Achievement* achievements, AchievementType type);

// Display unlocked achievements
void display_achievements(const Achievement* achievements);

#endif // ACHIEVEMENTS_H

