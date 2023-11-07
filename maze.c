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
Map map_alloc(int rows, int cols){
    Map mapa = {.rows = rows, .cols=cols,.cells=NULL};
    mapa.cells=malloc(sizeof(int)*rows*cols);
    return mapa;
}
int main(int argc, char *argv[]){
    Map mapa;
    int number[100];
    int index=0;
    int rows;
    int cols;
    FILE *pFile = fopen(argv[4],"r");
    if(pFile == NULL){
        fprintf(stderr,"File doesn't exist. Check ./maze --help for information.");
        fclose(pFile);
        return 1;
    } else {
        fscanf(pFile,"%d %d",&rows,&cols);
        map_alloc(rows,cols);
        while(fscanf(pFile,"%d",&number[index]) != EOF){
            index++;
        }
        mapa.cells = (unsigned char*)number;
        fclose(pFile);
    }


    if(argc>1 && strcmp(argv[1], "--help")==0){
        print_help();
    } else if (argc==3 && strcmp(argv[1], "--test")==0){
        printf("test");
        printf("%hhu",mapa.rows);
    } else if (argc==5 && strcmp(argv[1],"--rpath")==0){
        printf("COLS: %d\n",mapa.cols);
        printf("ROWS: %d\n",mapa.rows);
        printf("MAPA: %s\n",mapa.cells);
    } else if (argc==5 && strcmp(argv[1],"--lpath")==0){
        printf("lpath");
    } else {
        print_help();
    }
    return 0;
}
