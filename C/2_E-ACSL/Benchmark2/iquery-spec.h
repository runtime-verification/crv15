/*@ requires \forall integer i; 0 <= i < __n ==> \valid((char*)__dest+i); */
extern void *memcpy (void *__restrict __dest,
       __const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
