#include "macros.h"

#ifdef __cplusplus
extern "C" {
#endif

//Define storage for the IRQ lock counter
//This must be done in a 'c/cpp' file to prevent "duplicate definitions" errors
//when linking the program (as its a global variable)
volatile uint8 g_irqLockCnt = 0;

#ifdef __cplusplus
}  /* extern "C" */
#endif
