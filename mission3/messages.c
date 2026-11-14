#include "messages.h"
#include <stdlib.h>
#include <string.h>

int store_init(MessageStore *store, size_t initial_capacity)
{
    if (store == NULL) return 0;
    if (initial_capacity==0)
    {
        store->capacity=0;
        store->count=0;
        store->messages=NULL;
        return 1;
    }
    if (initial_capacity > ((size_t)-1) / sizeof(Message)) return 0;
    store->messages=calloc(initial_capacity,sizeof(Message));
    if (store->messages==NULL)
    {
        store->capacity=0;
        store->count=0;
        return 0;
    }
    store->capacity=initial_capacity;
    store->count=0;
    return 1; /* TODO: Stage A */
}

int message_init(Message *msg, int id, const char *text)
{
    if (msg==NULL)
        return 0;
    if (id<=0 || text==NULL)
    {
        msg->id=0;
        msg->text=NULL;
        return 0;
    }
    

        
        
    size_t len=strlen(text);
    msg->text=malloc(len+1);
    if (msg->text==NULL)
    {
        msg->id=0;
        return 0;
    }
    strcpy(msg->text,text);
    msg->id=id;

    return 1; /* TODO: Stage A */
}

int store_reserve(Message **messages, size_t *capacity, size_t required)
{
    (void)messages;
    (void)capacity;
    (void)required;
    return 0; /* TODO: Stage B */
}

int store_append(MessageStore *store, int id, const char *text)
{
    (void)store;
    (void)id;
    (void)text;
    return 0; /* TODO: Stage B */
}

void message_free(Message *msg)
{
    (void)msg; /* TODO: Stage C */
}

void store_free(MessageStore *store)
{
    (void)store; /* TODO: Stage C */
}

int store_clone(MessageStore *dest, const MessageStore *source)
{
    (void)dest;
    (void)source;
    return 0; /* TODO: Stage D */
}
