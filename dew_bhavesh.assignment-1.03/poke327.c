#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "heap.h"

#define MAP_WIDTH 80
#define MAP_HEIGHT 21
#define WORLD_WIDTH 401
#define WORLD_HEIGHT 401
#define START_X 200
#define START_Y 200

typedef enum {
    grass,
    clearing,
    boulder,
    border,
    tree,
    water,
    road,
    pokemart,
    pokecenter,
    gate,
    pc,
    hiker,
    rival,
} terrain_e;

typedef struct map {
    terrain_e terrain[MAP_HEIGHT][MAP_WIDTH];
    int n, s, w, e; 
} map_t;

typedef struct coordinate {
    int x;
    int y;
} coordinate_t;

typedef enum trainer_type {
    pc_train,
    hiker_train,
    rival_train,
    swimmer_train,
    other,
} trainer_type_e;

typedef struct terrain {
    terrain_e type;
    int cost;
} terrain_t;

typedef struct path {
    heap_node_t *heap_node;
    coordinate_t coordinate;
    terrain_t terrain;
    int cost; 
} path_t;

typedef struct trainer {
    trainer_type_e type;
    coordinate_t position;
} trainer_t;


void building_to_map(map_t *map, terrain_e building, int y, int x, int gate_row) {
    int building_connected = 0;

    map->terrain[y][x] = building;
    map->terrain[y + 1][x] = building;
    map->terrain[y][x + 1] = building;
    map->terrain[y + 1][x + 1] = building;

    map->terrain[y - 1][x] = road;
    map->terrain[y - 1][x + 1] = road;
    map->terrain[y - 1][x + 2] = road;
    map->terrain[y][x + 2] = road;
    map->terrain[y + 1][x + 2] = road;
    map->terrain[y + 2][x + 2] = road;
    map->terrain[y + 2][x + 1] = road;
    map->terrain[y + 2][x] = road;
    map->terrain[y + 2][x - 1] = road;
    map->terrain[y + 1][x - 1] = road;
    map->terrain[y][x - 1] = road;
    map->terrain[y - 1][x - 1] = road;

    if (y > gate_row + 1 && y > 3) { 
        y--;
        while (y > 1 && map->terrain[y - 1][x] != road) {
            y--;
            map->terrain[y][x] = road;
        }

        if (y != 1) {
            building_connected = 1;
        }
    } else if (y < gate_row - 2 && y < MAP_HEIGHT - 4) { 
        y += 2;
        while (y < MAP_HEIGHT - 2 && map->terrain[y + 1][x] != road) {
            y++;
            map->terrain[y][x] = road;
        }

        if (y != MAP_HEIGHT - 2) {
            building_connected = 1;
        }
    }

    
    if (building_connected == 0) {
        if (map->w > 0) { 
            while (x > 0 && map->terrain[y][x - 1] != road) {
                x--;
                map->terrain[y][x] = road;
            }

            if (x > 0) {
                building_connected = 1;
            }
        } else { 
            while (x < MAP_WIDTH - 1 && map->terrain[y][x + 1] != road) {
                x++;
                map->terrain[y][x] = road;
            }

            if (x < MAP_WIDTH - 1) {
                building_connected = 1;
            }
        }

        if (map->n > 0 && building_connected == 0) {
            while (y > 1 && map->terrain[y - 1][x] != road) {
                y--;
                map->terrain[y][x] = road;
            }
        } else if (building_connected == 0) { 
            while (y < MAP_HEIGHT - 1 && map->terrain[y + 1][x] != road) {
                y++;
                map->terrain[y][x] = road;
            }
        }
    }
}

void place_buildings(map_t *map, int manhattan_distance) {
    terrain_e first_building, second_building;
    int i, y, x;

    i = rand() % 2;

    first_building = (i < 1) ? pokemart : pokecenter;
    second_building = (first_building == pokemart) ? pokecenter : pokemart;

    int building_probability = ((-45 * manhattan_distance) / 200 + 50);

    if (manhattan_distance == 0 || rand() % 100 < building_probability) {
        y = rand() % 16 + 2; 
        x = rand() % 36 + 2; 

        building_to_map(map, first_building, y, x, map->w);
    }

    if (manhattan_distance == 0 || rand() % 100 < building_probability) {
        y = rand() % 16 + 2; 
        x = rand() % 36 + 41; 

        building_to_map(map, second_building, y, x, map->e);
    }
}

