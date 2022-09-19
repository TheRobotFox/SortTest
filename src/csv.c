#include "csv.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

CSV CSV_load_file(char* path){
    FILE* File = 0;
    File = fopen(path,"rb");
    if(File){
        int file_size=0, read_bytes;
        fseek(File,0,SEEK_END);
        file_size=ftell(File);
        char* buff = malloc(file_size);
        fseek(File,0,SEEK_SET);
        read_bytes = fread(buff,1,file_size,File);
        if(read_bytes==file_size)
            return CSV_load_string(buff, file_size);;
    }
    CSV csv={0,-1};
    return csv;
}

CSV CSV_load_string(char *str,int size){
    return CSV_load_string_del(str,size,',');
}

CSV CSV_load_string_del(char *str,int size,char sep)
{
    int count = 1;
    for(int i=0; i<size; i++){
        if(str[i]==sep)
            count++;
    }
    CSV csv= {malloc(sizeof(char*)*count+1),count};
    count=0;
    csv.data[count]=str;
    count++;
    for(int i=0; i<size; i++){
        if(str[i]==sep){
            str[i]=0;
            csv.data[count]=str+i+1;
            count++;
        }
    }
    str[size]=0;
    return csv;
}

void CSV_print(CSV CSV){
    for(int i=0; i< CSV.size; i++)
        printf("%s ", CSV.data[i]);
}

void CSV_free(CSV CSV){
    free(CSV.data);
}
