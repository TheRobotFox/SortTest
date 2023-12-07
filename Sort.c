#include "Sort.h"
#include "List/List.h"
#include "SortTest.h"

static size_t comp;
static size_t swap;

void status_reset()
{
    comp=0;
    swap=0;
}

size_t getComp()
{
    return comp;
}
size_t getSwap()
{
    return swap;
}

static void Swap(List l, size_t a, size_t b)
{

    swap++;
    List_swap(l, a, b);

    GUI_update(0);
    GUI_wait();
}

static int Compare(List l, size_t a, size_t b)
{
    static long long int a_last=-1, b_last=-1;
    comp++;
    GUI_update(0);
    GUI_wait();
    if(l){

        if(a_last!=-1){
            GUI_Window_marks_remove(0, a_last);
            GUI_Window_marks_remove(0, b_last);
        }
        GUI_Window_marks_add(0, a, (struct Color){255,0,0});
        GUI_Window_marks_add(0, b, (struct Color){0,255,0});
        a_last=a;
        b_last=b;
        return LIST_get(S_TYPE)(l, a)>LIST_get(S_TYPE)(l, b);
    }
    return a>b;
}

void BubbleSort(List l)
{

    char sorted=0;
    int size=List_size(l);
    while(!sorted){
        sorted=1;
        for(int i=0; i<size-1; i++){
            if(Compare(l,i,i+1)){
                Swap(l,i,i+1);
                sorted=0;
            }
        }
        size--;
    }
}

void Bad_BubbleSort(List l)
{

    for(int i=0; i<List_size(l); i++){
        for(int x=0; x<List_size(l)-1; x++){
            if(Compare(l,x,x+1)){
                Swap(l,x,x+1);
            }
        }
    }
}

void SelectionSort(List l)
{

    int smallest_index;
    for(int i=0; i<List_size(l)-1;i++){
        smallest_index=i;
        for(int e=i+1; e<List_size(l);e++){
            if(Compare(l,smallest_index,e))
                smallest_index=e;
        }
        if(smallest_index!=i){
            Swap(l,i,smallest_index);
        }

    }
}

void StalinSort(List l)
{
    for(int i=0; i<List_size(l)-1;i++){
        while(i<List_size(l)-1){
            if(Compare(l,i,i+1)){
                List_rmi(l, i+1);
                continue;
            }
            break;
        }
    }
}

#ifdef NSPIRE
#define RADIX 4
#else
#define RADIX 2 // FASTEST 256
#endif

#ifdef TCC
void memcpy(unsigned char *a, unsigned char *b, int size){
    for(int i=0; i<size; i++){
        a[i]=b[i];
    }
}
#endif

static void ClearArray(int* a){
    for(int i=0; i<RADIX; i++){
        a[i]=0;
    }
}

void LSD_Radix(List l)
{
    int Counts[RADIX],
        size = List_size(l),
        *data;
    List l2 = LIST_create(S_TYPE),
         *input=&l,
         *output=&l2;

    List_resize(l2, size);


    // backup main window title
    const char *title_backup = GUI_Window_title_get(0);
    GUI_Window_title_set(0, "primary buffer");

    // setup second buffer window
    GUI_Window_id win = GUI_windows_append();
    GUI_Window_set_list(win, l2);
    GUI_Window_do_render(win, 1);
    GUI_Window_title_set(win, "secondary buffer");

    void *tmp;

    int iter = (int)(log2(get_max(l))/log2(RADIX)+1);

    for(int level=0; level<iter; level++){
        data = List_start(*output);
        for(int i=0; i<size; i++)
            data[i]=0;

        int shift=(level*(int)log2(RADIX));
        ClearArray(Counts);
        for(int i=0; i<List_size(*input);i++){
            Counts[(LIST_get(S_TYPE)(*input, i)>>shift)&(RADIX-1)]++;
        }
        for(int i=1; i<RADIX; i++){
            Counts[i]+=Counts[i-1];
        }
        for(int i=List_size(*input)-1; i>=0; i--){
            LIST_get(S_TYPE)(*output,--Counts[(LIST_get(S_TYPE)(*input, i)>>shift)&(RADIX-1)]) = LIST_get(S_TYPE)(*input, i);
            GUI_wait();
            GUI_update(0);
        }

        // swap input / output
        tmp=input;
        input=output;
        output=tmp;

    }
    // uneven iteration -> swap to main list
    if(iter%2)
        List_copy(l,l2);

    GUI_windows_remove(win);
    List_free(l2);

    // reset main window title
    GUI_Window_title_set(0, title_backup);
}