void pave_roads(map_t *map)
{
    int x_run = map->s - map->n;
    int row, col;

    row = 1;
    col = map->n;

    
    if (map->n > 0 && map->s > 0) {
        while (row < (MAP_HEIGHT - 2) / 2) {
            map->terrain[row][col] = road;
            row++;
        }

        if (x_run < 0) { 
            while (col > map->s) {
                map->terrain[row][col] = road;
                col--;
            }
        } else if (x_run > 0) { 
            while (col < map->s) {
                map->terrain[row][col] = road;
                col++;
            }
        }

        while (row < MAP_HEIGHT - 1) {
            map->terrain[row][col] = road;
            row++;
        }
    }

    int y_rise = map->e - map->w;

    col = 1;
    row = map->w;

    if (map->w > 0 && map->e > 0) {
        while (col < (MAP_WIDTH - 2) / 2) { 
            map->terrain[row][col] = road;
            col++;
        }

        if (y_rise < 0) { 
            while (row > map->e) {
                map->terrain[row][col] = road;
                row--;
            }
        } else if (y_rise > 0) { 
            while (row < map->e) {
                map->terrain[row][col] = road;
                row++;
            }
        }

        while (col < MAP_WIDTH - 1) { 
            map->terrain[row][col] = road;
            col++;
        }
    }
}

void pave_roads_edge(map_t *map)
{
    int row, col;

    if (map->n < 0) {
        col = map->s;

        if (map->w < 0) { 
            for (row = MAP_HEIGHT - 2; row > map->e; row--) {
                map->terrain[row][col] = road;
            }

            for (col = map->s; col < MAP_WIDTH - 1; col++) {
                map->terrain[row][col] = road;
            }
        } else if (map->e < 0) { 
            for (row = MAP_HEIGHT - 2; row > map->w; row--) {
                map->terrain[row][col] = road;
            }

            for (col = map->s; col > 0; col--) {
                map->terrain[row][col] = road;
            }
        } else { 
            pave_roads(map); 

            row = MAP_HEIGHT - 2;
            while (map->terrain[row][col] != road && row > 0) {
                map->terrain[row][col] = road;
                row--;
            }
        }
    } else if (map->s < 0) {
        col = map->n;

        if (map->e < 0) { 
            for (row = 0; row < map->w; row++) {
                map->terrain[row][col] = road;
            }

            for (col = map->n; col > 0; col--) {
                map->terrain[row][col] = road;
            }
        } else if (map->w < 0) { 
            for (row = 0; row < map->e; row++) {
                map->terrain[row][col] = road;
            }

            for (col = map->n; col < MAP_WIDTH - 1; col++) {
                map->terrain[row][col] = road;
            }
        } else { 
            pave_roads(map); 

            row = 0;
            while (map->terrain[row][col] != road && row < MAP_HEIGHT - 1) {
                map->terrain[row][col] = road;
                row++;
            }
        }
    } else if (map->w < 0) { 
        pave_roads(map);

        row = map->e;
        col = MAP_WIDTH - 2;

        while (map->terrain[row][col] != road && col > 0) {
            map->terrain[row][col] = road;
            col--;
        }
    } else if (map->e < 0) { 
        pave_roads(map); 

        row = map->w;
        col = 1;

      
        while (map->terrain[row][col] != road && col < MAP_WIDTH - 1) {
            map->terrain[row][col] = road;
            col++;
        }
    }
}


void generate_regional_terrain(map_t *map, int west_bound, int east_bound)
{
    int x, y, region_width, region_height;
    int i, row, col;
    terrain_e regions[5] = {boulder, tree, water, grass, grass};

    for (i = 0; i < 5; i++) {
   
        x = rand() % (east_bound - west_bound);
        x += west_bound;
        y = rand() % (MAP_HEIGHT - 3) + 2;

      
        region_width = rand() % 30 + 10;
        region_height = rand() % 6 + 5;

        for (row = 0; row < region_height; row++) {
            if (y + row < MAP_HEIGHT - 1) { 
                for (col = 0; col < region_width; col++) {
                    if (x + col < MAP_WIDTH - 1) {
                        map->terrain[y + row][x + col] = regions[i];
                    } else { 
                        map->terrain[y + row][x - col] = regions[i];
                    }
                }
            } else { 
                for (col = 0; col < region_width; col++) {
                    if (x + col < MAP_WIDTH - 1) { 
                        map->terrain[y - row][x + col] = regions[i];
                    } else { 
                        map->terrain[y - row][x - col] = regions[i];
                    }
                }
            }
        }
    }
}


