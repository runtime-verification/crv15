#include <stdio.h>
#include <stdio.h>

/* Example monitor inclusion #include "__RVC_Malloc_Free_Monitor.h" */

void
__RVC_free(void* key)
{
    // Add instrumentation for free here
    printf("free@%p\n", key);

    // Example: __RVC_Malloc_Free_free(key);
}

void
__RVC_access(void* key)
{
    // Add instrumentation for safe access here
    printf("safe_access@%p\n", key);

    // Example: __RVC_Malloc_Free_safe_access(key);
}

void
__RVC_malloc(void* key)
{
    // Add instrumentation for safe access here
    printf("malloc@%p\n", key);

    // Example: __RVC_Malloc_Free_malloc(key);
}


