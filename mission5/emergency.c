#include "emergency.h"

int parse_launch_options(int argc, char *const argv[], LaunchOptions *out)
{
    int device = 0;
    int mode = 0;
    int action = 0;
    if(out == NULL){
        return 0;
    }
    out -> device[0] = '\0';
    out -> mode[0] = '\0';
    out -> action[0] = '\0';
    if (argc != 7 ){
        return 0;
    }
    for (int i = 1; i<argc; i=i+2){
        if(device > 1 || mode > 1 || action > 1){
            return 0;
        }
        int w  = i+1; 
        if (w >= argc) {
            return 0;
            }
        if(strcmp(argv[i], "--mode") == 0){
            if(mode != 0){
                return 0;
                }
            if(strcmp(argv[w],"emergency") != 0){
                return 0;
            }
            else{
                mode = mode+1;
                strcpy(out->mode, argv[w]);
            }
        }
        if(strcmp(argv[i], "--device") == 0){
            if(device != 0){
                return 0;
                }
            if(strcmp(argv[w],"BEERSHEVANATOR") != 0){
                return 0;
            }
            else{
                device = device+1;
                strcpy(out->device, argv[w]);
            }
        }
        if(strcmp(argv[i], "--action") == 0){
            if(action != 0){
                return 0;
                }
            if(strcmp(argv[w],"self-destruct") != 0){
                return 0;
            }
            else{
                action = action+1;
                strcpy(out->action, argv[w]);
            }
        }
        
    }
    if (device != 1 || mode != 1 || action != 1) {
        out->device[0] = '\0';
        out->mode[0] = '\0';
        out->action[0] = '\0';
        return 0;
    }
    return 1;
}

const AuthorizationRecord *find_authorization(const AuthorizationRecord *records, size_t count,
                                              int request_id, int unit_id, int required_clearance)
{
    if (records == NULL){
        return NULL;
    }
    for (int i = 0; i < count; i++){
       if (records[i].request_id == request_id &&
            records[i].unit_id == unit_id &&
            records[i].clearance >= required_clearance &&
            records[i].approved == 1) {
            return &records[i];
            }
    }
    return NULL;
}

int emergency_request_init(EmergencyRequest *dest,
                           const LaunchOptions *options,
                           const AuthorizationRecord *authorization,
                           const int *steps, size_t step_count)
{
    (void)dest;
    (void)options;
    (void)authorization;
    (void)steps;
    (void)step_count;
    /* TODO Stage C: create independent owned copies with rollback. */
    return 0;
}

void emergency_request_free(EmergencyRequest *request)
{
    (void)request;
    /* TODO Stage C: release owned allocations and reset every field. */
}

int validate_emergency_request(const EmergencyRequest *request)
{
    (void)request;
    /* TODO Stage D: validate the complete request without changing it. */
    return 0;
}