void generate_terrain(map_t *map)
{
    generate_regional_terrain(map, 1, MAP_WIDTH / 2 - 1); 
    generate_regional_terrain(map, MAP_WIDTH / 2 - 1, MAP_WIDTH - 1); 

   
    int i, row, col;

    for (i = 0; i < 20; i++) {
        row = rand() % (MAP_HEIGHT - 2) + 1;
        col = rand() % (MAP_WIDTH - 2) + 1;

        map->terrain[row][col] = border;

        row = rand() % (MAP_HEIGHT - 2) + 1;
        col = rand() % (MAP_WIDTH - 2) + 1;

        map->terrain[row][col] = tree;
    }
}


void construct_border(map_t *map, int n, int s, int w, int e)
{
    
    int i, j;

    for (i = 0; i < MAP_HEIGHT; i++) {
        map->terrain[i][0] = border;
        map->terrain[i][MAP_WIDTH - 1] = border;
    }

    for (j = 0; j < MAP_WIDTH; j++) {
        map->terrain[0][j] = border;
        map->terrain[MAP_HEIGHT - 1][j] = border;
    }

   
    map->n = (n == 0) ? rand() % (MAP_WIDTH - 2) + 1 : n; 
    map->s = (s == 0) ? rand() % (MAP_WIDTH - 2) + 1 : s;
    map->w = (w == 0) ? rand() % (MAP_HEIGHT - 2) + 1 : w;
    map->e = (e == 0) ? rand() % (MAP_HEIGHT - 2) + 1 : e;

 
    map->terrain[0][map->n] = (n == -1) ? border : gate;
    map->terrain[MAP_HEIGHT - 1][map->s] = (s == -1) ? border : gate;
    map->terrain[map->w][0] = (w == -1) ? border : gate;
    map->terrain[map->e][MAP_WIDTH - 1] = (e == -1) ? border : gate;
}

void generate_map(map_t *map, int n, int s, int w, int e, int manhattan_distance)
{
    construct_border(map, n, s, w, e);

    generate_terrain(map);

    if (n < 0 || s < 0 || w < 0 || e < 0) { 
        pave_roads_edge(map);
    } else {
        pave_roads(map);
    }

    place_buildings(map, manhattan_distance);
}

void map_init(map_t *map)
{
    int x, y;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            map->terrain[y][x] = grass;
        }
    }

    map->n = 0;
    map->s = 0;
    map->w = 0;
    map->e = 0;
}

int get_terrain_cost(terrain_e terrain_type, trainer_type_e trainer_type) {
    int terrain_cost = INT_MAX;

    if (terrain_type == pc) {
        terrain_cost = 0;
    } else if (terrain_type == road) {
        if (trainer_type != swimmer_train) {
            terrain_cost = 10;
        }
    } else if (terrain_type == pokemart || terrain_type == pokecenter) {
        if (trainer_type == pc_train) {
            terrain_cost = 10;
        } else if (trainer_type != swimmer_train) {
            terrain_cost = 50;
        }
    } else if (terrain_type == grass) {
        if (trainer_type == hiker_train) {
            terrain_cost = 15;
        } else if (trainer_type != swimmer_train) {
            terrain_cost = 20;
        }
    } else if (terrain_type == clearing) {
        if (trainer_type != swimmer_train) {
            terrain_cost = 10;
        }
    } else if (terrain_type == boulder || terrain_type == tree) {
        if (trainer_type == hiker_train) {
            terrain_cost = 15;
        }
    } else if (terrain_type == water) {
        if (trainer_type == swimmer_train) {
            terrain_cost = 7;
        }
    } else if (terrain_type == gate) {
        if (trainer_type == pc_train) {
            terrain_cost = 10;
        }
    }

    return terrain_cost;
}

