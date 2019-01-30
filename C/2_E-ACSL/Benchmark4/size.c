#include <stdlib.h>
#include <stdio.h>

int f(int x) {
  int *p = (int *)malloc(sizeof(int));
  int tmp;
  if (x) {
    *p = 1 + f(x - 1);
    tmp = *p;
    free(p);
    return tmp;
  }
  free(p);
  return 1;
}

int g(int x) {
  int *p = (int *)malloc(sizeof(int));
  int tmp;
  if (x) {
    *p = 1 + g(x - 1);
    tmp = *p;
    free(p);
    return tmp;
  }
  free(p);
  return 1;
}

int main(void) {
  int res;
  char *p;
  printf("Calling function f...\n");
  res = f(100);
  printf("The result is %d.\n", res);
  printf("Calling function g...\n");
  res = g(10000);
  printf("The result is %d.\n", res);
  p = (char*)malloc(sizeof(char));
  printf("Exiting.\n");
  return 0;
}
