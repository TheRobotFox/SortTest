#include "Sort.h"
#include <signal.h>
static size_t comp;
static size_t swap;

void reset(){
    comp=0;
    swap=0;
}

size_t getComp(){
    return comp;
}
size_t getSwap(){
    return swap;
}

void Swap(unsigned int a, unsigned int b, LIST *List){
	//if(a<0 || a>=List->size || b<0 || b<List->size){
		//printf("OOB Swap!");
		//raise(SIGINT);
	//}
    int tmp;
    tmp=List->data[a];
    List->data[a]=List->data[b];
    List->data[b]=tmp;
    swap++;
}

int Compare(unsigned int a, unsigned int b, LIST *List){
    comp++;
    gui_wait();
    if(List){
        gui_pointer1(a);
        gui_pointer2(b);
        return List->data[a]>List->data[b];
    }
    return a>b;
}

void BubbleSort(LIST *List){

    char sorted=0;
    int size=List->size;
    while(!sorted){
        sorted=1;
        for(int i=0; i<size-1; i++){
            if(Compare(i,i+1,List)){
                Swap(i,i+1,List);
                sorted=0;
            }
        }
        size--;
    }
}

void Bad_BubbleSort(LIST *List){

    for(int i=0; i<List->size; i++){
        for(int x=0; x<List->size-1; x++){
            if(Compare(x,x+1,List)){
                Swap(x,x+1,List);
            }
        }
    }
}

void SelectionSort(LIST *List){

    int smallest_index;
    for(int i=0; i<List->size-1;i++){
        smallest_index=i;
        for(int e=i+1; e<List->size;e++){
            if(Compare(smallest_index,e,List))
                smallest_index=e;
        }
        if(smallest_index!=i){
            Swap(i,smallest_index,List);
        }

    }
}

void StalinSort(LIST *List){
    int commrads = List->size;

    for(int i=0; i<commrads-1;){
        while(Compare(i,i+1,List)){
            for(int commrad=i+1; commrad<commrads-1; commrad++){
                List->data[commrad]=List->data[commrad+1];
            }
            commrads--;
            List->data[commrads]=0;
            List->size=commrads;
            continue;
        }
        i++;
    }
}

#ifdef NSPIRE
#define RADIX 4
#else
#define RADIX 256 // FASTEST 256
#endif
#define RADIX_ITER (int)(sizeof(int)*8.0/log2(RADIX))

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

void LSD_Radix(LIST *List){
    int Counts[RADIX];
    LIST Output=CreateList(List->size);
    Output.filled=List->filled;
    unsigned int *tmp;

    int iter = (int)(log2(get_max(List))/log2(RADIX)+1);

    for(int level=0; level<iter; level++){
        int shift=(level*(int)log2(RADIX));
        ClearArray(Counts);
        for(int i=0; i<List->size;i++){
            Counts[(List->data[i]>>shift)&(RADIX-1)]++;
        }
        for(int i=1; i<RADIX; i++){
            Counts[i]+=Counts[i-1];
        }
        for(int i=List->size-1; i>=0; i--){
            Output.data[--Counts[(List->data[i]>>shift)&(RADIX-1)]]=List->data[i];
            gui_wait();
        }
        gui_updateList(&Output);
        //memcpy(List->data,Output,List->size*sizeof(int));
        tmp=Output.data;
        Output.data=List->data;
        List->data=tmp;
    }
    if(RADIX_ITER%2){
        tmp=Output.data;
        Output.data=List->data;
        List->data=tmp;
    }
    gui_updateList(List);
    free_List(&Output);
}


void InsertionSort(LIST *List){
    int j;
    unsigned int tmp;
    for(int i=1; i<List->size;i++){
        tmp=List->data[i];
        gui_pointer1(i);
        j=i-1;
        while(j >=0 && Compare(List->data[j],tmp,0)){
            gui_pointer2(j);
            List->data[j+1]=List->data[j];
            j--;
        }
        List->data[j+1]=tmp;
    }

}

void CocktailshakerSort(LIST *List){

    unsigned int iteration=0, i=0, swaped=2;
    int direction=1;

    do{
        swaped--;
        for(i+=direction; (i<List->size-iteration-1) && (i>=iteration); i+=direction){
            if(Compare(i,i-direction,List)^(direction>0)){
                Swap(i,i-direction,List);
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

void BogoSort(LIST *List){
    while (Verify(List)){
        int i=(float)rand()/RAND_MAX*(List->size-1);
        int j=(float)rand()/RAND_MAX*(List->size-1);
        Swap(i,j,List);
        gui_wait();

    }
}

void QuickSort(int left, int right, LIST* List){
    if(left<right){
        unsigned int pivot = right;
        int i=left,j=right-1;
        while(i<j){
            for(;i<right && Compare(pivot,i,List);i++);
            for(;j>left && !Compare(pivot,j,List);j--);
            if(i<j)
                Swap(i,j,List);
        }
        if(Compare(i,pivot,List))
            Swap(i,pivot,List);
        QuickSort(left,i-1,List);
        QuickSort(i+1,right,List);
    }
}

void QuickSort_launcher(LIST *List){
    QuickSort(0,List->size-1,List);
}