void dijkstra_path(heap_t *heap, map_t *map, path_t path[MAP_HEIGHT][MAP_WIDTH], trainer_type_e trainer_type, coordinate_t to)
{
    if (trainer_type != hiker_train && trainer_type != rival_train) {
        return;
    }

    path_t *p;

    int x, y;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            path[y][x].coordinate.y = y;
            path[y][x].coordinate.x = x;
        }
    }

    for (y = 0; y < MAP_HEIGHT; y++) { 
        for (x = 0; x < MAP_WIDTH; x++) {
            path[y][x].cost = INT_MAX;
        }
    }

    int terrain_cost;
    for (y = 0; y < MAP_HEIGHT; y++) { 
        for (x = 0; x < MAP_WIDTH; x++) {
            path[y][x].terrain.type = map->terrain[y][x];

            terrain_cost = get_terrain_cost(path[y][x].terrain.type, trainer_type);

            if (terrain_cost != INT_MAX) {
                if (terrain_cost == 0) {
                    path[y][x].cost = terrain_cost; 
                }
                path[y][x].heap_node = heap_insert(heap, &path[y][x]);
            } else {
                path[y][x].heap_node = NULL;
            }

            path[y][x].terrain.cost = terrain_cost;
        }
    }

    while ((p = heap_remove_min(heap))) { 
        p->heap_node = NULL;

        if ((path[p->coordinate.y - 1][p->coordinate.x - 1].heap_node) &&
            (path[p->coordinate.y - 1][p->coordinate.x - 1].cost >
             (p->cost + path[p->coordinate.y - 1][p->coordinate.x - 1].terrain.cost) &&
              p->cost + path[p->coordinate.y - 1][p->coordinate.x - 1].terrain.cost > 0)) {
            path[p->coordinate.y - 1][p->coordinate.x - 1].cost =
                    (p->cost + path[p->coordinate.y - 1][p->coordinate.x - 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y - 1][p->coordinate.x - 1].heap_node);
        }
        if ((path[p->coordinate.y - 1][p->coordinate.x    ].heap_node) &&
            (path[p->coordinate.y - 1][p->coordinate.x    ].cost >
             (p->cost + path[p->coordinate.y - 1][p->coordinate.x].terrain.cost) &&
              p->cost + path[p->coordinate.y - 1][p->coordinate.x].terrain.cost > 0)) {
            path[p->coordinate.y - 1][p->coordinate.x    ].cost =
                    (p->cost + path[p->coordinate.y - 1][p->coordinate.x    ].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y - 1][p->coordinate.x    ].heap_node);
        }
        if ((path[p->coordinate.y - 1][p->coordinate.x + 1].heap_node) &&
            (path[p->coordinate.y - 1][p->coordinate.x + 1].cost >
             (p->cost + path[p->coordinate.y - 1][p->coordinate.x + 1].terrain.cost) &&
              p->cost + path[p->coordinate.y - 1][p->coordinate.x + 1].terrain.cost > 0)) {
            path[p->coordinate.y - 1][p->coordinate.x + 1].cost =
                    (p->cost + path[p->coordinate.y - 1][p->coordinate.x + 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y - 1][p->coordinate.x + 1].heap_node);
        }
        if ((path[p->coordinate.y    ][p->coordinate.x - 1].heap_node) &&
            (path[p->coordinate.y    ][p->coordinate.x - 1].cost >
             (p->cost + path[p->coordinate.y    ][p->coordinate.x - 1].terrain.cost) &&
              p->cost + path[p->coordinate.y    ][p->coordinate.x - 1].terrain.cost > 0)) {
            path[p->coordinate.y    ][p->coordinate.x - 1].cost =
                    (p->cost + path[p->coordinate.y    ][p->coordinate.x - 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y    ][p->coordinate.x - 1].heap_node);
        }
        if ((path[p->coordinate.y    ][p->coordinate.x + 1].heap_node) &&
            (path[p->coordinate.y    ][p->coordinate.x + 1].cost >
             (p->cost + path[p->coordinate.y    ][p->coordinate.x + 1].terrain.cost) &&
              p->cost + path[p->coordinate.y    ][p->coordinate.x + 1].terrain.cost > 0)) {
            path[p->coordinate.y    ][p->coordinate.x + 1].cost =
                    (p->cost + path[p->coordinate.y    ][p->coordinate.x + 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y    ][p->coordinate.x + 1].heap_node);
        }
        if ((path[p->coordinate.y + 1][p->coordinate.x - 1].heap_node) &&
            (path[p->coordinate.y + 1][p->coordinate.x - 1].cost >
             (p->cost + path[p->coordinate.y + 1][p->coordinate.x - 1].terrain.cost) &&
              p->cost + path[p->coordinate.y + 1][p->coordinate.x - 1].terrain.cost > 0)) {
            path[p->coordinate.y + 1][p->coordinate.x - 1].cost =
                    (p->cost + path[p->coordinate.y + 1][p->coordinate.x - 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y + 1][p->coordinate.x - 1].heap_node);
        }
        if ((path[p->coordinate.y + 1][p->coordinate.x    ].heap_node) &&
            (path[p->coordinate.y + 1][p->coordinate.x    ].cost >
             (p->cost + path[p->coordinate.y + 1][p->coordinate.x    ].terrain.cost) &&
              p->cost + path[p->coordinate.y + 1][p->coordinate.x    ].terrain.cost > 0)) {
            path[p->coordinate.y + 1][p->coordinate.x    ].cost =
                    (p->cost + path[p->coordinate.y + 1][p->coordinate.x    ].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y + 1][p->coordinate.x    ].heap_node);
        }
        if ((path[p->coordinate.y + 1][p->coordinate.x + 1].heap_node) &&
            (path[p->coordinate.y + 1][p->coordinate.x + 1].cost >
             (p->cost + path[p->coordinate.y + 1][p->coordinate.x + 1].terrain.cost) &&
              p->cost + path[p->coordinate.y + 1][p->coordinate.x + 1].terrain.cost > 0)) {
            path[p->coordinate.y + 1][p->coordinate.x + 1].cost =
                    (p->cost + path[p->coordinate.y + 1][p->coordinate.x + 1].terrain.cost);
            heap_decrease_key_no_replace(heap, path[p->coordinate.y + 1][p->coordinate.x + 1].heap_node);
        }
    }
}

