#include "decoder.h"
#include<string.h>
#include <stdio.h>

int main(void)
{
    /*
     * TODO: read lines with fgets until EOF, remove \n and optional \r,
     * call process_transmission, count both outcomes, and print the summary.
     */
    int countV =0 ;
    int countIN=0;
    char myBuffer[250];
    while (fgets(myBuffer,sizeof(myBuffer),stdin))
    {
        myBuffer[strcspn(myBuffer,"\r\n")]='\0';
        int re=process_transmission(myBuffer);
        if (re==0)
        {
            countIN+=1;
        }else{
            countV+=1;
        }
        
    }
    
    printf("Recovered: %d | Rejected: %d\n" , countV,countIN);
    return 0;
}

