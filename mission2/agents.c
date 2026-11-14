#include "agents.h"
#include <string.h>

Agent *find_agent(Agent *agents, size_t count, int id)
{
    if (agents==NULL || count==0)
    {
        return NULL;
    }
    
    Agent* returnPtr;
    for (size_t i = 0; i < count; i++)
    {
        returnPtr=&agents[i];
        if (returnPtr->id==id)
        {
            return returnPtr;
        }
        
    }
    
    return NULL; /* TODO: Stage A */
}

int copy_agents(Agent *dest, size_t capacity, const Agent *source, size_t count)
{
    if (capacity<count)
    {
        return 0;
    }
    if (count==0)
    {
        return 1;
    }
    if (dest == NULL || source == NULL) {
    return 0;
    }
    
    for (size_t i = 0; i < count; i++)
    {
        Agent* des=&dest[i];
        const Agent* sou=&source[i];
        *des=*sou;

    }
    
    return 1; /* TODO: Stage B */
}

int update_agent(Agent *agent, AgentPatch patch)
{
    if (agent== NULL ||agent->id != patch.agent_id || (patch.new_active!=0 && patch.new_active!=1) || patch.new_clearance>5 ||patch.new_clearance<0 )
    {
        return 0;
    }
    Agent AG=*agent;
    AG.active=patch.new_active;
    AG.clearance=patch.new_clearance;
    *agent=AG;
    return 1; /* TODO: Stage B */
}

int apply_patch(Agent *agents, size_t count, const AgentPatch *patch)
{
    if(agents==NULL || count==0 || patch==NULL){
        return 0;
    }
    Agent* AGp=find_agent(agents,count,patch->agent_id);
    if (AGp==NULL)
    {
        return 0;
    }   
        
    return update_agent(AGp,*patch); /* TODO: Stage B */
}

int classify_access(const Agent *agents, size_t count, const AccessEvent *event)
{
    if (event == NULL || (agents == NULL && count != 0)) {
    return ACCESS_INVALID;
    }
        if (event->agent_id<=0 || (event->resource_id!=AIR_SUPPORT && event->resource_id!=CONTROL_SYSTEM && event->resource_id!=BGU_CAMERAS && event->resource_id!=PORT_8080 && event->resource_id!= EVIL_MORTY_PRIVATE_COMMS && event->resource_id!=SERVER_BACKUP))
        {
           return ACCESS_INVALID;
        }
        if (event->required_clearance>5 || event->required_clearance<0 || (event->granted!=0 &&  event->granted!=1) || event->sequence<-1 || event->sequence>7 || event->encoded_symbol<-1 || event->encoded_symbol>25)
        {
            return ACCESS_INVALID;
        }
        
        const Agent *AGp = NULL;
        for (size_t i = 0; i < count; i++) {
           if (agents[i].id == event->agent_id) {
              AGp = &agents[i];
              break;
            }
        }
        int is_authorized = 0;
        if (AGp != NULL && AGp->active == 1 && AGp->clearance >= event->required_clearance) {
          is_authorized = 1;
        }
        if ( event->granted==1 && is_authorized==1)
        {
            return ACCESS_OK;
        }
        if (is_authorized==0 && event->granted==1)
        {
            return ACCESS_UNAUTHORIZED_GRANTED;
        }
        if (is_authorized==1 && event->granted==0)
        {
            return ACCESS_UNEXPECTED_DENIAL;
        }
        if (is_authorized==0 && event->granted==0)
        {
            return ACCESS_DENIED;
        }
    
    return ACCESS_INVALID; /* TODO: Stage C */
}

int recover_code(const Agent *agents, size_t agent_count, const AccessEvent *events, size_t event_count, char *code, size_t capacity)
{
    
    if (code != NULL && capacity > 0)
        code[0] = '\0';
    if (capacity<9 || agents==NULL || events==NULL|| code==NULL)
        return 0;
    
    int Help[9]={0,0,0,0,0,0,0,0,0};       
    for (size_t i = 0; i < event_count; i++)
    {
        const AccessEvent* event=&events[i];
        if (events[i].agent_id!=67 || classify_access(agents,agent_count,event)!=ACCESS_UNAUTHORIZED_GRANTED || events[i].sequence<0 || events[i].sequence>7 || events[i].encoded_symbol<0 || events[i].encoded_symbol>25)
        {
            continue;
        }
        if (Help[events[i].sequence]==1)
        {
            code[0] = '\0';
            return 0;
        }
        code[events[i].sequence]='A'+((event->encoded_symbol-event->sequence+26)%26);
        Help[events[i].sequence]=1;
    }
    for (size_t i = 0; i < 8; i++)
    {
        if (Help[i]==0)
        {
            code[0] = '\0';
            return 0;
        }
        
    }
    code[8]='\0';
    

        
    return 1; /* TODO: Stage D */
}
