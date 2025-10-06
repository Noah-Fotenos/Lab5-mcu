// Necessary includes for printf to work
#include <stdio.h>
#include "stm32l432xx.h"

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}

int main(void) {
  int i;

  for (i = 0; i < 100; i++) {
      // printf
    printf("Hello World %d!\n", i);
  }
  do {
    i++;
  } while (1);
}