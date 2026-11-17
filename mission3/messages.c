#include "messages.h"
#include <stdint.h>
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
if (messages==NULL || capacity==NULL){
    return 0;
}
if(required<=*capacity){
  return 1;
}
size_t new_capacity;
if(*capacity==0){
    new_capacity=3;
}else{
    new_capacity=(*capacity)*2;
}
if(new_capacity<required){
    new_capacity=required;
}
if(new_capacity>SIZE_MAX/sizeof(Message)){
    return 0;
}
Message *temp=realloc(*messages,new_capacity*sizeof(Message));
if(temp==NULL){
    return 0;
}
*messages=temp;
for(size_t i=*capacity;i<new_capacity;i++){
    (*messages)[i].id=0;
    (*messages)[i].text=NULL;
}
*capacity=new_capacity;
return 1; 
}

int store_append(MessageStore *store, int id, const char *text)
{
    if (store==NULL || id<=0 || text==NULL)
        return 0;

    if (store->count==store->capacity){
    int result1=store_reserve(&store->messages,&store->capacity,store->count+1);
    if (result1==0)
        return 0;

    }
    
    
    int result=message_init(&store->messages[store->count],id,text);
    if (result==0)
    return 0;
    store->count++;
    return 1;
    /* TODO: Stage B */
}

void message_free(Message *msg)
{
    if (msg != NULL) {
        free(msg->text);
        msg->text = NULL;
        msg->id = 0;
    }
}

void store_free(MessageStore *store)
{
    if (store != NULL) {
        for (size_t i = 0; i < store->count; i++) {
            message_free(&store->messages[i]);
        }
        free(store->messages);
        store->messages = NULL;
        store->count = 0;
        store->capacity = 0;
    }
}


int store_clone(MessageStore *dest, const MessageStore *source)
{
    if (dest==NULL || source==NULL)
        return 0;
    dest->count=source->count;
    dest->capacity=source->capacity;
    if(source->count==0) 
        return 1;
    dest->messages=calloc(source->capacity,sizeof(Message));
    if(dest->messages==NULL)
        return 0;
    for(size_t i=0;i<source->count;i++){
        int result2=message_init(&dest->messages[i],source->messages[i].id,source->messages[i].text);
        if(result2==0){
            store_free(dest);
            return 0;
        }
    }
    return 1;
}
