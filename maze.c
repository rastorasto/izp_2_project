#include<stdio.h>
#include<string.h>
#include<stdbool.h>
void print_help(){
    printf("Use --test file.txt to test the map.\n");
    printf("Use --rpath R C file.txt for searching with right hand rule.\n");
    printf("Use --lpath R C file.txt for searching with left hand rule.\n");
    printf("R stands for number of rows and C stands for number of collumns.\n");
}
void createmap(){
    
}
int main(int argc, char *argv[]){
    typedef struct {
        int rows;
        int cols;
        unsigned char *cells;
    } Map;
    Map mapa;
    char buffer[1000];
    char number;
    int index=0;
    FILE *pFile = fopen(argv[4],"r");
    if(pFile == NULL){
        fprintf(stderr,"File doesn't exist. Check ./maze --help for information.");
        fclose(pFile);
        return 1;
    } else {
        while((number = fgetc(pFile)) != EOF){
                if(number != ' ' && number != '\n'){
                    if(index==0){
                        mapa.cols = number - '0';
                        index++;
                    } else if (index==1) {
                        mapa.rows = number - '0';
                        index++;
                    } else {
                        strcat(buffer,&number);
                    }
                }
    }
        strcat(buffer,"\0");
        mapa.cells = (unsigned char*)buffer;
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
