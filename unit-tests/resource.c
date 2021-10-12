#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "resource.h"

struct resource* resource(int r_count, char* r_name)
{
    struct resource* p_resource = (struct resource *) malloc(sizeof(struct resource));
    p_resource->r_count = p_resource->r_free = r_count;
    p_resource->r_name = r_name;

    return p_resource;
}

struct resource_pool* resource_pool(int size_max)
{
    struct resource_pool* p_pool = (struct resource_pool*) malloc (sizeof (struct resource_pool));
    p_pool ->resources = (struct resource *) malloc (sizeof (struct resource) * size_max);
    p_pool -> size_max = size_max;
    p_pool -> size_cur = 0;

    return p_pool;
}

void append(struct resource_pool* pool, int r_count, const char* r_name)
{
    if (pool -> size_cur == pool -> size_max)
    {
        printf("Error adding another resource to list. Number of different resources exceeded the specified value\n");
        return;
    }

    (pool -> resources)[pool -> size_cur].r_count = r_count;
    (pool -> resources)[pool -> size_cur].r_free = r_count;
    
    (pool -> resources)[pool -> size_cur].r_name = (char *) r_name;

    (pool -> size_cur) += 1;
}

int main()
{
    printf("Running Unit Tests for resource.c functions...\n\n");

    struct resource_pool *pool = resource_pool(3);
    
    if (pool->size_max == 3) printf("TEST CASE 1 PASSED\n");
    else printf("TEST CASE 1 FAILED\n");
    
    if (pool->size_cur == 0) printf("TEST CASE 2 PASSED\n");
    else printf("TEST CASE 2 FAILED\n");

    append(pool, 3, "R1");
    if (pool->resources[0].r_count == 3) printf("TEST CASE 3 PASSED\n");
    else printf("TEST CASE 3 FAILED\n");

    if (pool->resources[0].r_free == 3) printf("TEST CASE 4 PASSED\n");
    else printf("TEST CASE 4 FAILED\n");

    if (!strcmp(pool->resources[0].r_name, "R1")) printf("TEST CASE 5 PASSED\n");
    else printf("TEST CASE 5 FAILED\n");
    
    append(pool, 3, "R2");
    if (pool->resources[1].r_count == 3) printf("TEST CASE 6 PASSED\n");
    else printf("TEST CASE 6 FAILED\n");

    if (pool->resources[1].r_free == 3) printf("TEST CASE 7 PASSED\n");
    else printf("TEST CASE 7 FAILED\n");

    if (!strcmp(pool->resources[1].r_name, "R2")) printf("TEST CASE 8 PASSED\n");
    else printf("TEST CASE 8 FAILED\n");
    
    append(pool, 3, "R3");
    if (pool->resources[2].r_count == 3) printf("TEST CASE 9 PASSED\n");
    else printf("TEST CASE 9 FAILED\n");

    if (pool->resources[2].r_free == 3) printf("TEST CASE 10 PASSED\n");
    else printf("TEST CASE 10 FAILED\n");

    if (!strcmp(pool->resources[2].r_name, "R3")) printf("TEST CASE 11 PASSED\n");
    else printf("TEST CASE 11 FAILED\n");

    return 0;
}
