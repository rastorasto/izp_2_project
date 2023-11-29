#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#define righthand 1
#define lefthand 2
#define left_side 1
#define right_side 2
#define top_bot_side 3
#define MAX_FILENAME_LENGTH 100
#define MAX_MAP_CELLS 100
typedef struct {
    int rows;
    int cols;
    unsigned char *cells;
} Map;

int parse_args(char *value);
void print_help();
int map_alloc(Map *map, int rows, int cols);
int map_init(Map *map, int rows, int cols, int *cells_values);
int map_dest(Map *map);
int read_file(char *filename,int *rows, int *cols, int *cells_values, int *border);
bool isborder(Map *map, int r, int c, int border);
int check_file_cells(int index, int rows, int cols);
int check_borders(Map *map);
int can_start(Map *map, int r, int c);
int start_border(Map *map, int r, int c, int leftright);
int finished(Map *map, int row, int col);
int triangle_has_bottom(int rows, int col);
int tirangle_up_down_changed(int row, int col);
int next_border(int border, int pos_row, int pos_col, int leftright);
void move_position(int *pos_row, int *pos_col, int border);
void find_path(Map *map, int start_row, int start_col, int start_border_side, int leftright);

int main(int argc, char *argv[]){
    Map map;
    int rows = 0;
    int cols = 0;
    int index = 0;
    int cells_values[MAX_MAP_CELLS] = {0};
    char filename[MAX_FILENAME_LENGTH] = {0};
    int start_border_side = 0;
    if(argc==5){
        strcpy(filename,argv[4]);
        int start_row = parse_args(argv[2]);
        int start_col = parse_args(argv[3]);
        if(start_row == -1 || start_col == -1){
            fprintf(stderr,"Invalid arguments.");
            return 1;
        }
        if(!read_file(filename,&rows,&cols,cells_values,&index)){
            fprintf(stderr,"Failed to read from file.");
            return 1;
        }
        if(!map_alloc(&map, rows,cols)){
            fprintf(stderr,"Failed to allocate cells.");
            return 1;
        }
        map_init(&map, rows,cols,cells_values);
        if(!check_file_cells(index,rows,cols) || !check_borders(&map)){
            printf("Invalid\n");
            return 1;
        }
        if(strcmp(argv[1],"--rpath")==0){
            if(!can_start(&map,start_row,start_col)){
                fprintf(stderr,"Invalid start cell.");
                map_dest(&map);
                return 1;
            }
            start_border_side = start_border(&map,start_row,start_col,righthand);
            if(start_border_side){
                find_path(&map,start_row,start_col,start_border_side,righthand);
            } else {
                map_dest(&map);
                return 1;
            }
        } else if (strcmp(argv[1],"--lpath")==0){
            if(!can_start(&map,start_row,start_col)){
                map_dest(&map);
                fprintf(stderr,"Invalid start cell.");
                return 1;
            }
            start_border_side = start_border(&map,start_row,start_col,lefthand);
            if(start_border_side){
                find_path(&map,start_row,start_col,start_border_side,lefthand);
            } else {
                fprintf(stderr,"Failed to get start border side.");
                map_dest(&map);
                return 1;
            }
        } else {
            map_dest(&map);
            return 1;
        }
    } else if (argc==3 && strcmp(argv[1], "--test")==0){
        strcpy(filename,argv[2]);
        if(!read_file(filename,&rows,&cols,cells_values,&index)){
            fprintf(stderr,"Failed to read from file.");
            return 1;
        }
        if(!map_alloc(&map, rows,cols)){
            fprintf(stderr,"Failed to allocate cells.");
            return 1;
        }
        map_init(&map, rows,cols,cells_values);
        if(!check_file_cells(index,rows,cols) || !check_borders(&map)){
            printf("Invalid\n");
            return 1;
        } else {
            printf("Valid\n");
        }
    } else if (argc==2 && strcmp(argv[1], "--help")==0){
        print_help();
        return 0;
    } else {
        fprintf(stderr,"Invalid arguments. Use --help for more infomation.");
        return 1;
    }
}
int parse_args(char *value){
    char *endptr;
    long long_value = strtol(value,&endptr,0);
    if(*endptr != '\0' || value == endptr || !(long_value >= 0)){
        return -1;
    } else {
        return (int) long_value;
    }
}
void print_help(){
    printf("Use --test file.txt to test the map.\n");
    printf("Use --rpath R C file.txt for searching with right hand rule.\n");
    printf("Use --lpath R C file.txt for searching with left hand rule.\n");
    printf("R stands for number of rows and C stands for number of collumns.\n");
}
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
int map_init(Map *map,int rows, int cols,int *cells_values){
    map->rows=rows;
    map->cols=cols;
    for(int i=0; i < rows*cols;i++){
        map->cells[i]=(unsigned char)cells_values[i];
    }
    return 1;
}
int map_dest(Map *map){
    free(map->cells);
    map->cols=0;
    map->rows=0;
    return 1;
}
int read_file(char *filename,int *rows,int *cols,int *cells_values,int *index){
    FILE *pFile = fopen(filename,"r");
    if(pFile == NULL){
        return 0;
    } else {
        fscanf(pFile,"%d %d",rows,cols);
        char temp;
        while((temp = fgetc(pFile)) != EOF){
            if(temp >= '0' && temp <= '7'){
                cells_values[*index] = temp;
                (*index)++;
            }
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
int check_file_cells(int index,int rows, int cols){
    if(rows < 0 || cols < 0 || index != rows*cols){
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
int can_start(Map *map,int r, int c){
    if(c == 1 && !isborder(map, r-1,c-1,left_side)){
        return 1;
    } else if (c == map->cols && !isborder(map, r-1,c-1,right_side)){
        return 1;
    } else if ((r == 1 || r == map->rows) && c % 2 != 0 && !isborder(map, r-1,c-1,top_bot_side)){
        return 1;
    } else {
        return 0;
    }
}
int start_border(Map *map, int r, int c, int leftright){
        if(r % 2 == 1 && c == 1){
            if(leftright==righthand){

                return right_side;
            } else {

                return top_bot_side;
            }
            // printf("1");
        } else if (r % 2 == 0 && c==1){
            // printf("2");
            if(leftright==righthand){
                return top_bot_side;
            } else {
                return right_side;
            }
        } else if (c == map->cols && r % 2 == 1){
            // printf("5");
            if(leftright==right_side){
                return top_bot_side;
            } else {
                return left_side;
            }
        } else if (r % 2 == 0 && c == map->cols) {
            // printf("6");
            if(leftright==righthand){
                return left_side;
            } else {
                return top_bot_side;
            }
        } else if (r == 1){
            // printf("3");
            if(leftright==righthand){
                return left_side;
            } else {
                return right_side;
            }
        } else if (r == map->rows){
            // printf("4");
            if(leftright==righthand){
                return right_side;
            } else {
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
    if((triangle_has_bottom(pos_row,pos_col) && leftright == righthand) || (!triangle_has_bottom(pos_row,pos_col) && leftright == lefthand)) { //normalny
        // border=(border%3)+1;
        if(border==top_bot_side){
            border=right_side;
        } else if (border==right_side){
            border=top_bot_side;
        } else if (border==left_side){
            border=left_side;
        }
        return border;
    } else/* if((triangle_has_bottom(pos_row,pos_col) && leftright == lefthand) || (!triangle_has_bottom(pos_row,pos_col) && leftright == righthand)) */{ 
        if(border==top_bot_side){
            border=left_side;
        } else if (border==right_side){
            border=right_side;
        } else if (border==left_side){
            border=top_bot_side;
        }
        return border;
        } /*else {
            return 0; // zbytocne asi
        }*/
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