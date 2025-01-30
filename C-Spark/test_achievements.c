#include <stdio.h>
#include <assert.h>
#include "achievements.h"
Achievement achievements[ACH_MILESTONES_COUNT];

AchievementType type = ACH_FIRST_PROGRAM;

void test_unlock_achievement() {
    printf("Testing unlock_achievement...\n");
    unlock_achievement(achievements,type);
    printf("-->  unlock_achievement executed (check UI/logs)\n");
}

void test_save_and_load_achievements() {
    printf("Testing save and load achievements...\n");
    save_achievements(achievements,"achievments.txt");
    load_achievements(achievements, "achievments.txt");
    printf("--> save and load achievements executed\n");
}
