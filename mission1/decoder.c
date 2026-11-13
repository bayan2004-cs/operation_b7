#include "decoder.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include<stdlib.h>

int decode_message(const char *encoded, int shift, char *decoded, size_t capacity)
{
    /* TODO: validate, reverse the full string, and undo the shift. */
    if(decoded==NULL || encoded==NULL || shift>25 || shift<0 || strlen(encoded)+1> capacity ){
        return 0;
    }
    int len =strlen(encoded);
    for(int i=0 ; i<len;i++){
        if((encoded[len-1-i]>90 && encoded[len-1-i]!=95) || (encoded[len-1-i]<65 && encoded[len-1-i]!=95) ){
            return 0;

        }
        if (encoded[len-1-i]!='_')
        {
            decoded[i]=((encoded[len-1-i]-'A'-shift+26)%26)+'A';
        
        }else{
            decoded[i]=encoded[len-1-i];
        }

    }
    decoded[len]='\0';
    
    return 1;
}

int calculate_checksum(const char *message)
{
    /* TODO: sum decoded characters modulo 97. */
    int result=0;
    if (message!=NULL)
    {
        for (size_t i = 0; i < strlen(message); i++)
        {
            result+=message[i];
            
        }
        result=result%97;
        return result;
        
    }
    
    return -1;
}

int process_transmission(const char *line)
{
    /* TODO: parse, validate, decode, verify, and print one record. */
    int count=0;
    for (size_t i = 0; i < strlen(line); i++)
    {
       if(line[i]=='|'){
        count+=1;
       }
       if(line[i]==' '){
        printf("[INVALID]\n");
        return 0;
       }
    }
    if (count!=3)
    {
     printf("[INVALID]\n");
     return 0;
    }
    
    
    char myCopy[strlen(line)+1];
    strcpy(myCopy,line);

    char *sender=strtok(myCopy,"|");

    char *Key=strtok(NULL,"|");
    char *CheckSum=strtok(NULL,"|");
    char *message=strtok(NULL,"|");
    if (sender == NULL || Key == NULL || CheckSum== NULL || message == NULL || strlen(sender)>24 || strlen(message)>160 || strlen(CheckSum)!=2) {
        printf("[INVALID]\n");
    return 0; 
    }
    for (size_t i = 0; i <  strlen(sender); i++)
    {
        if(!(isupper(sender[i])|| sender[i]=='_')){
            printf("[INVALID]\n");
            return 0;
        } 

    }
    for (size_t i = 0; i < strlen(message); i++)
    {
       if(!(isupper(message[i])|| message[i]=='_')){
        printf("[INVALID]\n");
            return 0;
        } 
    }
    for (size_t i = 0; i < strlen(Key); i++)
    {
        if(!(isdigit(Key[i]))){
            printf("[INVALID]\n");
            return 0;
        } 
    }
    for (size_t i = 0; i < strlen(CheckSum); i++)
    {
        if(!(isdigit(CheckSum[i]))){
            printf("[INVALID]\n");
            return 0;
        } 
    }
    int key=atoi(Key);
    int checksum=atoi(CheckSum);
    if (key>25 || checksum>96)
    {
        printf("[INVALID]\n");
        return 0;
    }
    char result[strlen(message)+1];
    int de=decode_message(message,key,result,strlen(message)+1);
    if (de==0)
    {
        printf("[INVALID]\n");
        return 0;
    }
    if (calculate_checksum(result)==checksum)
    {
        printf("[%s] %s\n", sender, result);
        return 1;
    }
    printf("[INVALID]\n");
    return 0;
}

int decode_advanced(const char *encoded, const int *keys, size_t key_count, char *decoded, size_t capacity)
{
    /* Optional TODO. */
    if(decoded==NULL || encoded==NULL|| keys==NULL || key_count==0  || strlen(encoded)+1> capacity ){
        return 0;
    }
    for (size_t i = 0; i < key_count; i++)
    {
        if (keys[i]>25 || keys[i]<0)
        {
            return 0;
        }
        
    }
    
    int len =strlen(encoded);
    for(int i=0 ; i<len;i++){
        if((encoded[len-1-i]>90 && encoded[len-1-i]!=95) || (encoded[len-1-i]<65 && encoded[len-1-i]!=95) ){
            return 0;

        }
        if (encoded[len-1-i]!='_')
        {
            decoded[i]=((encoded[len-1-i]-'A'-keys[i%key_count]+26)%26)+'A';
        
        }else{
            decoded[i]=encoded[len-1-i];
        }

    }
    decoded[len]='\0';
    return 1;
}

