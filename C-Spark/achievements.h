#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

// Define the number of achievement milestones
#define ACH_MILESTONES_COUNT 3

// Enumeration for achievement types
typedef enum {
    ACH_FIRST_PROGRAM,
    ACH_FIRST_FUNCTION,
    ACH_COMPLEX_PROGRAM
} AchievementType;

// Structure for storing an achievement
typedef struct {
    AchievementType type;
    char* description; // Dynamically allocated string
    int unlocked;      // 1 if unlocked, 0 otherwise
} Achievement;

// Function declarations
void update_rewards_ui(const Achievement* achievements);
void unlock_achievement(Achievement* achievements, AchievementType type);
void initialize_achievements(Achievement* achievements);
void free_achievements(Achievement* achievements);
void display_achievements(const Achievement* achievements);
void save_achievements(const Achievement* achievements, const char* filename);
void load_achievements(Achievement* achievements, const char* filename);

#endif // ACHIEVEMENTS_H