typedef struct world {
    map_t *board[WORLD_HEIGHT][WORLD_WIDTH];
    map_t *current_map;
    coordinate_t location;

    path_t hiker_path[MAP_HEIGHT][MAP_WIDTH];
    path_t rival_path[MAP_HEIGHT][MAP_WIDTH];

    char view[MAP_HEIGHT][MAP_WIDTH + 1];
} world_t;

static int32_t path_cmp(const void *key, const void *with) { 
    return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

void print_view(map_t *map, char view[MAP_HEIGHT][MAP_WIDTH + 1])
{
    int y, x;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            switch (map->terrain[y][x])
            {
                case grass:
                    view[y][x] = ':';
                    break;
                case clearing:
                    view[y][x] = '.';
                    break;
                case boulder:
                    view[y][x] = '%';
                    break;
                case border:
                    view[y][x] = '%';
                    break;
                case tree:
                    view[y][x] = '^';
                    break;
                case water:
                    view[y][x] = '~';
                    break;
                case road:
                    view[y][x] = '#';
                    break;
                case gate:
                    view[y][x] = '#';
                    break;
                case pokemart:
                    view[y][x] = 'M';
                    break;
                case pokecenter:
                    view[y][x] = 'C';
                    break;
                case pc:
                    view[y][x] = '@';
                    break;
                case hiker:
                    view[y][x] = 'h';
                    break;
                case rival:
                    view[y][x] = 'r';
                    break;
                default:
                    view[y][x] = '!'; 
                    break;
            }
        }

        if (y < MAP_HEIGHT - 1) {
            view[y][MAP_WIDTH] = '\n';
        }
    }

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH + 1; x++) {
            printf("%c", view[y][x]);
        }
    }

    printf("\n");
}

/* According to 1.03, print out the Dijkstra's path map. */
void print_path_map(path_t path[MAP_HEIGHT][MAP_WIDTH])
{
    int y, x;
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH - 1; x++) {
            if (path[y][x].cost == INT_MAX) {
                printf("   ");
            } else {
                printf("%02d ", path[y][x].cost % 100);
            }
        }
        if (path[y][MAP_WIDTH - 1].cost == INT_MAX) {
            printf("  \n");
        } else {
            printf("%02d\n", path[y][MAP_WIDTH - 1].cost % 100);
        }
    }
    printf("\n");
}

/* Check if trainer can go to location based on terrain and if it is occupied or not. Return 0 if trainer can be at location. */
int check_trainer_position(trainer_type_e type, terrain_e terrain)
{
    int check = 1;

    if (terrain == border ||
        terrain == pc ||
        terrain == hiker ||
        terrain == rival) {
        check = 0;
    } else if ((type == swimmer_train && terrain != water) || (terrain == water && type != swimmer_train)) {
        check = 0;
    } else if (terrain == boulder || terrain == tree) {
        if (type != hiker_train) {
            check = 0;
        }
    } else if (terrain == gate && type != pc_train) {
        check = 0;
    }

    return check;
}

