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
void map_init(Map *map, int rows, int cols, int *cells_values);
void map_dest(Map *map);
int read_file(char *filename,int *rows, int *cols, int *cells_values, int *border);
bool isborder(Map *map, int rows, int cols, int border);
int check_file_cells(int index, int rows, int cols);
int check_borders(Map *map);
int can_start(Map *map, int rows, int cols);
int start_border(Map *map, int rows, int cols, int leftright);
int finished(Map *map, int row, int col);
int triangle_has_bottom(int rows, int col);
int tirangle_up_down_changed(int row, int col);
int next_border(int border, int pos_row, int pos_col, int leftright);
void move_position(int *pos_row, int *pos_col, int border);
void find_path(Map *map, int start_row, int start_col, int start_border_side, int leftright);

/// @brief Function takes command-line arguments and perfroms actions based on them.
/// @param argc Number of command-line arguments.
/// @param argv Array of command-line arguments.
/// @return Returns 0 on successfull execution. Otherwise returns 1.
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
/// @brief Function checks if argument is valid number greater than 0.
/// @param value The input argument as character.
/// @return Returns parsed integer value. Otherwise returns -1.
int parse_args(char *value){
    char *endptr;
    long long_value = strtol(value,&endptr,0);
    if(*endptr != '\0' || value == endptr || !(long_value >= 0)){
        return -1;
    } else {
        return (int) long_value;
    }
}
/// @brief Prints information about the usage of the program.
void print_help(){
    printf("Use --test file.txt to test the map.\n");
    printf("Use --rpath R C file.txt for searching with right hand rule.\n");
    printf("Use --lpath R C file.txt for searching with left hand rule.\n");
    printf("R stands for number of rows and C stands for number of columns.\n");
}
/// @brief Allocates memory for 2D map.
/// @param map Pointer to Map structure.
/// @param rows Number of rows for the map.
/// @param cols Number of columns for the map.
/// @return Returns 1 on successfull allocation. Otherwise returns 0;
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
/// @brief Initializes a Map structure with given dimensions and cell values.
/// @param map Pointer to the Map structure.
/// @param rows Number of rows for the map.
/// @param cols Number of columns for the map.
/// @param cells_values Array containing values of cells.
void map_init(Map *map,int rows, int cols,int *cells_values){
    map->rows=rows;
    map->cols=cols;
    for(int i=0; i < rows*cols;i++){
        map->cells[i]=(unsigned char)cells_values[i];
    }
}
/// @brief Deallocates memory and sets dimenstions to 0.
/// @param map Pointer to Map structure.
void map_dest(Map *map){
    free(map->cells);
    map->cols=0;
    map->rows=0;
}
/// @brief Reads data from file and saves them to an array.
/// @param filename Name of the file.
/// @param rows Pointer to store the number of rows read from the file.
/// @param cols Pointer to store the number of columns read from the file.
/// @param cells_values Array to store the cell values.
/// @param index Pointer to an index to keep track of number of cells.
/// @return Returns 1 if file was read successfully. Otherwise returns 0.
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
/// @brief Checks specific border of cell.
/// @param map Pointer to Map structure.
/// @param rows Row index of cell.
/// @param cols Column index of cell.
/// @param border Specifies which border to check.
/// @return Retruns 1 if there is a border. Otherwise returns 0.
bool isborder(Map *map, int rows, int cols, int border){
    if(border==top_bot_side){
        if(map->cells[(rows*map->cols)+cols] & 4){
            return 1;
        }
    } else if (border==right_side){
        if(map->cells[(rows*map->cols)+cols] & 2){
            return 1;
        } 
    } else if(border==left_side){
        if(map->cells[(rows*map->cols)+cols] & 1){
            return 1;
        }
    }
    return 0;
}
/// @brief Checks if number of cells read from file matches the expected count of cells.
/// @param index Total number of cells read from file.
/// @param rows Specified number of rows for the map.
/// @param cols Specified number of columns for the map.
/// @return Returns 1 if total number of cells from file matches the expected count and the values are valid. Otherwise returns 0.
int check_file_cells(int index,int rows, int cols){
    if(rows < 0 || cols < 0 || index != rows*cols){
        return 0;
    } else {
        return 1;
    }
}
/// @brief Checks if the borders of neighouring cells match.
/// @param map Pointer to the Map structure.
/// @return Returns 1 if borders are valid. Otherwise returns 0.
int check_borders(Map *map){
    for(int rows_index=0;rows_index<map->rows;rows_index++){
        for(int column_index=0;column_index<map->cols-1;column_index++){
            if(!(isborder(map,rows_index,column_index,right_side) == isborder(map,rows_index,column_index+1,left_side))){
                return 0;
            }
            if(rows_index < map->rows-1 && (rows_index+column_index) % 2 != 0){
                if(!(isborder(map,rows_index,column_index,top_bot_side) == isborder(map,rows_index+1,column_index,top_bot_side))){
                    return 0;
                } 
            }
        }
    }
    return 1;
}
/// @brief Checks if program can start at specified place.
/// @param map Pointer to the Map structure.
/// @param rows Row index of cell.
/// @param cols Column index of cell.
/// @return Returns 1 if the place is valid. Otherwise returns 0.
int can_start(Map *map,int rows, int cols){
    if(cols == 1 && !isborder(map, rows-1,cols-1,left_side)){
        return 1;
    } else if (cols == map->cols && !isborder(map, rows-1,cols-1,right_side)){
        return 1;
    } else if ((rows == 1 || rows == map->rows) && cols % 2 != 0 && !isborder(map, rows-1,cols-1,top_bot_side)){
        return 1;
    } else {
        return 0;
    }
}
/// @brief Determines the starting border of specified cell according to the assignment.
/// @param map Pointer to the Map structure.
/// @param rows Rows index of cell.
/// @param cols Column index of cell.
/// @param leftright Specifies which hand to use for solving of the Map.
/// @return Returns the starting border. 
int start_border(Map *map, int rows, int cols, int leftright){
        if(rows % 2 == 1 && cols == 1){
            // Case 1
            if(leftright==righthand){

                return right_side;
            } else {

                return top_bot_side;
            }
        } else if (rows % 2 == 0 && cols==1){
            // Case 2
            if(leftright==righthand){
                return top_bot_side;
            } else {
                return right_side;
            }
        } else if (cols == map->cols && rows % 2 == 1){
            // Case 5
            if(leftright==right_side){
                return top_bot_side;
            } else {
                return left_side;
            }
        } else if (rows % 2 == 0 && cols == map->cols) {
            // Case 6
            if(leftright==righthand){
                return left_side;
            } else {
                return top_bot_side;
            }
        } else if (rows == 1){
            // Case 3
            if(leftright==righthand){
                return left_side;
            } else {
                return right_side;
            }
        } else if (rows == map->rows){
            // Case 4
            if(leftright==righthand){
                return right_side;
            } else {
                return left_side;
            }
        }
        return 0;
}
/// @brief Checks is specified cell is outside of the Map.
/// @param map Pointer to the Map structure.
/// @param row Rows index of cell.
/// @param col Column index of cell.
/// @return Returns 1 if the cell is ouside of the Map. Otherwise returns 0.
int finished(Map *map, int row, int col){
    if(row > map->rows || row < 1 || col > map->cols || col < 1){
        return 1;
    } else {
        return 0;
    }
}
/// @brief Checks the cell orientation.
/// @param row Row index of cell.
/// @param col Column index of cell.
/// @return Returns 1 if the cell has bottom side. Otherwise returns 0. (It has top side)
int triangle_has_bottom(int row,int col){
    if((col % 2 == 1 && row % 2 == 0) || (col % 2 == 0 && row % 2 == 1)){
        return 1;
    } else {
        return 0;
    }
}
/// @brief Changes which border to check first for the next cell.
/// @param border The current border.
/// @param pos_row Current row index.
/// @param pos_col Current column index.
/// @param leftright Specifies which hand is used to solve the Map.
/// @return Returns the modified border.
int triangle_up_down_changed(int border, int pos_row, int pos_col, int leftright){
    if((triangle_has_bottom(pos_row,pos_col) && leftright == righthand) || (!triangle_has_bottom(pos_row,pos_col) && leftright == lefthand)) { // Bottom side triangle
        if(border==top_bot_side){
            border=right_side;
        } else if (border==right_side){
            border=top_bot_side;
        } else if (border==left_side){
            border=left_side;
        }
        return border;
    } else { // Top side triangle 
        if(border==top_bot_side){
            border=left_side;
        } else if (border==right_side){
            border=right_side;
        } else if (border==left_side){
            border=top_bot_side;
        }
        return border;
        }
}
/// @brief Changes the next border according to triangle orientation and hand used to solve the Map.
/// @param border The current border.
/// @param pos_row Row index of triangle.
/// @param pos_col Column index of triangle.
/// @param leftright Specifies which hand is used to solve the Map.
/// @return Returns the modified border.
int next_border(int border, int pos_row, int pos_col, int leftright){
    if(leftright==righthand){
        if(triangle_has_bottom(pos_row,pos_col)) { // Bottom side triangle
            if(border==right_side){
                return left_side;
            }
            border=((border%3)+2);
            return border;
        } else { // Top side triangle
            border=(border%3)+1;
            return border;
        }
    } else { // Lefthand
        if(triangle_has_bottom(pos_row,pos_col)) { // Bottom side trianagle
            border=(border%3)+1;
            return border;
        } else { // Top side triangle
            if(border==right_side){
                return left_side;
            }
            border=((border%3)+2);
            return border;
        }
    }
}
/// @brief Moves the position based on the border.
/// @param pos_row Pointer to the row index of the current position.
/// @param pos_col Pointer to the column index of the current position.
/// @param border Border indicating the direction of the movement.
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
/// @brief Finds and prints the path to solve the Map.
/// @param map Pointer to the Map structure.
/// @param start_row Starting row index.
/// @param start_col Starting column index.
/// @param start_border_side Starting border.
/// @param leftright Specifies which hand is used to solve the Map.
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