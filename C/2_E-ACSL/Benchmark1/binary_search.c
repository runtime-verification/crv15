#include <stdio.h>
#define LEN 10000000
#define SPECIAL_IDX (LEN/2-1)

#include "binary_search.h"

int binary_search(int* a, int length, int key) {
  int low = 0, high = length - 1;
  while (low<=high) {
    int mid = low + (high - low) / 2;
    if (a[mid] == key) return mid;
    if (a[mid] < key) { low = mid + 1; }
    else { high = mid - 1; }
  }
  return -1;
}

int t[LEN];

int main(void) {
  int res, i;

  for(i = 0; i < LEN; i++)
    t[i] = 2 * i + 1;

  res = binary_search(t, LEN, 10101);
  printf("searching index of 10101 (expecting 5050): %d\n\n", res);

  res = binary_search(t, LEN, 10100);
  printf("searching index of 10101 (expecting -1): %d\n\n", res);

  /* // the given length is too large */
  /* res = binary_search(t, LEN+1, 101); // error detected: invalid array length */
  /*                                     // (cannot conclude that the array is  */
  /*                                     // sorted for the given length anymore) */
  /* printf("searching index of LEN (error previously reported): %d\n\n", res); */

  // the array is no more sorted
  t[SPECIAL_IDX] = t[0];
  res = binary_search(t, LEN, 101); // error detected: unsorted array
  printf("searching index of 101 (error previously reported): %d\n\n", res);

  return 0;
}
