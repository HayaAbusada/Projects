#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    ifstream INPUTFILE(fileName); 
    if (!INPUTFILE.is_open()) { 
        return nullptr; 
    } 
    INPUTFILE >> maxRow;
    if(INPUTFILE.fail() || maxRow > 999999 || maxRow <= 0){
        return nullptr;
    }
    INPUTFILE >> maxCol;
    if(INPUTFILE.fail() || maxCol > 999999 || maxCol <= 0){
        return nullptr;
    }
    INPUTFILE >> player.row;
    if(INPUTFILE.fail() ||  player.row < 0 || player.row > 999999 || maxRow <= 0 || player.row > maxRow){
        return nullptr;
    }
    INPUTFILE >> player.col;
    if(INPUTFILE.fail() || player.col < 0 || player.col > 999999 || maxCol <= 0 || player.col > maxCol){
        return nullptr;
    }
    if(player.row >= maxRow || player.col >= maxCol){
        return nullptr;
    }
    if(maxRow >= INT32_MAX/maxCol || maxCol >= INT32_MAX/maxRow){
        return nullptr;
    }
    char** dungeonMap = createMap(maxRow,maxCol); 
    if (dungeonMap == nullptr) { 
        deleteMap(dungeonMap, maxRow);
        return nullptr; 
    } 
    int ColCount = 0;
    int RowCount = 0;
    for (int i = 0; i < maxRow; i++) { 
        RowCount ++;
        for (int j = 0; j < maxCol; j++) { 
            ColCount ++;
            INPUTFILE >> dungeonMap[i][j];
            if ( (dungeonMap[i][j] != '$') && (dungeonMap[i][j] != '@') && (dungeonMap[i][j] != 'M') && (dungeonMap[i][j] != '+') && (dungeonMap[i][j] != '-') && (dungeonMap[i][j] !='?') && (dungeonMap[i][j] != '!') ) {
                cout << "Invalid" << endl;
                return nullptr;
            }
            if(dungeonMap[i][j] != TILE_AMULET && dungeonMap[i][j] != TILE_DOOR && dungeonMap[i][j] != TILE_EXIT && dungeonMap[i][j] != TILE_MONSTER && dungeonMap[i][j] != TILE_OPEN && dungeonMap[i][j] != TILE_PILLAR && dungeonMap[i][j] != TILE_PLAYER && dungeonMap[i][j] != TILE_TREASURE){
                deleteMap(dungeonMap, maxRow);
                return nullptr;
            }
            if (i == player.row && j == player.col) { 
                dungeonMap[i][j] = TILE_PLAYER; 
            } 
        }
    }
    if(RowCount != maxRow || ColCount/RowCount != maxCol){
        deleteMap(dungeonMap, maxRow);
        return nullptr;
    }   
    bool Door = false;
    bool Exit = false;

    for(int i = 0; i < maxRow; i++)
    {
        for(int j = 0; j < maxCol; j++)
        {
            if(dungeonMap[i][j] == TILE_EXIT){
                Exit = true;
            } else if(dungeonMap[i][j] == TILE_DOOR){
                Door = true;
            }
        }
    }
    if(Exit == false && Door == false)
    {
        deleteMap(dungeonMap,maxRow);
        return nullptr;
    }
    INPUTFILE.close();
    return dungeonMap;
;
}


