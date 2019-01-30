#include "e_acsl_mmodel.h"

/*@ ensures __memory_size - \old(__memory_size) == 0; */
int f(int);

/*@ ensures __memory_size - \old(__memory_size) == 0; */
int g(int);

/*@ ensures __memory_size - \old(__memory_size) == 0; */
int main(void);
