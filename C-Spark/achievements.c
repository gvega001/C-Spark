#include "achievements.h"
#include <stdio.h>

void update_rewards_ui(const Achievement* achievements) {
    printf("=== Rewards Center ===\n");
    for (int i = 0; i < ACH_MILESTONES_COUNT; i++) {
        printf("[%s] %s\n",
            achievements[i].unlocked ? "Unlocked" : "Locked",
            achievements[i].description);
    }
}
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
}
void initialize_achievements(Achievement* achievements) {
    for (int i = 0; i < ACH_MILESTONES_COUNT; i++) {
        achievements[i].type = i;
        achievements[i].unlocked = 0;
    }
    achievements[ACH_FIRST_PROGRAM].description = "First Program";
    achievements[ACH_FIRST_FUNCTION].description = "First Function";
    achievements[ACH_COMPLEX_PROGRAM].description = "Complex Program";
}
void display_achievements(const Achievement* achievements) {
    printf("=== Achievements ===\n");
    for (int i = 0; i < ACH_MILESTONES_COUNT; i++) {
        printf("[%s] %s\n",
            achievements[i].unlocked ? "Unlocked" : "Locked",
            achievements[i].description);
    }
}
