#include "emergency.h"
#include <string.h>
#include <stdio.h>
int parse_launch_options(int argc, char *const argv[], LaunchOptions *out)
{
    // 1. קודם מוודאים ש-out קיים כדי שנוכל לכתוב אליו
    if (out == NULL) {
        return 0;
    }
    
    // 2. מיד מאפסים את המבנה! 
    // כך מובטח שבכל שגיאה (כולל argv==NULL) הוא יישאר ריק כנדרש.
    out->device[0] = '\0';
    out->mode[0] = '\0';
    out->action[0] = '\0';

    // 3. עכשיו אפשר לבדוק בבטחה את שאר הקלטים
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

        // וידוא שהמחרוזות עצמן אינן NULL
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

    return 1; // הצלחה

failure:
    // איפוס חוזר למקרה שנכתב משהו וגילינו שגיאה באמצע הלולאה
    out->device[0] = '\0';
    out->mode[0] = '\0';
    out->action[0] = '\0';
    return 0;
}
const AuthorizationRecord *find_authorization(const AuthorizationRecord *records, size_t count,
                                              int request_id, int unit_id, int required_clearance)
{
    if (records == NULL){
        return NULL;
    }
    for (size_t i = 0; i < count; i++){
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
