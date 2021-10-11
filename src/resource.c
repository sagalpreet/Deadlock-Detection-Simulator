#include <stdlib.h>
#include <stdio.h>

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