/* Add the PC to a road in this map. */
void place_pc(map_t *map, trainer_t *pc_trainer)
{
    int y, x, count;
    count = 0;

    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain[y][x] == road) {
                count++;
            }
        }
    }

  
    coordinate_t pc_start_options[count];

    count = 0;
    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain[y][x] == road) {
                pc_start_options[count].x = x;
                pc_start_options[count].y = y;
                count++;
            }
        }
    }

    pc_trainer->position = pc_start_options[rand() % count];
    map->terrain[pc_trainer->position.y][pc_trainer->position.x] = pc;
}

void place_trainer(trainer_t trainer, terrain_e terrain[MAP_HEIGHT][MAP_WIDTH]) {
    int valid_character_position;
    trainer.position.x = (rand() % MAP_WIDTH - 2) + 1;
    trainer.position.y = (rand() % MAP_HEIGHT - 2) + 1;

    valid_character_position = check_trainer_position(trainer.type,
                                                      terrain[trainer.position.y][trainer.position.x]
    );
    while(!valid_character_position) {
        trainer.position.x = (rand() % MAP_WIDTH - 2) + 1;
        trainer.position.y = (rand() % MAP_HEIGHT - 2) + 1;
        valid_character_position = check_trainer_position(trainer.type,
                                                          terrain[trainer.position.y][trainer.position.x]
        );
    }

    switch (trainer.type) {
        case hiker_train:
            terrain[trainer.position.x][trainer.position.y] = hiker;
            break;
        case rival_train:
            terrain[trainer.position.x][trainer.position.y] = rival;
            break;
        default:
            break;
    }
}

void place_gates(world_t *world)
{
    if (world->location.y == 0) { 
        world->current_map->n = -1;
    } else if (world->location.y == WORLD_HEIGHT - 1) {
        world->current_map->s = -1;
    }

    if (world->location.x == 0) { 
        world->current_map->w = -1;
    } else if (world->location.x == WORLD_WIDTH - 1) { 
        world->current_map->e = -1;
    }

    if (world->location.y > 0 && world->board[world->location.y - 1][world->location.x] != NULL) {
        world->current_map->n = world->board[world->location.y - 1][world->location.x]->s;
    }

    
    if (world->location.y < WORLD_HEIGHT - 1 && world->board[world->location.y + 1][world->location.x] != NULL) {
        world->current_map->s = world->board[world->location.y + 1][world->location.x]->n;
    }

   
    if (world->location.x > 0 && world->board[world->location.y][world->location.x - 1] != NULL) {
        world->current_map->w = world->board[world->location.y][world->location.x - 1]->e;
    }

  
    if (world->location.x < WORLD_WIDTH - 1 && world->board[world->location.y][world->location.x + 1] != NULL) {
        world->current_map->e = world->board[world->location.y][world->location.x + 1]->w;
    }
}

void trainer_init(map_t *map, trainer_t *pc_trainer, trainer_t *hiker_trainer, trainer_t *rival_trainer)
{
    pc_trainer->type = pc_train;
    pc_trainer->position.x = -1;
    pc_trainer->position.y = -1;
    place_pc(map, pc_trainer);

    hiker_trainer->type = hiker_train;
    hiker_trainer->position.x = -1;
    hiker_trainer->position.y = -1;

    rival_trainer->type = rival_train;
    rival_trainer->position.x = -1;
    rival_trainer->position.y = -1;
}

