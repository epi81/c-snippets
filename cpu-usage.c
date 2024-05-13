#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>

int main(int argc,char* argv[]){
    char str[128];
    const char d[2] = " ";
    char* token;
    int i = 0;
    unsigned long long sum = 0, idle, lastSum = 0,lastIdle = 0;
    int usage;
    
    while(1){
        FILE* fp = fopen("/proc/stat","r");
        i = 0;
        fgets(str,128,fp);
        fclose(fp);
        token = strtok(str,d);
        sum = 0;
        while(token!=NULL){
                token = strtok(NULL,d);
                if(token!=NULL){
                    sum += strtoull(token,0,10);
        
                    if(i==3)
                        idle = strtoull(token,0,10);
                    i++;
                }
        }
        usage = (int)(100.0 - (idle-lastIdle)*100.0/(sum-lastSum));
        printf("usage:%2d%%\n", usage);                                                                                                                                                                                      

        lastIdle = idle;
        lastSum = sum;


        sleep(1);
    }
    return 0;
}