void InsertionSort(List l)
{
    int j, p1=-1, p2=-1;
    S_TYPE tmp;
    for(int i=1; i<List_size(l);i++){
        tmp=LIST_get(S_TYPE)(l, i);
        if(p1!=-1)
            GUI_Window_marks_remove(0, p1);
        GUI_Window_marks_add(0, i, (struct Color){255,0,0});
        p1=i;
        j=i-1;
        while(j >=0 && Compare(NULL,LIST_get(S_TYPE)(l, j),tmp)){
            if(p2!=-1)
                GUI_Window_marks_remove(0, p2);
        GUI_Window_marks_add(0, j, (struct Color){0,255,0});
            p2=j;
            LIST_get(S_TYPE)(l, j+1)=LIST_get(S_TYPE)(l, j);
            j--;
        }
        LIST_get(S_TYPE)(l, j+1)=tmp;
    }

}

void CocktailshakerSort(List l)
{

    unsigned int iteration=0, i=0, swaped=2;
    int direction=1;

    do{
        swaped--;
        for(i+=direction; (i<List_size(l)-iteration-1) && (i>=iteration); i+=direction){
            if(Compare(l,i,i-direction)^(direction>0)){
                Swap(l,i,i-direction);
                swaped=2;
            }
        }
        direction=-direction;
        if(direction>0){
            iteration++;
            i=iteration;
        }
    }while(swaped>0);
}

void BogoSort(List l)
{
#ifdef NSPIRE
    while (List_verify(l) && !isKeyPressed(KEY_NSPIRE_ESC)){
#else
    while (List_verify(l)){
#endif
        int i=(float)rand()/RAND_MAX*(List_size(l)-1);
        int j=(float)rand()/RAND_MAX*(List_size(l)-1);
        Swap(l,i,j);

    }

}

void QuickSort(int left, int right, List l)
{
    if(left<right){
        unsigned int pivot = right;
        int i=left, j=right-1;
        while(i<j){
            for(;i<right && Compare(l,pivot,i);i++);
            for(;j>left && !Compare(l,pivot,j);j--);
            if(i<j)
                Swap(l,i,j);
        }
        if(Compare(l,i,pivot))
            Swap(l,i,pivot);
        QuickSort(left,i-1,l);
        QuickSort(i+1,right,l);
    }
}

void QuickSort_launcher(List l)
{
    QuickSort(0,List_size(l)-1,l);
}

static GUI_Window_id u, o;
void _ReculinSort(List l, int iter)
{
    {
        List urlaub = List_create(sizeof(S_TYPE)),
             output = List_create(sizeof(S_TYPE));

        GUI_Window_set_list(u, urlaub);
        GUI_Window_set_list(o, output);
        GUI_Window_marks_clear(0);

        {
            int largest = 0,
                i;
            List_push(output, List_at(l, 0));
            GUI_Window_marks_add(0, 0, (struct Color){0,255,0});

            for(i=1; i<List_size(l)-iter; i++)
            {
                if(LIST_get(S_TYPE)(l, i)>LIST_get(S_TYPE)(l, largest)){
                    List_push(output, List_at(l, i));
                    GUI_Window_marks_add(0, i, (struct Color){0,255,0});
                    largest=i;
                }else{
                    List_push(urlaub, List_at(l, i));
                    GUI_Window_marks_add(0, i, (struct Color){255,0,0});
                }
                GUI_wait();
                GUI_update(0);
            }
            for(; i<List_size(l); i++)
                    List_push(output, List_at(l, i));
        }


        GUI_Window_do_render(u, 0);
        GUI_Window_do_render(o, 0);
        //printf("%d | %d\n", urlaub.filled, output.filled);
        if(!List_size(urlaub)){

            List_copy(l, output);

            List_free(urlaub);
            List_free(output);
            return;
        }

        List_copy(l, urlaub);
        List_concat(l, output);

        List_free(urlaub);
        List_free(output);
    }

        _ReculinSort(l, iter+1);
}

void ReculinSort(List l)
{
    u=GUI_windows_append();
    GUI_Window_foreground_set(u, (struct Color){255,0,0});
    GUI_Window_background_set(u, (struct Color){127,127,127});
    GUI_Window_title_set(u, "Urlaub");

    o=GUI_windows_append();
    GUI_Window_foreground_set(o, (struct Color){0,255,0});
    GUI_Window_background_set(o, (struct Color){127,127,127});
    GUI_Window_title_set(o, "Output");

    _ReculinSort(l, 0);
    GUI_windows_remove(u);
    GUI_windows_remove(o);
}

