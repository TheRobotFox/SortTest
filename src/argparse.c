#include "argparse.h"


typedef struct{
    short type;
    char* name;
    int (*resolve)(int ,arg*, int*, int, char**);
} arg_resolver;

#ifdef NSPIRE
#define SEP "-"
#else
#define SEP "_"
#endif
// Sortingalgorithms

SortAlg Sorting_Algorithms[]= {
    {"Bad_BubbleSort",Bad_BubbleSort},
    {"BubbleSort",BubbleSort},
    {"SelectionSort",SelectionSort},
    {"StalinSort",StalinSort},
    {"LSD"SEP"RadixSort",LSD_Radix},
    {"InsertionSort",InsertionSort},
    {"CocktailshakerSort",CocktailshakerSort},
    {"QuickSort",QuickSort_launcher},
    {"ReculinSort", ReculinSort},
//#ifndef NSPIRE
    {"BogoSort",BogoSort}
//#endif



};
const int Sorting_Algorithms_count = sizeof(Sorting_Algorithms)/sizeof(SortAlg);

//Filling Commands

const FillCmd Fill_Commands[]={
    {"fill",List_fill_to_n, "Normal fill 0-n"},
    {"fill"SEP"reversed",List_fill_reversed,"Reverse order fill 0-n"},
    {"fill"SEP"random",List_fill_random, "Fill 0-n and randomize"},
    {"random",List_random_fill,"Fill with n random numbers 0-(n*10)"}
};

const int Fill_Commands_count = sizeof(Fill_Commands)/sizeof(FillCmd);

// STANDARD

int getString(int arg_list_index ,arg* arg_list, int* arg_index, int argc, char** argv)
{
    arg_list[arg_list_index].str=argv[(*arg_index)+1];
    return 0;
}

int getInt(int arg_list_index ,arg* arg_list, int* arg_index, int argc, char** argv)
{
    if(to_num(argv[(*arg_index)+1],&arg_list[arg_list_index].val)){
        sprintf( arg_list[arg_list_index].Error,"Could not interpret %s as int",argv[(*arg_index)+1]);
        return 1;
    }
    return 0;
}

int getFloat(int arg_list_index ,arg* arg_list, int* arg_index, int argc, char** argv)
{
    float x;
    if(sscanf(argv[(*arg_index)+1],"%f",&x)!=1){
        sprintf( arg_list[arg_list_index].Error,"Could not interpret %s as float",argv[(*arg_index)+1]);
        return 1;
    }
    arg_list[arg_list_index].f_val=x;
    return 0;
}

int getFlag(int arg_list_index ,arg* arg_list, int* arg_index, int argc, char** argv)
{
    arg_list[arg_list_index].boolean=1;
    return 0;
}



int getCSV(int arg_list_index ,arg* arg_list, int* arg_index, int argc, char** argv)
{
    arg_list[arg_list_index].csv=CSV_load_string(argv[(*arg_index)+1],strlen(argv[(*arg_index)+1]));
    if(arg_list[arg_list_index].csv.size==-1){
        sprintf( arg_list[arg_list_index].Error,"Unable to create CSV");
        return -1;
    }
    return 0;

}

int getFileIn(int arg_list_index ,arg* arg_list, int* arg_index, int argc, char** argv)
{
    char* Path = argv[(*arg_index)+1];
    arg_list[arg_list_index].File=fopen(Path,"r");
    if (!arg_list[arg_list_index].File){
        sprintf( arg_list[arg_list_index].Error,"Unable to open File %s",Path);
        return 1;
    }
    return 0;
}

int getFileOut(int arg_list_index ,arg* arg_list, int* arg_index, int argc, char** argv)
{
    char* Path = argv[(*arg_index)+1];
    arg_list[arg_list_index].File=fopen(Path,"w");
    if (!arg_list[arg_list_index].File){
        sprintf( arg_list[arg_list_index].Error,"Unable to open File %s",Path);
        return 1;
    }
    return 0;
}