void world_init(world_t *world)
{
    int x, y;
    for (y = 0; y < WORLD_HEIGHT; y++) {
        for (x = 0; x < WORLD_WIDTH; x++) {
            world->board[y][x] = NULL;
        }
    }

    world->current_map = malloc(sizeof (*world->current_map));

    world->location.x = START_X;
    world->location.y = START_Y;

    world->board[START_Y][START_X] = malloc(sizeof (*world->current_map));
    world->current_map = world->board[START_Y][START_X];
    map_init(world->current_map);

    generate_map(world->current_map, world->current_map->n, world->current_map->s, world->current_map->w, world->current_map->e, 0);

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            world->hiker_path[y][x].heap_node = NULL;
            world->hiker_path[y][x].coordinate.x = x;
            world->hiker_path[y][x].coordinate.y = y;
            world->hiker_path[y][x].terrain.type = border;
            world->hiker_path[y][x].terrain.cost = INT_MAX;
            world->hiker_path[y][x].cost = INT_MAX;

            world->rival_path[y][x].heap_node = NULL;
            world->rival_path[y][x].coordinate.x = x;
            world->rival_path[y][x].coordinate.y = y;
            world->rival_path[y][x].terrain.type = border;
            world->rival_path[y][x].terrain.cost = INT_MAX;
            world->rival_path[y][x].cost = INT_MAX;
        }
    }

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            world->view[y][x] = '!';
        }

        world->view[y][MAP_WIDTH] = '\n';
    }
}

void world_delete(world_t *world)
{
    int x, y;
   
    for (y = 0; y < WORLD_HEIGHT; y++) {
        for (x = 0; x < WORLD_WIDTH; x++) {
            if (world->board[y][x] != NULL) {
                free(world->board[y][x]);
            }
        }
    }
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char *argv[])
{
    srand(time(NULL));

    world_t *world;
    world = malloc(sizeof (*world));
    world_init(world);

    trainer_t pc_trainer, hiker_trainer, rival_trainer;
    trainer_init(world->current_map, &pc_trainer, &hiker_trainer, &rival_trainer);

    heap_t heap;
    heap_init(&heap, path_cmp, NULL);

    print_view(world->current_map, world->view);

    dijkstra_path(&heap, world->current_map, world->hiker_path, hiker_trainer.type, pc_trainer.position);
    dijkstra_path(&heap, world->current_map, world->rival_path, rival_trainer.type, pc_trainer.position);

    print_path_map(world->hiker_path);
    print_path_map(world->rival_path);
    printf("(%d, %d)\n", world->location.x - START_X, START_Y - world->location.y);

    char input = '\0';
    int fly_x, fly_y, manhattan_distance;

    while (input != 'q') {
        scanf(" %c", &input);

        if (input == 'n' && world->location.y > 0) {
            world->location.y--;
        } else if (input == 's' && world->location.y < WORLD_HEIGHT - 1) {
            world->location.y++;
        } else if (input == 'w' && world->location.x > 0) {
            world->location.x--;
        } else if (input == 'e' && world->location.x < WORLD_WIDTH - 1) {
            world->location.x++;
        } else if (input == 'f') {
            scanf(" %d %d", &fly_x, &fly_y);

            if (fly_x >= -1 * WORLD_WIDTH / 2 && fly_x <= WORLD_WIDTH / 2) {
                world->location.x = START_X + fly_x;
            } else {
                printf("Please enter a valid x-coordinate: [-200][200]\n");
                continue;
            }

            if (fly_y >= -1 * WORLD_HEIGHT / 2 && fly_y <= WORLD_HEIGHT / 2) {
                world->location.y = START_Y - fly_y;
            } else {
                printf("Please enter a valid y-coordinate: [-200][200]\n");
                continue;
            }
        } else if (input == 'q') {
            heap_delete(&heap);

            world_delete(world);
            free(world);

            break;
        } else {
            printf("Please enter a valid command: n, s, w, e, f x y, or q\n");
            continue;
        }

        if (world->board[world->location.y][world->location.x] == NULL) {
            world->board[world->location.y][world->location.x] = malloc(sizeof (*world->current_map));
            world->current_map = world->board[world->location.y][world->location.x];
            map_init(world->current_map);

            place_gates(world);

            manhattan_distance = abs(world->location.x - START_X) + abs(world->location.y - START_Y);

            generate_map(world->current_map, world->current_map->n, world->current_map->s, world->current_map->w, world->current_map->e, manhattan_distance);
        }

        world->current_map = world->board[world->location.y][world->location.x];

        place_pc(world->current_map, &pc_trainer);

        print_view(world->current_map, world->view);

        dijkstra_path(&heap, world->current_map, world->hiker_path, hiker_trainer.type, pc_trainer.position);
        dijkstra_path(&heap, world->current_map, world->rival_path, rival_trainer.type, pc_trainer.position);

        print_path_map(world->hiker_path);
        print_path_map(world->rival_path);
        printf("(%d, %d)\n", world->location.x - START_X, START_Y - world->location.y);
    }

    return 0;
}