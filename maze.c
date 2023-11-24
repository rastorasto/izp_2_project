#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#define righthand 1
#define lefthand 2
#define left_side 1
#define right_side 2
#define top_bot_side 3
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
        return 0;
    }
    return 1;
}
int map_init(Map *map,int rows, int cols,int *number){
    map->rows=rows;
    map->cols=cols;
    for(int i=0; i < rows*cols;i++){
        map->cells[i]=(unsigned char)number[i];
    }
    return 1;
}
int map_dest(Map *map){
    free(map->cells);
    map->cols=0;
    map->rows=0;
    return 1;
}/*
int print_map(Map *map){
        printf("ROWS: %d\n",map->rows);
        printf("COLS: %d\n",map->cols);
        printf("MAPA: ");
        for(int i=0;i<map->rows*map->cols;i++){
            printf("%d ",map->cells[i]);
        }
        printf("\n");
        return 1;
}*/
int read_file(char *filename,int *rows,int *cols,int *number,int *index){
    FILE *pFile = fopen(filename,"r");
    if(pFile == NULL){
        fclose(pFile);
        return 0;
    } else {
        fscanf(pFile,"%d %d",rows,cols);

        while(fscanf(pFile,"%d",&number[*index]) != EOF){
            (*index)++;
        }
        fclose(pFile);
        return 1;
    }
}
bool isborder(Map *map, int r, int c, int border){
    if(border==top_bot_side){
        if(map->cells[(r*map->cols)+c] & 4){
            return 1;
        }
    } else if (border==right_side){
        if(map->cells[(r*map->cols)+c] & 2){
            return 1;
        } 
    } else if(border==left_side){
        if(map->cells[(r*map->cols)+c] & 1){
            return 1;
        }
    }
    return 0;
}
/*
int print_map_binary(Map *map) {
    printf("Binary representation of MAPA:\n");
    for (int i = 0; i < map->rows * map->cols; i++) {
            if(i%map->cols == 0){
                printf("\n");
            }
        for (int bit = 2; bit >= 0; bit--) {
            printf("%d ", (map->cells[i] >> bit) & 1);
        }
        printf(" ");
    }
    return 0;
}*/
int check_file_cells(int index,int rows, int cols){
    if(index != rows*cols){
        return 0;
    } else {
        return 1;
    }
}
int check_borders(Map *map){
    for(int i=0;i<map->rows;i++){
        for(int j=0;j<map->cols-1;j++){
            if(!(isborder(map,i,j,right_side) == isborder(map,i,j+1,left_side))){
                return 0;
            }
            if(i < map->rows-1 && (i+j) % 2 != 0){
                if(!(isborder(map,i,j,top_bot_side) == isborder(map,i+1,j,top_bot_side))){
                    return 0;
                } 
            }
        }
    }
    return 1;
}
int start_border(Map *map, int r, int c, int leftright){
    if(leftright == righthand){
        if(r % 2 == 1 && c == 1){
            // printf("1 prava");
            return right_side;
        } else if (r % 2 == 0 && c==1){
            // printf("2 dolni");
            return top_bot_side;
        } else if (c == map->cols && r % 2 == 1){
            // printf("5 horni");
            return top_bot_side;
        } else if (r % 2 == 0 && c == map->cols) {
            // printf("6 leva");
            return left_side;
        } else if (r == 1){
            // printf("3 leva");
            return left_side;
        } else if (r == map->rows){
            // printf("4 prava");
            return right_side;
        }
    } else if (leftright ==lefthand){
        if(r % 2 == 1 && c == 1){
            // printf("1 leva");
            return top_bot_side;
        } else if (r % 2 == 0 && c==1){
            // printf("2 horni");
            return right_side; 
        } else if (c == map->cols && r % 2 == 1){
            // printf("5 dolni");
            return left_side;
        } else if (r % 2 == 0 && c == map->cols) {
            // printf("6 prava");
            return top_bot_side;
        } else if (r == 1){
            // printf("3 prava");
            return right_side;
        } else if (r == map->rows){
            // printf("4 leva");
            return left_side;
        }
    }
    return 0;

}
int finished(Map *map, int row, int col){
    if(row > map->rows || row < 1 || col > map->cols || col < 1){
        return 1;
    } else {
        return 0;
    }
}
int triangle_has_bottom(int row,int col){
    if((col % 2 == 1 && row % 2 == 0) || (col % 2 == 0 && row % 2 == 1)){
        return 1;
    } else {
        return 0;
    }
}
int triangle_up_down_changed(int border, int pos_row, int pos_col, int leftright){
    if(leftright==righthand){
        if(triangle_has_bottom(pos_row,pos_col)) { //normalny
            // border=(border%3)+1;
            if(border==top_bot_side){
                border=right_side;
            } else if (border==right_side){
                border=top_bot_side;
            } else if (border==left_side){
                border=left_side;
            }
            return border;
        } else { // hore nohami
            if(border==top_bot_side){
                border=left_side;
            } else if (border==right_side){
                border=right_side;
            } else if (border==left_side){
                border=top_bot_side;
            }
            return border;
        }
    } else if (leftright==lefthand){
            if(triangle_has_bottom(pos_row,pos_col)) { 
            if(border==top_bot_side){
                border=left_side;
            } else if (border==right_side){
                border=right_side;
            } else if (border==left_side){
                border=top_bot_side;
            }
            return border;
        } else { 
            if(border==top_bot_side){
                border=right_side;
            } else if (border==right_side){
                border=top_bot_side;
            } else if (border==left_side){
                border=left_side;
            }
            return border;
        }
    } else {
        return 0; // zbytocne asi
    }
}
int next_border(int border, int pos_row, int pos_col, int leftright){
    if(leftright==righthand){
        if(triangle_has_bottom(pos_row,pos_col)) { //normalny
            if(border==right_side){
                return left_side;
            } else {
                border=((border%3)+2);
                return border;
            }
        } else { // hore nohami
            border=(border%3)+1;
            return border;
        }
    } else if (leftright==lefthand){
        if(triangle_has_bottom(pos_row,pos_col)) { //normalny
            border=(border%3)+1;
            return border;
        } else { // hore nohami
            if(border==right_side){
                return left_side;
            } else {
                border=((border%3)+2);
                return border;
            }
        }
    } else {
        return 0; //zbytocne asi
    }
}
void move_position(int *pos_row, int *pos_col,int border){            
    if(border==right_side){
        (*pos_col)++;
    } else if(border==left_side){
        (*pos_col)--;
    } else if (border ==top_bot_side && triangle_has_bottom(*pos_row,*pos_col)){
        (*pos_row)++;
    } else if (border ==top_bot_side && !triangle_has_bottom(*pos_row,*pos_col)){
        (*pos_row)--;  
    }
}
void find_path(Map *map,int start_row,int start_col,int start_border_side,int leftright){
    int pos_row = start_row;
    int pos_col = start_col;
    int border = start_border_side;
    while(!finished(map, pos_row,pos_col)){
        printf("%d,%d\n",pos_row,pos_col);
        while(isborder(map,pos_row-1,pos_col-1,border)){
            border = next_border(border,pos_row,pos_col,leftright);
        }
        move_position(&pos_row,&pos_col,border);
        border = triangle_up_down_changed(border,pos_row,pos_col,leftright);
    }
}
int main(int argc, char *argv[]){
    Map map;
    int rows=0;
    int cols=0;
    int index = 0;
    int number[100];
    char vstup[100]={0};
    int start_border_side = 0;
    if(argc>1 && strcmp(argv[1], "--help")==0){
        print_help();
        return 0;
    }
    if(argc == 5){
        strcpy(vstup,argv[4]);
    } else if (argc == 3){
        strcpy(vstup,argv[2]);
    } 
    if(!read_file(vstup,&rows,&cols,number,&index)){
        fprintf(stderr,"Failed to read from file.");
        return 1;
    }
    if(!map_alloc(&map, rows,cols)){
        fprintf(stderr,"Failed to allocate cells.");
        return 1;
    }
    map_init(&map, rows,cols,number);
    if (argc==3 && strcmp(argv[1], "--test")==0){
        if(!check_file_cells(index,rows,cols)){
            printf("Invalid\n");
            return 1;
        } else if (!check_borders(&map)){
            printf("Invalid\n");
        } else {
            printf("Valid\n");
        }
    } else if (argc==5 && strcmp(argv[1],"--rpath")==0){        //spojit --rpath --lpath do jedneho ifu
        // print_map(&map);
        // print_map_binary(&map);
        int start_row = atoi(argv[2]);
        int start_col = atoi(argv[3]);
        if((start_border_side = start_border(&map,start_row,start_col,righthand))){
            find_path(&map,start_row,start_col,start_border_side,righthand);
        } else {
            return 1;
        }
    } else if (argc==5 && strcmp(argv[1],"--lpath")==0){
        int start_row = atoi(argv[2]);
        int start_col = atoi(argv[3]);
        if((start_border_side = start_border(&map,start_row,start_col,lefthand))){
            find_path(&map,start_row,start_col,start_border_side,lefthand);
        } else {
            return 1;
        }
    } else {
        fprintf(stderr,"Invalid arguments. Use --help for more infomation.");
        return 1;
    }
    map_dest(&map);
    return 0;
}
