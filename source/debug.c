#include "debug.h"

#include <stdio.h>
#include <stdarg.h>

char* gba_printf(const char* fmt, ...)
{
    static char str[DEBUG_PRINT_BUFSIZE];
    va_list arglist;
										
    // Initialization
    str[0] = '\0';

    // Variable argument conversion 
    va_start(arglist, fmt);          // variable number of arguments of initialization 
    vsprintf(str, fmt, arglist);     // variable number of arguments conversion 
    va_end(arglist);                 // variable number of arguments reset

    // Display 
    asm volatile("mov r0, %0;"       // Visual Boy Advance (Mappy) Debug Output
                 "swi 0xff;"
                : // no ouput
                : "r" (str)
                : "r0");
    return (str);
}
