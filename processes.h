#include <sys/time.h>
#include "psm.h"

unsigned long long currentTime();
void construction(TriplePSM * output, int rank, int numOfItems);
void painter(TriplePSM * input, TriplePSM * output, int numOfItems);
void checker(PSM * input, TriplePSM * output, int numOfItems);
void assembler(TriplePSM * input, int numOfItems);

