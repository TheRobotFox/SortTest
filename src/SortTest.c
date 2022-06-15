#include "argparse.h"
#include "Sort.h"
#include "List.h"
#include "csv.h"
#include "GUI.h"
#include "math.h"

typedef struct{
    LIST List;
    SortAlg *SortAlgs;
    int Sortalg_count;
    int iterations;
    FILE* output;
    int delay;
} Settings;

void free_Settings(Settings* Conf){
    free_List(&Conf->List);
	if(Conf->output)
	    fclose(Conf->output);
    //free(Conf->SortAlgs);
}

void printSettings(Settings* Conf){
    printf("\nSortingalgorithms:\n");
    for(int i=0; i<Conf->Sortalg_count; i++)
        printf("    %s\n",Conf->SortAlgs[i].name);
    printf("\nList_length: %d\n",Conf->List.size);
    printf("Iterations: %d\n\n",Conf->iterations);
}

infoGUI info= {0};

LIST Test(LIST* SortList, SortAlg *Alg, int iterations){
    LIST List = CreateList(SortList->size);
    for(int iter=0; iter<iterations; iter++){
		List.size=SortList->size;
        for(int i=0; i<List.size; i++)
            List.data[i]=SortList->data[i];
        reset();
        gui_updateList(&List);
        gui_pointer1(-1);
        gui_pointer2(-1);
    	for(int i=0; info.active && i<50; i++){
    	    Sleep(20);
    	}
        #ifndef NSPIRE
        Alg->time_start = clock();
        #endif
        Alg->func(&List);
        #ifdef NSPIRE
        gui_update();
        #else
        Alg->time+=clock()-Alg->time_start;
        #endif
    	for(int i=0; info.active && i<50; i++){
    	    Sleep(20);
    	}
    }
    Alg->time/=iterations;
    Alg->comp=getComp();
    Alg->swap=getSwap();

    if(Verify(&List))
        printf("ERROR!\n%s misperformed\n",Alg->name);

	gui_updateList(NULL);
    return List;
}


#ifdef NSPIRE
#define OUT_FORMAT "%-20s - %.3fs\n%10llu swaps %10llu compares\n\n"
#else
#define OUT_FORMAT "%-20s %fs %19llu swaps %19llu compares\n"
#endif



#ifdef NSPIRE

#define CMDLINELENGTH 512
#define NAME "SortTest.tns"
char CMDBUF[CMDLINELENGTH]= NAME;

int getcmd(){
    size_t i=sizeof(NAME)-1;
    char c;
    CMDBUF[i]=' ';
    i++;
    CMDBUF[i]=0;
    printf("%s",CMDBUF);
    fflush(stdout);
    fflush(stdin);
    while(i<CMDLINELENGTH)
    {
        c=getchar();
        if(c=='\n'){
            if(isKeyPressed(KEY_NSPIRE_ESC))
                return 1;
            getchar(); //catch second newline; why ???
            break;
        }
        CMDBUF[i]=c;
        i++;
    }
    CMDBUF[i]=0;
    return 0;
}

int main(void){
    while(!getcmd()){
    wait_no_key_pressed();
    CSV argv_csv = split(CMDBUF,strlen(CMDBUF),' ');

    if(argparse(argv_csv.size,argv_csv.data)){
        free_CSV(argv_csv);
        continue;
    }
    free_CSV(argv_csv);
#else
int main(int argc, char** argv){
    if(argparse(argc,argv))
        return 1;
#endif

    Settings Conf = {0};

    Conf.output=arg_get('o')->File;
    Conf.iterations=arg_get('i')->val;
    Conf.Sortalg_count=arg_get('a')->length;
    Conf.SortAlgs=arg_get('a')->sorting_algs;
    int GUI = (arg_get('v')->f_val>=0);

    int error=0;

    if(Conf.iterations<=0){
        printf("Invalid value for Iterations %d\n",Conf.iterations);
        error++;
    }
    if(Conf.Sortalg_count==0){
        printf("Must specify at least one Sortingalgorithm\n");
        error++;
    }

    if(error){
        #ifdef NSPIRE
        continue;
        #else
        return 1;
        #endif
    }

    char l_args[]="lrc";
    for(unsigned int i=0; i<sizeof(l_args)-1; i++){
        if(arg_get(l_args[i])->val){
            Conf.List=arg_get(l_args[i])->List;
            error++;
        }
    }

    if(error!=1){
        printf("One List must be specified!\n");
        if(error==0)
            #ifdef NSPIRE
            continue;
            #else
            return 1;
            #endif
				goto end;
    }

    if(Conf.List.size==0){
        printf("List is empty\n");
				goto end;
    }


    printSettings(&Conf);
    if(Conf.output){
        fprintf(Conf.output,"Unsorted List:");
        List_to_File(Conf.output,&Conf.List);
    }

    LIST List={0};


    // Create GUI Thread
    if(GUI){
        info.activate=1;
        info.delay=arg_get('v')->f_val;
        #ifdef _WIN32
        // info.delay=10000.0/pow(Conf.List.size,1.9);
        info.hinstance=GetModuleHandle(0);
        #endif
    }
    #ifdef _WIN32
    HANDLE Thread;
    SECURITY_ATTRIBUTES sa = {0};
    Thread = CreateThread(&sa,0,create_gui,&info,0,0);
    #elif defined(__unix__)
    pthread_t Thread;
    pthread_create(&Thread, NULL, create_gui, &info);
    #elif defined(NSPIRE)
    create_gui(&info);
    #endif

    while(!info.active && info.activate)
        Sleep(10);
    // Test each Algorithm
    for(int i=0; i<Conf.Sortalg_count; i++){
        // Update GUI Name
        if(info.active)
            info.Alg=Conf.SortAlgs+i;
        List = Test(&Conf.List,&Conf.SortAlgs[i],Conf.iterations);
        if(Conf.output){
            fprintf(Conf.output,"\n\n%s:",Conf.SortAlgs[i].name);
            List_to_File(Conf.output,&List);
            fprintf(Conf.output,OUT_FORMAT,Conf.SortAlgs[i].time/CLOCKS_PER_SEC,Conf.SortAlgs[i].swap,Conf.SortAlgs[i].comp);
        }

        printf(OUT_FORMAT,Conf.SortAlgs[i].name,Conf.SortAlgs[i].time/CLOCKS_PER_SEC,Conf.SortAlgs[i].swap,Conf.SortAlgs[i].comp);
        if(List.size>0)
            free_List(&List);
    }
end:
    #ifdef NSPIRE
    free_Settings(&Conf);
    } // LOOP TIL QUIT

    #else
    if(info.active){
		info.active=0;
		#ifdef _WIN32_
        WaitForSingleObject(Thread,INFINITE);
		#elif defined(__unix__)
		pthread_join(Thread, NULL);
		#endif
	}
    free_Settings(&Conf);
    #endif

    return 0;
}
