#include <stdio.h>
#include <assert.h>
#include "error_reporting.h"

void test_report_error() {
    printf("Testing report_error...\n");
    report_error("Syntax Error", 1, 5,  "test_report_error");
    printf("--> report_error executed (check console output)\n");
}

void test_log_error() {
    printf("Testing log_error...\n");
    log_error(2, 3, "Undefined variable");
    printf("--> log_error executed (check console output)\n");
}
