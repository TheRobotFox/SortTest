#include "argparse.h"
#include "Sort.h"
#include "csv.h"
#include "GUI.h"
#include <math.h>
#include <stdio.h>


typedef struct{
    List l;
    SortAlg *SortAlgs;
    int Sortalg_count;
    int iterations;
    FILE* output;
    int delay;
} Settings;

void free_Settings(Settings* Conf){
    List_free(Conf->l);
	if(Conf->output)
	    fclose(Conf->output);
    //free(Conf->SortAlgs);
}

void printSettings(Settings* Conf){
    printf("\nSortingalgorithms:\n");
    for(int i=0; i<Conf->Sortalg_count; i++)
        printf("    %s\n",Conf->SortAlgs[i].name);
    printf("\nList_length: %ld\n",List_size(Conf->l));
    printf("Iterations: %d\n\n",Conf->iterations);
}

struct GUI_conf gui_conf = {0};

List Test(List SortList, SortAlg *Alg, int iterations){
    List l = List_create(sizeof(S_TYPE));
    for(int iter=0; iter<iterations; iter++)
    {
        List_copy(l, SortList);
    DBGP;
        status_reset();
        GUI_Window_set_list(0, l);
        GUI_Window_do_render(0,1); // start render
    DBGP;
        Sleep_while_active(1000);
    DBGP;

    #ifndef NSPIRE
        Alg->time_start = clock();
    #endif

        Alg->func(l);
    DBGP;
        GUI_update(1);

    #ifndef NSPIRE
        Alg->time+=clock()-Alg->time_start;
    #endif

        if(List_verify(l))
            printf("ERROR!\n%s misperformed on %d\n",Alg->name, iter);

        Sleep_while_active(1000);

    }
    Alg->time/=iterations;
    Alg->comp=getComp();
    Alg->swap=getSwap();

    GUI_Window_do_render(0,0); // stop render
    return l;
}


#ifdef NSPIRE
#define OUT_FORMAT "%-20s - %.3fs\n%10lu swaps %10lu compares\n\n"
#else
#define OUT_FORMAT "%-20s %fs %19lu swaps %19lu compares\n"
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
    CSV argv_csv = CSV_load_string_del(CMDBUF,strlen(CMDBUF),' ');

    if(argparse(argv_csv.size,argv_csv.data)){ // List length 34583458345
                                               // random crash on start TI
        CSV_free(argv_csv);
        continue;
    }
    CSV_free(argv_csv);
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
    gui_conf.delay=arg_get('v')->f_val;
    gui_conf.mergedistance=arg_get('m')->val;
    gui_conf.skip_pixels=arg_get('s')->val;

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
            Conf.l=arg_get(l_args[i])->l;
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

    if(!List_size(Conf.l)){
        printf("List is empty\n");
				goto end;
    }


    printSettings(&Conf);
    DBGP;
    if(Conf.output){
    DBGP;
        fprintf(Conf.output,"Unsorted List:");
    DBGP;
        List_save(Conf.l, Conf.output);
    }

    DBGP;

    gui_conf.activate = gui_conf.delay>0;
    gui_conf.do_render=1;

    DBGP;

    GUI_create(&gui_conf);
    DBGP;
    if(gui_conf.activate){
        // gui_conf.delay=10000.0/pow(Conf.List.size,1.9);
        while(!gui_conf.active)
            Sleep(20);
        GUI_windows_append(); // root window -> 0
    }

    DBGP;
    // Test each Algorithm
    List l;

    for(int i=0; i<Conf.Sortalg_count; i++){
        // Update GUI Name
        if(gui_conf.active)
            gui_conf.Alg=Conf.SortAlgs+i;
    DBGP;
        l = Test(Conf.l,Conf.SortAlgs+i,Conf.iterations);

#ifdef NSPIRE
    while(isKeyPressed(KEY_NSPIRE_ESC))
        ;
#endif

    DBGP;
        if(Conf.output){
            fprintf(Conf.output,"\n\n%s:",Conf.SortAlgs[i].name);
            List_save(l, Conf.output);
            fprintf(Conf.output,OUT_FORMAT, Conf.SortAlgs[i].name, Conf.SortAlgs[i].time/CLOCKS_PER_SEC,Conf.SortAlgs[i].swap,Conf.SortAlgs[i].comp);
        }

        printf(OUT_FORMAT,Conf.SortAlgs[i].name,Conf.SortAlgs[i].time/CLOCKS_PER_SEC,Conf.SortAlgs[i].swap,Conf.SortAlgs[i].comp);
        List_free(l);
    }
end:
    if(gui_conf.activate)
        GUI_destroy();

    free_Settings(&Conf);

#ifdef NSPIRE
    } // LOOP TIL QUIT
#endif

    return 0;
}