// COSTUM

int readFile(int arg_list_index ,arg* arg_list, int* arg_index, int argc, char** argv)
{
    CSV csv=CSV_load_file(argv[(*arg_index)+1]);
    if(csv.size==-1){
        sprintf( arg_list[arg_list_index].Error,"Unable to create CSV");
        return 1;
    }

    List l = List_create(sizeof(S_TYPE));
    if(List_append_csv(l, csv)){
        CSV_free(csv);
        sprintf( arg_list[arg_list_index].Error,"Unable to create List");
        return 1;
    }
    CSV_free(csv);

    arg_list[arg_list_index].l=l;
    return 0;
}

int getList(int arg_list_index ,arg* arg_list, int* arg_index, int argc, char** argv)
{
    CSV csv = CSV_load_string(argv[(*arg_index)+1],strlen(argv[(*arg_index)+1]));
    List l = List_create(sizeof(S_TYPE));
    if(List_append_csv(l, csv)){
        CSV_free(csv);
        sprintf( arg_list[arg_list_index].Error,"Unable to create List");
        return 1;
    }
    CSV_free(csv);

    arg_list[arg_list_index].l=l;
    return 0;
}

int getCmd(int arg_list_index ,arg* arg_list, int* arg_index, int argc, char** argv)
{
    CSV CSV = CSV_load_string(argv[(*arg_index)+1],strlen(argv[(*arg_index)+1]));
    int size = 0,
        num = 0,
        error;

    for(int i=1; i<CSV.size; i+=2){
        error=to_num(CSV.data[i],&num);
        if(error){
            sprintf( arg_list[arg_list_index].Error,"Could not interpret %s as integer",CSV.data[i]);
            return 1;
        }
        size+=num;
    }

    List l = List_create(sizeof(S_TYPE));
    List_reserve(l, size);

    for(int c=0; c<CSV.size; c+=2)
    {
        error=1;
        for(int i=0; i<Fill_Commands_count; i++)
        {
            if (!strcmp(CSV.data[c],Fill_Commands[i].name)){
                if(to_num(CSV.data[c+1],&num)){
                    sprintf( arg_list[arg_list_index].Error,"Could not interpret %s as integer",CSV.data[i]);
					goto error;
                }
                Fill_Commands[i].func(l, num);
                error=0;
            }
        }
        if(error){
            sprintf(arg_list[arg_list_index].Error,"Unknown Command: %s",CSV.data[c]);
			goto error;
        }
    }
    arg_list[arg_list_index].l=l;
    CSV_free(CSV);
    return 0;

	error:
	CSV_free(CSV);
	List_free(l);
	return 1;
}

int getAlgs(int arg_list_index ,arg* arg_list, int* arg_index, int argc, char** argv)
{
    CSV csv = CSV_load_string(argv[(*arg_index)+1],strlen(argv[(*arg_index)+1]));
    arg_list[arg_list_index].sorting_algs = malloc(csv.size*sizeof(SortAlg));
    arg_list[arg_list_index].length=csv.size;
    int error;
    if(csv.size==-1){
        printf("CSV is empty");
        return 1;
    }
    for(int a=0; a<csv.size; a++){
        error=1;
        for(int i=0; i<Sorting_Algorithms_count; i++){
            if (!strcmp(csv.data[a],Sorting_Algorithms[i].name)){
                arg_list[arg_list_index].sorting_algs[a]=Sorting_Algorithms[i];
                error=0;
            }
        }
        if(error){
            sprintf( arg_list[arg_list_index].Error,"%s: algorithm does not exist",csv.data[a]);
            CSV_free(csv);
            return 1;
        }
    }
    CSV_free(csv);
    return 0;
}


