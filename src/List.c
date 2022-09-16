#include "List.h"

LIST CreateList(int size){
    LIST List;
    List.data=malloc(size*sizeof(int));
    List.size=size;
    List.filled=0;
    return List;
}

void free_List(LIST* List){
    free(List->data);
    List->size=0;
}

void printList(int line_length, LIST *List){
    for(int i=0; i<List->size; i++){
        if(i && i%line_length==0)
            printf("\n");
        printf("%d ",List->data[i]);
    }
    printf("\n");
}

int fill_random(int n, LIST *List){
    int start=List->filled;
    int res=fill_to_n(n,List);
    srand((int)time(0));
    unsigned int tmp,rn1;
    //for(int j=0; j<100; j++)
        for(int i=0; i<n; i++){
            rn1=start + (int)((float)rand()/RAND_MAX*(n-start-1));
            tmp=List->data[i];
            List->data[i]=List->data[rn1];
            List->data[rn1]=tmp;
        }
    return res;
}

int random_fill(int n, LIST *List){
    srand((int)time(0));
    if(n<=List->size-List->filled){
        for(int i=0; i<n; i++)
            List->data[i+List->filled]=(int)((float)rand()/RAND_MAX*(n*10));
        List->filled+=n;
        return 0;
    }
    return -1;
}

int fill_reversed(int n, LIST *List){
    if(n<=List->size-List->filled){
        for(int i=0; i<n;i++){
            List->data[i+List->filled]=n-i-1;
        }
        List->filled+=n;
        return 0;
    }
    return -1;
}

int fill_to_n(int n, LIST *List){
    if(n<=List->size-List->filled){
        for(int i=0; i<n;i++){
            List->data[i+List->filled]=i;
        }
        List->filled+=n;
        return 0;
    }
    return -1;
}

LIST List_from_csv(CSV CSV){
    LIST List = CreateList(CSV.size);
    int num,res;
    for(int i=0; i<List.size; i++){
        res=to_num(CSV.data[i],&num);
        if(res){
            List.size=-1;
            return List;
        }
        List.data[i]=num;
    }
    return List;
}

int get_line_length(int n, double width_mult)
{
    int line_length;
    line_length = sqrt(n)*width_mult;
    return line_length;
}

char tmp_file_buf[128];
int List_to_File(FILE* File, LIST* List){
    int num_length = (int)(log(get_max(List))/log(10))+1;
    int line_length=get_line_length(List->size,1.6/sqrt(num_length+2));
    snprintf(tmp_file_buf,128,"%%%dd, ",num_length);
    for(int i=0; i<List->size;i++){
        if(!(i%line_length)){
            fputc('\n',File);
        }
        fprintf(File,tmp_file_buf,List->data[i]);
    }
    fseek(File,-1,SEEK_CUR);
    fputc('\n',File);
    return 0;
}

int Verify(LIST* List){
    for(int i=0; i<List->size-1; i++){
        if(List->data[i]>List->data[i+1])
            return 1;
    }
    return 0;
}

unsigned int get_max(LIST* List){
    unsigned int max=0;
    for(int i=0; i<List->size; i++){
        if(List->data[i]>max)
            max=List->data[i];
    }
    return max;
}

int to_num(char* str, int* num){
    return (sscanf(str,"%d",num)!=1);
}


void List_append(LIST *List, unsigned int e)
{
    List->data[List->filled++]=e;
}

void List_concat(LIST *a, LIST *b)
{
    unsigned int *tmp = a->data;
    a->data = malloc((a->filled+b->filled)*sizeof(unsigned int));
    for(size_t i=0; i<a->filled; i++)
        a->data[i]=tmp[i];
    free(tmp);
    for(size_t i=0; i<b->filled; i++)
        a->data[a->filled+i]=b->data[i];
    a->filled+=b->filled;
}
