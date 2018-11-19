#include <sys/time.h>
#include "psm.h"

unsigned long long currentTime();
void construction(PSM * output, int rank, int numOfItems);
void painter(PSM * input, TriplePSM * output, int numOfItems);
void checker(PSM * input, PSM * output, int numOfItems);
void assembler(PSM * input, int numOfItems);

