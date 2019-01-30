/*@ requires \forall integer i; (0 <= i < \block_length(__src)
  @   ==> __src[i] == '\0'
  @   ==> \valid(__dest+i)); */
extern char *strcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
