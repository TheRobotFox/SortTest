#include "csv.h"

CSV CSV_from_File(char* path){
    FILE* File = 0;
    File = fopen(path,"rb");
    if(File){
        int File_size=0;
        fseek(File,0,SEEK_END);
        File_size=ftell(File);
        char* buff = malloc(File_size);
        fseek(File,0,SEEK_SET);
        fread(buff,1,File_size,File);
        return csv(buff, File_size);;
    }
    CSV csv={0,-1};
    return csv;
}

CSV csv(char *str,int size){
    return split(str,size,',');
}

CSV split(char *str,int size,char sep){
    int count = 1;
    for(int i=0; i<size; i++){
        if(str[i]==sep)
            count++;
    }
    CSV CSV= {malloc(sizeof(char*)*count+1),count};
    count=0;
    CSV.data[count]=str;
    count++;
    for(int i=0; i<size; i++){
        if(str[i]==sep){
            str[i]=0;
            CSV.data[count]=str+i+1;
            count++;
        }
    }
    str[size]=0;
    return CSV;
}

void free_CSV(CSV CSV){
    free(CSV.data);
}