/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    switch (input) {
        case MOVE_UP:
            nextRow -= 1; 
            break;
        case MOVE_LEFT:
            nextCol -= 1;
            break;
        case MOVE_DOWN:
            nextRow += 1;  
            break;
        case MOVE_RIGHT:
            nextCol += 1; 
            break;
        default:
            break;
    }   
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    if (maxRow <= 0 || maxCol <= 0) {
        return nullptr;  
    }
    char** map = new char*[maxRow];
    for (int i = 0; i < maxRow; i++) {
        map[i] = new char[maxCol];
        for (int j = 0; j < maxCol; j++) {
            map[i][j] = TILE_OPEN; 
        }
    }
    return map; 
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    if (map != nullptr){
        for (int i = 0; i < maxRow; i++) {
            delete[] map[i];
        }
        delete[] map;
        map = nullptr;
    }
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    if (map == nullptr || maxRow <= 0 || maxCol <= 0) {
        return nullptr;
    }
    int TMaxRow = maxRow;
    int TMaxCol = maxCol;
    maxRow = maxRow * 2;
    maxCol = maxCol * 2;
    char** Tmap = map;
    map = createMap(maxRow, maxCol);
    for (int i = 0; i < TMaxRow; i++) {
        for (int j = 0; j < TMaxCol; j++){
            if (Tmap[i][j] == TILE_PLAYER){
                map[i][j] = TILE_PLAYER;
                map[i + TMaxRow][j] = TILE_OPEN;
                map[i][j + TMaxCol] = TILE_OPEN;
                map[i + TMaxRow][j + TMaxCol] = TILE_OPEN;
            }else{
                map[i][j] = Tmap[i][j];
                map[i + TMaxRow][j] = Tmap[i][j];
                map[i][j + TMaxCol] = Tmap[i][j];
                map[i + TMaxRow][j + TMaxCol] = Tmap[i][j];          
            }
        }
    }
    deleteMap(Tmap, TMaxRow);
    return map;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    if (nextRow < 0 || nextRow >= maxRow || nextCol < 0 || nextCol >= maxCol) {
        return STATUS_STAY;
    }
    char nextTile = map[nextRow][nextCol];
    switch (nextTile) {
        case TILE_PILLAR:
        case TILE_MONSTER:
            return STATUS_STAY; 
        
        case TILE_EXIT:
            if (player.treasure > 0) {
                map[player.row][player.col] = TILE_OPEN;
                map[nextRow][nextCol] = TILE_PLAYER;
                player.row = nextRow;
                player.col = nextCol;
                return STATUS_ESCAPE;
            } else {
                return STATUS_STAY; 
            }
        case TILE_DOOR:
            map[nextRow][nextCol] = TILE_PLAYER;
            map[player.row][player.col] = TILE_OPEN;
            player.row = nextRow;
            player.col = nextCol;
            return STATUS_LEAVE;

        case TILE_TREASURE:
            player.treasure+=1;
            map[nextRow][nextCol] = TILE_PLAYER;
            map[player.row][player.col] = TILE_OPEN;
            player.row = nextRow;
            player.col = nextCol;
            return STATUS_TREASURE;

        case TILE_AMULET:
            map[nextRow][nextCol] = TILE_PLAYER;
            map[player.row][player.col] = TILE_OPEN;
            player.row = nextRow;
            player.col = nextCol;
            return STATUS_AMULET;

        case TILE_OPEN:
            map[nextRow][nextCol] = TILE_PLAYER;
            map[player.row][player.col] = TILE_OPEN;
            player.row = nextRow;
            player.col = nextCol;
            return STATUS_MOVE;

        default:
            return STATUS_MOVE;
    }
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    for (int row = player.row - 1; row >= 0; row--) {
        if (map[row][player.col] == TILE_MONSTER) {
            map[row][player.col] = TILE_OPEN;
            map[row+1][player.col] = TILE_MONSTER;
        } else if (map[row][player.col] == TILE_PILLAR) {
            break; 
        }
    }
    for (int row = player.row + 1; row < maxRow; row++) {
        if (map[row][player.col] == TILE_MONSTER) {
            map[row][player.col] = TILE_OPEN;
            map[row-1][player.col] = TILE_MONSTER;
        } else if (map[row][player.col] == TILE_PILLAR) {
            break; 
        }
    }
    for (int col = player.col - 1; col >= 0; col--) {
        if (map[player.row][col] == TILE_MONSTER) {
            map[player.row][col] = TILE_OPEN;
            map[player.row][col+1] = TILE_MONSTER;
        } else if (map[player.row][col] == TILE_PILLAR) {
            break; 
        }
    }
    for (int col = player.col + 1; col < maxCol; col++) {
        if (map[player.row][col] == TILE_MONSTER) {
            map[player.row][col] = TILE_OPEN;
            map[player.row][col-1] = TILE_MONSTER;
        } else if (map[player.row][col] == TILE_PILLAR) {
            break; 
        }
    }

    if (map[player.row][player.col] == TILE_MONSTER){
        return true;
    } else{
        return false;
    }

}
