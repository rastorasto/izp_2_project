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
int map_alloc(Map *map,int rows, int cols){
    map->rows = 0;
    map->cols = 0;
    map->cells=(unsigned char *)malloc(sizeof(unsigned char)*rows*cols);
    if(map->cells==NULL){
        free(map->cells);
        return 1;
    }
    return 0;
}
int map_init(Map *map,int rows, int cols,int *number){
    map->rows=rows;
    map->cols=cols;
    for(int i=0; i < rows*cols;i++){
        map->cells[i]=(unsigned char)number[i];
    }
    return 0;
}
int map_dest(Map *map){
    free(map->cells);
    map->cols=0;
    map->rows=0;
    return 0;
}
int print_map(Map *map){
        printf("COLS: %d\n",map->cols);
        printf("ROWS: %d\n",map->rows);
        printf("MAPA: ");
        for(int i=0;i<map->rows*map->cols;i++){
            printf("%d ",map->cells[i]);
        }
        printf("\n");
        return 0;
}
int read_file(char *filename,int *rows,int *cols,int *number){
    int index=0;
    FILE *pFile = fopen(filename,"r");
    if(pFile == NULL){
        fclose(pFile);
        return 1;
    } else {
        fscanf(pFile,"%d %d",rows,cols);

        while(fscanf(pFile,"%d",&number[index]) != EOF){
            index++;
        }
        fclose(pFile);
        return 0;
    }
}
bool isborder(Map *map, int r, int c, int border){
    if(border==0){
        if(map->cells[(r*c)+c] & 4){
            return 1;
        }
    } else if (border==1){
        if(map->cells[(r*c)+c] & 2){
            return 1;
        } 
    } else if(border==2){
        if(map->cells[(r*c)+c] & 1){
            return 1;
        }
    }
    return 0;
}
int print_map_binary(Map *map) {
    printf("Binary representation of MAPA:\n");
    for (int i = 0; i < map->rows * map->cols; i++) {
        for (int bit = 2; bit >= 0; bit--) {
            printf("%d ", (map->cells[i] >> bit) & 1);
        }
        printf("\n");
    }
    return 0;
}
int main(int argc, char *argv[]){
    Map map;
    int rows=0;
    int cols=0;
    int number[100];
    if(argc == 5 && argv[4]){
        if(read_file(argv[4],&rows,&cols,number)){
            fprintf(stderr,"Failed to read from file.");
            return 1;
        }
    }
    if(map_alloc(&map, rows,cols)){
        fprintf(stderr,"Failed to allocate cells.");
        return 1;
    }
    map_init(&map, rows,cols,number);

    if(isborder(&map,0,1,0)){
        printf("Is border");
    }

    if(argc>1 && strcmp(argv[1], "--help")==0){
        print_help();
    } else if (argc==3 && strcmp(argv[1], "--test")==0){
        printf("test");
    } else if (argc==5 && strcmp(argv[1],"--rpath")==0){
        print_map(&map);
        print_map_binary(&map);
    } else if (argc==5 && strcmp(argv[1],"--lpath")==0){
        printf("lpath");
    } else {
        fprintf(stderr,"Invalid arguments. Use --help for more infomation.");
        return 1;
    }
    map_dest(&map);
    return 0;
}
