#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*PREDICATE)(const void*);

// ---------------------------------------------------

int filter(void* output, const void* input, int length, int item_size, PREDICATE pred) {
  int count = 0;
  const float* inp = input;
  float* outp = output;
  for(int i = 0; i < length; i++) {
    if(pred(inp)) {
      memcpy(outp, inp, item_size);
      count++;
      outp++;
    }
    inp++;
  }
  return count;
}

int less_than_50(const void* p) {
  float num = *(const float*)p;
  return num < 50.0;
}

// -------------------------------------------------------
// you shouldn't have to change the stuff below here.
// you can for testing, but please put it back the way it was before you submit.

#define NUM_VALUES 10

float float_values[NUM_VALUES] = {
  31.94, 61.50, 36.10, 1.00, 6.35,
  20.76, 69.30, 19.60, 79.74, 51.29,
};

int main() {
  float filtered[NUM_VALUES];
  int filtered_len = filter(filtered, float_values, NUM_VALUES, sizeof(float), &less_than_50);

  printf("there are %d numbers less than 50:\n", filtered_len);

  for(int i = 0; i < filtered_len; i++)
    printf("\t%.2f\n", filtered[i]);

  return 0;
}