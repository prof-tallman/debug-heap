
#include <stdlib.h>
#include <stdio.h>
#include "debug_heap.h"


static int g_malloc_count = 0;
static DBG_HEAP_ENTRY g_malloc_list[DBG_HEAP_COUNT] = {0};


void* _debug_malloc(int size, char* file, int line)
{
    // Sanity check that we can keep track of this debug allocation
    // If we're out of room, we'll still malloc but we won't be able to track it
    // Set the count to be DBG_HEAP_SIZE
    if (g_malloc_count >= DBG_HEAP_COUNT)
    {
        fprintf(stderr,  "Error: Debug heap reached tracking limit (%d)\n", DBG_HEAP_COUNT);
        return malloc(size);
    }

    // Perform the allocation and track it for debugging purposes
    void* addr = malloc(size);
    g_malloc_list[g_malloc_count].addr = addr;
    g_malloc_list[g_malloc_count].size = size;
    g_malloc_list[g_malloc_count].file = file;
    g_malloc_list[g_malloc_count].line = line;
    g_malloc_count++;

    // Return the actual malloced address
    return addr;
}


void debug_free(void* address)
{
    // Search the debug allocation list for this address and remove it if found
    for(int idx = 0; idx < g_malloc_count; idx++)
    {
        if (g_malloc_list[idx].addr == address)
        {
            // Perform the actual free
            g_malloc_count--;
            free(address);

            // Delete this entry by overwriting it with the last value in the list
            g_malloc_list[idx].addr = g_malloc_list[g_malloc_count].addr;
            g_malloc_list[idx].size = g_malloc_list[g_malloc_count].size;
            g_malloc_list[idx].file = g_malloc_list[g_malloc_count].file;
            g_malloc_list[idx].line = g_malloc_list[g_malloc_count].line;
            
            // Zero out the last value in the list and decrement the count
            g_malloc_list[g_malloc_count].addr = NULL;
            g_malloc_list[g_malloc_count].size = 0;
            g_malloc_list[g_malloc_count].file = NULL;
            g_malloc_list[g_malloc_count].line = 0;

            return;
        }
    }

    fprintf(stderr, "Error: Address %p not in the debug heap allocation list\n", address);
    return;
}

int debug_verify_heap()
{
    // If the counter 
    if (g_malloc_count == 0)
    {
        fprintf(stderr, "No memory leaks: all allocations were properly freed.\n");
        return EXIT_SUCCESS;
    }
    else
    {
        fprintf(stderr, "Error: %d memory leaks found.\n", g_malloc_count);
        for(int i = 0; i < g_malloc_count; i++)
        {
            fprintf(stderr, " * Memory leak %s:%d (size %d).\n", 
                g_malloc_list[i].file, g_malloc_list[i].line, g_malloc_list[i].size);
        }
        return EXIT_FAILURE;
    }
}
