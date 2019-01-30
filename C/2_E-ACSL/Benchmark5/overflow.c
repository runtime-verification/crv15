#include <limits.h>
#include <stdio.h>

int main(void) {
  int i = 0;
  int x = INT_MIN;

  for (i = 0; i <= INT_MAX; i++) {
    if (i == INT_MAX || i < 0) printf("i = %d (x = %d)\n",i,x);
    if (i % 2 == 0) x = -i;
    else x += i;
    if (i < 0) break;
  }

  return 0;
}
