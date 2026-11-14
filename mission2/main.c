#include "investigation.h"

int main(void)
{
    Agent working[16];
    size_t capacity = 0; /* TODO Stage B: use sizeof on working. */
    capacity=sizeof(working)/sizeof(Agent);

    return run_investigation(working, capacity);
}
