#undef NDEBUG

#ifndef DEBUG
#define DEBUG
#endif

#ifndef _minunit_h
#define _minunit_h

#include <stdio.h>
#include <stdlib.h>
#include <gba.h>

#include "../source/debug.h"

#define debug debug_print

#define mu_suite_start() char *message = NULL

#define mu_assert(test, message) if (!(test)) { debug("[ERROR] %s\n", message); return message; }
#define mu_run_test(test) debug("\n-----%s\n", " " #test); \
    message = test(); tests_run++; if (message) return message;

#define RUN_TESTS(name) int main(void) {\
		irqInit();\
		irqEnable(IRQ_VBLANK);\
		consoleDemoInit();\
		iprintf("\x1b[10;10H%s", "RUNNING TESTS...");\
        debug("----\nRUNNING TESTS\n", "");\
        char *result = name();\
        if (result != 0) {\
            debug("FAILED: %s\n", result);\
        }\
        else {\
            debug("\nALL TESTS PASSED\n", "");\
        }\
    	debug("Tests run: %d\n", tests_run);\
		debug("END\n", "");\
		exit(result != 0);\
}

int tests_run;

#endif

