/* ************************************************************************** */
/* instructions */
/* ************************************************************************** */
/*
  The goal is to check that each call to [binary_search] satisfies its
  precondition. More precisely, its input [a] must be a fully-allocated and
  sorted array of (at least) [length] elements.
*/
/* ************************************************************************** */

/*@ requires \forall integer i; 0 <= i < length - 1 ==> a[i] <= a[i+1]; */
int binary_search(int* a, int length, int key);
