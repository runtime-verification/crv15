#include <stdlib.h>

/* Prototypes for __malloc_hook, __free_hook */
#include <malloc.h>

#include "__RVC_Instrument.h"

/* Number of event fired (global, for sequencing purposes)  */
static int num_event = 0;

/* Prototypes for our hooks.  */
static void my_init_hook (void);
static void *my_malloc_hook (size_t, const void *);
static void my_free_hook (void*, const void *);

/* Override initializing hook from the C library. */
void (*__malloc_initialize_hook) (void) = my_init_hook;

static void *(*old_malloc_hook) (size_t, const void *);
static void (*old_free_hook) (void*, const void *);

static void my_init_hook (void)
{
  // Initialize malloc hook
  if (__malloc_hook == my_malloc_hook)
    return;
  old_malloc_hook = __malloc_hook;
  old_free_hook = __free_hook;
  __malloc_hook = my_malloc_hook;
  __free_hook = my_free_hook;
}

static void *
my_malloc_hook (size_t size, const void *caller)
{
  void *result;
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  __free_hook = old_free_hook;
  /* Call recursively */
  result = malloc (size);
  /* Call RV instrumentation */
  __RVC_malloc(result);
  /* Save underlying hooks */
  old_malloc_hook = __malloc_hook;
  old_free_hook = __free_hook;
  /* Restore our own hooks */
  __malloc_hook = my_malloc_hook;
  __free_hook = my_free_hook;
  return result;
}

static void
my_free_hook (void *ptr, const void *caller)
{
  /* Restore all old hooks */
  __malloc_hook = old_malloc_hook;
  __free_hook = old_free_hook;
  /* Call recursively */
  if (ptr == NULL)
    return;

  /* Call RV instrumentation */
  __RVC_free(ptr);
  free (ptr);
  /* Save underlying hooks */
  old_malloc_hook = __malloc_hook;
  old_free_hook = __free_hook;
  /* Restore our own hooks */
  __malloc_hook = my_malloc_hook;
  __free_hook = my_free_hook;
}

static void * alloc_free(void * violate_if_not_null)
{
  my_init_hook();

  int * p = malloc(num_event % 200);
  num_event++;
  if (p == NULL)
    return;

  *p = 1;

  // Don't monitor malloc inside event collection code
  // (use old malloc hook)
  __malloc_hook = old_malloc_hook;
  __free_hook = old_free_hook;
  /* Call RV instrumentation */
  __RVC_access(p);
  __malloc_hook = my_malloc_hook;
  __free_hook = my_free_hook;

  if (num_event % 500 != 0) {
    free(p);
  }

  if (num_event % 5000000 == 4999999) {
    // Double free every 5M events
    free(p);
  }

  if (num_event % 1000 == 80) {
    // 80 bytes allocated. (p + 4) is a valid location
    *(p+4) = 1;

    // Don't monitor malloc inside event collection code
    // (use old malloc hook)
    __malloc_hook = old_malloc_hook;
    __free_hook = old_free_hook;
    /* Call RV instrumentation */
    __RVC_access(p+4);
    __malloc_hook = my_malloc_hook;
    __free_hook = my_free_hook;
  }
}

int main( int argc, const char* argv[] )
{
  int i;
  for (i = 0; i < 10000000; i++) {
    // Fire 10M events
    alloc_free(NULL);
  }
}
