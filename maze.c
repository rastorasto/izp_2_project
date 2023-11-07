#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
void print_help(){
    printf("Use --test file.txt to test the map.\n");
    printf("Use --rpath R C file.txt for searching with right hand rule.\n");
    printf("Use --lpath R C file.txt for searching with left hand rule.\n");
    printf("R stands for number of rows and C stands for number of collumns.\n");
}
typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;
int map_alloc(Map *mapa,int rows, int cols){
    mapa->rows = 0;
    mapa->cols = 0;
    mapa->cells=malloc(sizeof(unsigned char)*rows*cols);
    if(mapa->cells==NULL){
        return 1;
    }
    return 0;
}
int map_init(Map *mapa,int rows, int cols,int number[]){
    mapa->rows=rows;
    mapa->cols=cols;
    for(int i=0; i < rows*cols;i++){
        mapa->cells[i]=number[i];
    }
    return 0;
}
int print_map(Map *mapa){
        printf("COLS: %d\n",mapa->cols);
        printf("ROWS: %d\n",mapa->rows);
        printf("MAPA: ");
        for(int i=0;i<mapa->rows*mapa->cols;i++){
            printf("%d ",mapa->cells[i]);
        }
        return 0;
}
int main(int argc, char *argv[]){
    Map mapa;
    int index=0;
    int rows;
    int cols;
    int number[100];
    FILE *pFile = fopen(argv[4],"r");
    if(pFile == NULL){
        fprintf(stderr,"File doesn't exist. Check ./maze --help for information.");
        fclose(pFile);
        return 1;
    } else {
        fscanf(pFile,"%d %d",&rows,&cols);
        if(map_alloc(&mapa, rows,cols)){
            fprintf(stderr,"Failed to allocate cells.");
            return 1;
        }

        while(fscanf(pFile,"%d",&number[index]) != EOF){
            index++;
        }

        fclose(pFile);
        map_init(&mapa, rows,cols,number);
    }


    if(argc>1 && strcmp(argv[1], "--help")==0){
        print_help();
    } else if (argc==3 && strcmp(argv[1], "--test")==0){
        printf("test");
    } else if (argc==5 && strcmp(argv[1],"--rpath")==0){
        print_map(&mapa);
    } else if (argc==5 && strcmp(argv[1],"--lpath")==0){
        printf("lpath");
    } else {
        fprintf(stderr,"Error. No input detected.");
        return 1;
    }
    return 0;
}
