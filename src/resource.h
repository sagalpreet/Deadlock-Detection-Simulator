struct resource
{
    int r_count;
    int r_free;
    char* r_name;
};

struct resource_pool
{
    int size_max;
    int size_cur;
    struct resource* resources; // array of resources
};

struct resource* resource(int r_count, char* r_name);

struct resource_pool* resource_pool(int size_max);

void append(struct resource_pool* pool, int r_count, char* r_name);