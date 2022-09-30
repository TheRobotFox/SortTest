#include "SortList.h"
#include "GUI.h"

void List_fill_random(List l, int n)
{
    S_TYPE *data,
           tmp;
    size_t rand_index;

    data = List_end(l);

    List_fill_to_n(l,n);

    srand((int)time(0));
    //for(int j=0; j<100; j++)
    for(int i=0; i<n; i++){
        rand_index=(size_t)((double)rand()/(double)(RAND_MAX)*(n-1));
        tmp=data[i];
        data[i] = data[rand_index];
        data[rand_index]=tmp;
    }
}

void List_random_fill(List l, int n){
    int num;
    srand((int)time(0));
    for(int i=0; i<n; i++)
    {
        num = (int)((float)rand()/RAND_MAX*(n*10));
        List_append(l, &num);
    }
}

void List_fill_reversed(List l, int n){
    for(int i=n-1; i>=0; i--)
    {
        List_append(l, &i);
    }
}

void List_fill_to_n(List l, int n){
    for(int i=0; i<n; i++)
    {
        List_append(l, &i);
    }
}

int List_append_csv(List l, CSV  csv)
{
    int num,
        res;

    for(int i=0; i<csv.size; i++)
    {
        res=to_num(csv.data[i],&num);
        if(res){
            return -1;
        }
        List_append(l, &num);
    }
    return 0;
}

char tmp_file_buf[128];
void List_save(List l, FILE *f)
{

    int num_length = (int)(log(get_max(l))/log(10))+1,
        line_length = sqrt(List_size(l))/sqrt(num_length)+1;

    snprintf(tmp_file_buf,128,"%%%dd, ", num_length);
    for(int i=0; i<List_size(l); i++)
    {
        if((i%line_length)==0)
            fputc('\n', f);

        fprintf(f,tmp_file_buf,*(S_TYPE*)List_get(l,i));
    }
    fseek(f,-1,SEEK_CUR);
    fputc('\n',f);
}

int List_verify(List l)
{

    const char *title_backup = GUI_Window_title_get(0);
    GUI_Window_title_set(0, "VERIFY");
    GUI_Window_marks_clear(0);
    S_TYPE *start = List_start(l),
           *end = List_end(l)-1;
    size_t i=0;

    for(; start<end-1; start++, i++){
        if(*start>*(start+1)){
            GUI_Window_marks_add(0, i, (struct Color){255,0,0});
            return 1;
        }else{
            GUI_Window_marks_add(0, i, (struct Color){0,255,0});
        }
        GUI_Window_marks_add(0, ++i, (struct Color){0,255,0});
        GUI_wait();
        GUI_update(0);
    }
    GUI_update(1);

    GUI_Window_title_set(0, title_backup);
    return 0;
}

unsigned int get_max(List l)
{
    S_TYPE *start = List_start(l),
           *end = List_end(l),
           max = 0;

    for(; start!=end; start++){
        if(*start>max)
            max=*start;
    }
    return max;
}

int to_num(char* str, int* num){
    return (sscanf(str,"%d",num)!=1);
}

