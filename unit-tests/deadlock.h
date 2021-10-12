void* detect_deadlock(void* arg);
int first_thread_heuristic(int num_resources, int* done, int* available);
int last_thread_heuristic(int num_resources, int* done, int* available);
int heaviest_thread_heuristic(int num_resources, int* done, int* available);