#include "emergency.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
int parse_launch_options(int argc, char *const argv[], LaunchOptions *out)
{

    if (out == NULL) {
        return 0;
    }
    
    out->device[0] = '\0';
    out->mode[0] = '\0';
    out->action[0] = '\0';

    
    if (argv == NULL) {
        return 0;
    }

    if (argc != 7) {
        return 0;
    }

    int device = 0, mode = 0, action = 0;

    for (int i = 1; i < argc; i += 2) {
        int w = i + 1; 
        if (w >= argc) {
            goto failure;
        }

        
        if (argv[i] == NULL || argv[w] == NULL) {
            goto failure;
        }

        if (strcmp(argv[i], "--mode") == 0) {
            if (mode != 0 || strcmp(argv[w], "emergency") != 0) goto failure;
            mode++;
            strcpy(out->mode, argv[w]);
        }
        else if (strcmp(argv[i], "--device") == 0) {
            if (device != 0 || strcmp(argv[w], "BEERSHEVANATOR") != 0) goto failure;
            device++;
            strcpy(out->device, argv[w]);
        }
        else if (strcmp(argv[i], "--action") == 0) {
            if (action != 0 || strcmp(argv[w], "self-destruct") != 0) goto failure;
            action++;
            strcpy(out->action, argv[w]);
        }
        else {
            goto failure;
        }
    }

    if (device != 1 || mode != 1 || action != 1) {
        goto failure;
    }

    return 1;

failure:
    
    out->device[0] = '\0';
    out->mode[0] = '\0';
    out->action[0] = '\0';
    return 0;
}
const AuthorizationRecord *find_authorization(const AuthorizationRecord *records, size_t count,
                                              int request_id, int unit_id, int required_clearance)
{
    if (records == NULL || count == 0){
        return NULL;
    }
    if (request_id < 0 || unit_id < 0 || required_clearance < 0) {
        return NULL;
    }
    for (size_t i = 0; i < count; i++){
       if (records[i].request_id == request_id &&
            records[i].unit_id == unit_id &&
            records[i].clearance >= required_clearance &&
            records[i].approved != 0) {
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
    if(dest == NULL || options == NULL || authorization == NULL){
        return 0;
    }

    if(dest->device != NULL || dest->steps != NULL ||
       dest->step_count != 0 || dest->request_id != 0 ||
       dest->unit_id != 0 ||
       dest->authorization.request_id != 0 ||
       dest->authorization.unit_id != 0 ||
       dest->authorization.approver_id != 0 ||
       dest->authorization.clearance != 0 ||
       dest->authorization.approved != 0){
        return 0;
    }

    for(size_t i = 0; i < M5_MODE_CAPACITY; i++){
        if(dest->mode[i] != '\0'){
            return 0;
        }
    }

    for(size_t i = 0; i < M5_ACTION_CAPACITY; i++){
        if(dest->action[i] != '\0'){
            return 0;
        }
    }

    if(step_count < 1 || step_count > 8 || steps == NULL){
        return 0;
    }

    if(options->device[0] == '\0'){
        return 0;
    }

    if(memchr(options->device, '\0', M5_DEVICE_CAPACITY) == NULL ||
       memchr(options->mode, '\0', M5_MODE_CAPACITY) == NULL ||
       memchr(options->action, '\0', M5_ACTION_CAPACITY) == NULL){
        return 0;
    }

    if(step_count > SIZE_MAX / sizeof(int)){
        return 0;
    }

    dest->authorization = *authorization;
    dest->request_id = authorization->request_id;
    dest->unit_id = authorization->unit_id;
    dest->step_count = step_count;

    snprintf(dest->mode, M5_MODE_CAPACITY, "%s", options->mode);
    snprintf(dest->action, M5_ACTION_CAPACITY, "%s", options->action);

    size_t de_len = strlen(options->device);
    dest->device = malloc(de_len + 1);

    if(dest->device == NULL){
        emergency_request_free(dest);
        return 0;
    }

    strcpy(dest->device, options->device);

    dest->steps = malloc(step_count * sizeof(int));

    if(dest->steps == NULL){
        emergency_request_free(dest);
        return 0;
    }

    for(size_t i = 0; i < step_count; i++){
        dest->steps[i] = steps[i];
    }

    return 1;
}

void emergency_request_free(EmergencyRequest *request)
{
    if (request == NULL) {
        return;
    }
    if (request->device != NULL) {
        free(request->device);
        request->device = NULL;
        }
    if (request->steps != NULL) {
        free(request->steps);
        request->steps = NULL;
        }
    request->step_count = 0;
    request->request_id = 0;
    request->unit_id = 0;
    
    memset(&request->authorization, 0, sizeof(request->authorization));
    memset(request->mode, 0, M5_MODE_CAPACITY);
    memset(request->action, 0, M5_ACTION_CAPACITY);
}

int validate_emergency_request(const EmergencyRequest *request)
{
    if (request == NULL) {
        return 0;
    }
    if (request->device == NULL) {
        return 0;
    }
    if (memchr(request->device, '\0', M5_DEVICE_CAPACITY) == NULL ||
    memchr(request->mode, '\0', M5_MODE_CAPACITY) == NULL ||
    memchr(request->action, '\0', M5_ACTION_CAPACITY) == NULL) {
    return 0;
}
    if (strcmp(request->device, "BEERSHEVANATOR") != 0) {
        return 0;
    }
    if (strcmp(request->mode, "emergency") != 0) {
        return 0;
    }
    if (strcmp(request->action, "self-destruct") != 0) {
        return 0;
    }
    if (request->request_id <= 0 || request->unit_id <= 0) {
        return 0;
    }
    if (request->authorization.approver_id <= 0) {
        return 0;
    }
    if (request->authorization.request_id != request->request_id ||
        request->authorization.unit_id != request->unit_id) {
        return 0;
    }
    if (request->authorization.approved != 1) {
        return 0;
    }
    if (request->authorization.clearance < M5_ADMIN_CLEARANCE || request->authorization.clearance > 5) {
        return 0;
    }
    if (request->step_count != 3 || request->steps == NULL) {
        return 0;
    }
    if (request->steps[0] != STEP_PREPARE_EMERGENCY ||
        request->steps[1] != STEP_REQUEST_AUTHORIZATION ||
        request->steps[2] != STEP_EXECUTE_COMMAND) {
        return 0;
    }

    return 1;
}
