
#include <stdio.h>
int main() {
  FILE *f = fopen("/proc/stat", "r");
  int test = 0;
  fscanf(f, "cpu %d", &test);
  printf("%d", test);
  return 0;
}