const arg_resolver arg_resolvers[]={
    {'S',"[String]",getString},
    {'I',"[int]",getInt},
    {'B',"",getFlag},
    {'F',"[float]",getFloat},

    {'O',"[Path]",getFileOut},
    {'F',"[Path]",getFileIn},
    {'L',"[CSV]",getCSV},

    {0,"[Path]",readFile},
    {1,"[CSV]",getList},
    {2,"[CSV]",getCmd},
    {3,"[CSV]",getAlgs}

};
const int arg_resolver_count = sizeof(arg_resolvers)/sizeof(arg_resolvers[0]);

arg arguments[]={
    {'c',2  ,"Create List via filling commands"},
    {'l',0  ,"load CSV from file"},
    {'r',1  ,"read raw csv"},
    {'a',3  ,"List of Sortingalgorithms to test"},
    {'o','O',"Output file path"},
    {'i','I',"num of iterations, results get averaged"},
    {'v','F',"Show Visualisation, float = delay"},
    {'s', 'I', "Render stepsize (Faster but lower quality)"},
    {'m', 'I', "Render collumn mergedistance in pixels"},
};

const int arguments_count = sizeof(arguments)/sizeof(arguments[0]);


int argparse(int argc, char** argv)
{

    for(int i=0; i<arguments_count; i++)
        arguments[i].data = NULL;

    // Set defaults
    arg_get('a')->sorting_algs=&Sorting_Algorithms[0];
    arg_get('a')->length=Sorting_Algorithms_count;
    arg_get('i')->val=1;
    arg_get('v')->f_val=-1.0;
    arg_get('o')->val=0;
    arg_get('m')->val=SKIP_PIXELS;
    arg_get('s')->val=MERGEDIS;

    for(int arg_index=1; arg_index<argc; arg_index++){
        if(argv[arg_index][0]=='-' && argv[arg_index][1]=='h'){
            print_usage(arguments, arguments_count);
            return 1;
        }
    }

    int found;
    for(int arg_index=1; arg_index<argc; arg_index++){
        found=0;
        for(int argument=0; argument<arguments_count; argument++){
            if(argv[arg_index][0]=='-'){
                if(argv[arg_index][1]==arguments[argument].id){
                    for(int resolver=0; resolver<arg_resolver_count; resolver++){
                        if(arg_resolvers[resolver].type==arguments[argument].type){
                            if(arg_resolvers[resolver].type!='B' && arg_index+1>=argc){
                                printf("Trailing Argument -%c!\n",arguments[argument].id);
                                found=1;
                                break;
                            }
                            if(arg_resolvers[resolver].resolve(argument,arguments,&arg_index,argc,argv)){
                                printf("An Error occured while parsing the Argumentlist! Argument -%c could not be resolved.\n  %s\n",arguments[argument].id,arguments[argument].Error);
                                return 1;
                            }
                            found=1;
                            break;
                        }
                    }
                    break;
                }
            }else{
                found=1;
                break;
            }
        }
        if(!found){
            printf("Unknown Argument -%c\n",argv[arg_index][1]);
        }
    }
    return 0;
}

void print_usage(){
    // Print Args
    printf("Arguments:\n");
    for(int i=0; i<arguments_count; i++){
        printf("  -%c ",arguments[i].id);
        for(int resolver=0; resolver<arg_resolver_count; resolver++){
            if(arg_resolvers[resolver].type==arguments[i].type){
                printf("%-12s",arg_resolvers[resolver].name);
                break;
            }
        }
        if(arguments[i].describtion)
            printf(" - %s\n", arguments[i].describtion);
    }

    // Print Extra
    printf("\nSortingalgoritms:\n");

    for(int i=0; i<Sorting_Algorithms_count; i++){
        printf("  %s\n",Sorting_Algorithms[i].name);
    }
    printf("\nFillingcommands:\n");
    for(int i=0; i<Fill_Commands_count; i++){
        printf(" %-13s - %s\n",Fill_Commands[i].name,Fill_Commands[i].description);
    }
}

arg* arg_get(char id){
    for(int i=0; i<arguments_count; i++)
        if(arguments[i].id==id)
            return arguments+i;
    return 0;
}
