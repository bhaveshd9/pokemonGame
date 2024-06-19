#include <string>
#include <cstring>
#include <ncurses.h>
#include <climits>
#include <iostream>
#include "heap.h"

#define MAP_WIDTH 80
#define MAP_HEIGHT 21
#define WORLD_WIDTH 401
#define WORLD_HEIGHT 401
#define START_X 200
#define START_Y 200

typedef struct coordinate {
    int x;
    int y;
} coordinate_t;

typedef enum trainer_type {
    pc_e,
    hiker_e,
    rival_e,
    pacer_e,
    wanderer_e,
    sentry_e,
    explorer_e,
    swimmer_e,
} trainer_type_e;

typedef enum {
    clearing,
    grass,
    boulder,
    edge,
    tree,
    water,
    road,
    pokemart,
    pokecenter,
    gate,
    bridge,
} terrain_e;

class trainer {
    protected:
        trainer_type_e type;
        int next_turn;
        int seq_num;
        coordinate_t pos;
        coordinate_t dir;
    public:
        explicit trainer(trainer_type_e t) : pos(), dir()
        {
            type = t;
            next_turn = 0;
            seq_num = 0;
            pos.x = 0;
            pos.y = 0;
        }
        explicit trainer(trainer *t) : pos(), dir()
        {
            type = t->type;
            next_turn = t->next_turn;
            seq_num = t->seq_num;
            pos = t->pos;
            dir = t->dir;
        }
        ~trainer() {}
        
        trainer_type_e get_type() { return type; }
        void set_next_turn(int next) { next_turn = next; }
        int get_next_turn() const { return next_turn; }
        int get_seq_num() const { return seq_num; }
        void set_pos(coordinate_t p) { pos = p; }
        void set_pos_y(int y) { pos.y = y; }
        void set_pos_x(int x) { pos.x = x; }
        coordinate_t get_pos() { return pos; }
        void set_dir_y(int y) { dir.y = y; }
        void set_dir_x(int x) { dir.x = x; }
        coordinate_t get_dir() { return dir; }
};

class pc : public trainer {
    public:
        pc() : trainer(pc_e) { seq_num = 0; };
        explicit pc(trainer *t) : trainer(t) {}
        ~pc() {}
};

class npc : public trainer {
    public:
        explicit npc(trainer *t) : trainer(t)
        { 
            switch (t->get_type())
            {
                case hiker_e:
                    seq_num = 1;
                    break;
                case rival_e:
                    seq_num = 2;
                    break;
                case pacer_e:
                    seq_num = 3;
                    break;
                case wanderer_e:
                    seq_num = 4;
                    break;
                case sentry_e:
                    seq_num = 5;
                    break;
                case explorer_e:
                    seq_num = 6;
                    break;
                case swimmer_e:
                    seq_num = 7;
                    break;
                default:
                    seq_num = 0;
                    break;
            }
        }
        ~npc() {}
};

class map {
    private:
        int n, s, w, e;  

        coordinate_t pc_pos;
        heap_t *turn_heap;
        int pc_turn;
    public:
        terrain_e terrain_map[MAP_HEIGHT][MAP_WIDTH];
        trainer *trainer_map[MAP_HEIGHT][MAP_WIDTH];
        map() : pc_pos(), terrain_map(), trainer_map()
        {
            int x, y;

            for (y = 0; y < MAP_HEIGHT; y++) {
                for (x = 0; x < MAP_WIDTH; x++) {
                    terrain_map[y][x] = clearing;
                    trainer_map[y][x] = nullptr;
                }
            }

            n = 0;
            s = 0;
            w = 0;
            e = 0;

            turn_heap = nullptr;
            pc_turn = 0;
            pc_pos.y = 0;
            pc_pos.x = 0;
        }
        ~map()
        {
            int x, y;

            heap_delete(turn_heap);
            free(turn_heap);

            for (y = 0; y < MAP_HEIGHT; y++) {
                for (x = 0; x < MAP_WIDTH; x++) {
                    if (trainer_map[y][x] != nullptr) {
                        free(trainer_map[y][x]);
                    }
                }
            }
        }
        static void generate_map(map *map, int n, int s, int w, int e, int manhattan_distance);
        static void trainer_map_init(map *map, int num_trainers, pc *pc);
        static void place_pc(map *map, pc *pc);
        int get_n() const { return n; }
        void set_n(int north) { n = north; }
        int get_s() const { return s; }
        void set_s(int south) { s = south; }
        int get_w() const { return w; }
        void set_w(int west) { w = west; }
        int get_e() const { return e; }
        void set_e(int east) { e = east; }
        coordinate_t get_pc_pos() { return pc_pos; }
        void set_pc_pos(coordinate_t pos) { pc_pos = pos; }
        heap_t *get_turn_heap() { return turn_heap; }
        void set_turn_heap(heap_t *heap) { turn_heap = heap; }
        int get_pc_turn() const { return pc_turn; }
        void set_pc_turn(int turn) { pc_turn = turn; }
};

static int32_t turn_cmp(const void *key, const void *with) { 
    int32_t next_turn = ((trainer *) key)->get_next_turn() - ((trainer *) with)->get_next_turn();
    return (next_turn == 0) ? ((trainer *) key)->get_seq_num() - ((trainer *) with)->get_seq_num() : next_turn;
}

void build_bridges(map *map)
{
    int x, y;

    for (y = 1; y < MAP_HEIGHT - 1; y++) {
        for (x = 1; x < MAP_WIDTH - 1; x++) {
            if (map->terrain_map[y][x] == road) {
                if (map->terrain_map[y - 1][x    ] == water ||
                    map->terrain_map[y    ][x + 1] == water ||
                    map->terrain_map[y + 1][x    ] == water ||
                    map->terrain_map[y    ][x - 1] == water) {
                    map->terrain_map[y][x] = bridge;
                }
            }
        }
    }
}

void building_to_map(map *map, terrain_e building, int y, int x, int gate_row) {
    int building_connected = 0;

    map->terrain_map[y][x] = building;
    map->terrain_map[y + 1][x] = building;
    map->terrain_map[y][x + 1] = building;
    map->terrain_map[y + 1][x + 1] = building;

    map->terrain_map[y - 1][x] = road;
    map->terrain_map[y - 1][x + 1] = road;
    map->terrain_map[y - 1][x + 2] = road;
    map->terrain_map[y][x + 2] = road;
    map->terrain_map[y + 1][x + 2] = road;
    map->terrain_map[y + 2][x + 2] = road;
    map->terrain_map[y + 2][x + 1] = road;
    map->terrain_map[y + 2][x] = road;
    map->terrain_map[y + 2][x - 1] = road;
    map->terrain_map[y + 1][x - 1] = road;
    map->terrain_map[y][x - 1] = road;
    map->terrain_map[y - 1][x - 1] = road;

    if (y > gate_row + 1 && y > 3) {
        y--;
        while (y > 1 && map->terrain_map[y - 1][x] != road) {
            y--;
            map->terrain_map[y][x] = road;
        }

        if (y != 1) {
            building_connected = 1;
        }
    } else if (y < gate_row - 2 && y < MAP_HEIGHT - 4) { 
        y += 2;
        while (y < MAP_HEIGHT - 2 && map->terrain_map[y + 1][x] != road) {
            y++;
            map->terrain_map[y][x] = road;
        }

        if (y != MAP_HEIGHT - 2) {
            building_connected = 1;
        }
    }

    if (building_connected == 0) { 
        if (map->get_w() > 0) {
            while (x > 0 && map->terrain_map[y][x - 1] != road) {
                x--;
                map->terrain_map[y][x] = road;
            }

            if (x > 0) {
                building_connected = 1;
            }
        } else { 
            while (x < MAP_WIDTH - 1 && map->terrain_map[y][x + 1] != road) {
                x++;
                map->terrain_map[y][x] = road;
            }

            if (x < MAP_WIDTH - 1) {
                building_connected = 1;
            }
        }

        if (map->get_n() > 0 && building_connected == 0) { 
            while (y > 1 && map->terrain_map[y - 1][x] != road) {
                y--;
                map->terrain_map[y][x] = road;
            }
        } else if (building_connected == 0) { 
            while (y < MAP_HEIGHT - 1 && map->terrain_map[y + 1][x] != road) {
                y++;
                map->terrain_map[y][x] = road;
            }
        }
    }
}

void place_buildings(map *map, int manhattan_distance) {
    terrain_e first_building, second_building;
    int i, y, x;
    i = rand() % 2;
    first_building = (i < 1) ? pokemart : pokecenter;
    second_building = (first_building == pokemart) ? pokecenter : pokemart;
    int building_probability = ((-45 * manhattan_distance) / 200 + 50);
    if (manhattan_distance == 0 || rand() % 100 < building_probability) {
        y = rand() % 16 + 2; 
        x = rand() % 36 + 2; 
        building_to_map(map, first_building, y, x, map->get_w());
    }

    if (manhattan_distance == 0 || rand() % 100 < building_probability) {
        y = rand() % 16 + 2; 
        x = rand() % 36 + 41; 
       building_to_map(map, second_building, y, x, map->get_e());
    }
}

void pave_roads(map *map)
{
    int x_run = map->get_s() - map->get_n();
    int y, x;

    y = 1;
    x = map->get_n();
    if (map->get_n() > 0 && map->get_s() > 0) {
        while (y < (MAP_HEIGHT - 2) / 2) { 
            map->terrain_map[y][x] = road;
            y++;
        }

        if (x_run < 0) { 
            while (x > map->get_s()) {
                map->terrain_map[y][x] = road;
                x--;
            }
        } else if (x_run > 0) {
            while (x < map->get_s()) {
                map->terrain_map[y][x] = road;
                x++;
            }
        }

        while (y < MAP_HEIGHT - 1) { 
            map->terrain_map[y][x] = road;
            y++;
        }
    }

    int y_rise = map->get_e() - map->get_w();

    x = 1;
    y = map->get_w();
    if (map->get_w() > 0 && map->get_e() > 0) {
        while (x < (MAP_WIDTH - 2) / 2) { 
            map->terrain_map[y][x] = road;
            x++;
        }

        if (y_rise < 0) { 
            while (y > map->get_e()) {
                map->terrain_map[y][x] = road;
                y--;
            }
        } else if (y_rise > 0) { 
            while (y < map->get_e()) {
                map->terrain_map[y][x] = road;
                y++;
            }
        }

        while (x < MAP_WIDTH - 1) { 
            map->terrain_map[y][x] = road;
            x++;
        }
    }
}

void pave_roads_edge(map *map)
{
    int y, x;

    if (map->get_n() < 0) {
        x = map->get_s();

        if (map->get_w() < 0) { 
            for (y = MAP_HEIGHT - 2; y > map->get_e(); y--) {
                map->terrain_map[y][x] = road;
            }

            for (x = map->get_s(); x < MAP_WIDTH - 1; x++) {
                map->terrain_map[y][x] = road;
            }
        } else if (map->get_e() < 0) { 
            for (y = MAP_HEIGHT - 2; y > map->get_w(); y--) {
                map->terrain_map[y][x] = road;
            }

            for (x = map->get_s(); x > 0; x--) {
                map->terrain_map[y][x] = road;
            }
        } else {
            pave_roads(map);
            y = MAP_HEIGHT - 2;
            while (map->terrain_map[y][x] != road && y > 0) {
                map->terrain_map[y][x] = road;
                y--;
            }
        }
    } else if (map->get_s() < 0) {
        x = map->get_n();

        if (map->get_e() < 0) { 
            for (y = 0; y < map->get_w(); y++) {
                map->terrain_map[y][x] = road;
            }

            for (x = map->get_n(); x > 0; x--) {
                map->terrain_map[y][x] = road;
            }
        } else if (map->get_w() < 0) { 
            for (y = 0; y < map->get_e(); y++) {
                map->terrain_map[y][x] = road;
            }

            for (x = map->get_n(); x < MAP_WIDTH - 1; x++) {
                map->terrain_map[y][x] = road;
            }
        } else { 
            pave_roads(map);
            y = 0;
            while (map->terrain_map[y][x] != road && y < MAP_HEIGHT - 1) {
                map->terrain_map[y][x] = road;
                y++;
            }
        }
    } else if (map->get_w() < 0) { 
        pave_roads(map);

        y = map->get_e();
        x = MAP_WIDTH - 2;
        while (map->terrain_map[y][x] != road && x > 0) {
            map->terrain_map[y][x] = road;
            x--;
        }
    } else if (map->get_e() < 0) { 
        pave_roads(map); 

        y = map->get_w();
        x = 1;
        while (map->terrain_map[y][x] != road && x < MAP_WIDTH - 1) {
            map->terrain_map[y][x] = road;
            x++;
        }
    }
}

void generate_regional_terrain(map *map, int west_bound, int east_bound)
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
                        map->terrain_map[y + row][x + col] = regions[i];
                    } else { 
                        map->terrain_map[y + row][x - col] = regions[i];
                    }
                }
            } else {
                for (col = 0; col < region_width; col++) {
                    if (x + col < MAP_WIDTH - 1) { 
                        map->terrain_map[y - row][x + col] = regions[i];
                    } else { 
                        map->terrain_map[y - row][x - col] = regions[i];
                    }
}}}}}

void generate_terrain(map *map)
{
    generate_regional_terrain(map, 1, MAP_WIDTH / 2 - 1); 
    generate_regional_terrain(map, MAP_WIDTH / 2 - 1, MAP_WIDTH - 1); 
    int i, y, x;

    for (i = 0; i < 20; i++) {
        y = rand() % (MAP_HEIGHT - 2) + 1;
        x = rand() % (MAP_WIDTH - 2) + 1;

        map->terrain_map[y][x] = edge;

        y = rand() % (MAP_HEIGHT - 2) + 1;
        x = rand() % (MAP_WIDTH - 2) + 1;

        map->terrain_map[y][x] = tree;
    }
}

void construct_border(map *map, int n, int s, int w, int e)
{
    int i, j, tmp;

    for (i = 0; i < MAP_HEIGHT; i++) {
        map->terrain_map[i][0] = edge;
        map->terrain_map[i][MAP_WIDTH - 1] = edge;
    }

    for (j = 0; j < MAP_WIDTH; j++) {
        map->terrain_map[0][j] = edge;
        map->terrain_map[MAP_HEIGHT - 1][j] = edge;
    }
    tmp = (n == 0) ? rand() % (MAP_WIDTH - 2) + 1 : n; 
    map->set_n(tmp);
    tmp = (s == 0) ? rand() % (MAP_WIDTH - 2) + 1 : s;
    map->set_s(tmp);
    tmp = (w == 0) ? rand() % (MAP_HEIGHT - 2) + 1 : w;
    map->set_w(tmp);
    tmp = (e == 0) ? rand() % (MAP_HEIGHT - 2) + 1 : e;
    map->set_e(tmp);
    map->terrain_map[0][map->get_n()] = (n == -1) ? edge : gate;
    map->terrain_map[MAP_HEIGHT - 1][map->get_s()] = (s == -1) ? edge : gate;
    map->terrain_map[map->get_w()][0] = (w == -1) ? edge : gate;
    map->terrain_map[map->get_e()][MAP_WIDTH - 1] = (e == -1) ? edge : gate;
}

void map::generate_map(map *map, int n, int s, int w, int e, int manhattan_distance)
{
    construct_border(map, n, s, w, e);

    generate_terrain(map);

    if (n < 0 || s < 0 || w < 0 || e < 0) { 
        pave_roads_edge(map);
    } else {
        pave_roads(map);
    }

    place_buildings(map, manhattan_distance);

    build_bridges(map);
}

int check_trainer_position(map *map, coordinate_t pos, trainer_type_e type)
{
    int check = 1;

    if (map->terrain_map[pos.y][pos.x] == edge ||
        map->terrain_map[pos.y][pos.x] == tree ||
        map->trainer_map[pos.y][pos.x] != nullptr) {
        check = 0;
    } else if ((type == swimmer_e && map->terrain_map[pos.y][pos.x] != water) ||
               (type != swimmer_e && map->terrain_map[pos.y][pos.x] == water)) {
        check = 0;
    } else if (map->terrain_map[pos.y][pos.x] == boulder ||
               map->terrain_map[pos.y][pos.x] == tree) {
        if (type != hiker_e) {
            check = 0;
        }
    } else if ((map->terrain_map[pos.y][pos.x] == gate ||
                map->terrain_map[pos.y][pos.x] == road ||
                map->terrain_map[pos.y][pos.x] == bridge) &&
               (type != pc_e)) {
        check = 0;
    }

    return check;
}

void place_new_pc(heap_t *turn_heap, map *map)
{
    int y, x, count;
    count = 0;
    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain_map[y][x] == road || map->terrain_map[y][x] == bridge) {
                count++;
            }
        }
    }
    coordinate_t start;
    coordinate_t start_options[count];
    count = 0;
    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain_map[y][x] == road || map->terrain_map[y][x] == bridge) {
                start_options[count].x = x;
                start_options[count].y = y;
                count++;
            }
        }
    }

    start = start_options[rand() % count];

    map->trainer_map[start.y][start.x] = new trainer(pc_e);
    map->set_pc_pos(start);
    map->trainer_map[start.y][start.x]->set_pos(start);
    heap_insert(turn_heap, map->trainer_map[start.y][start.x]);
}

void map::place_pc(map *map, pc *pc)
{
    int y, x, count;
    count = 0;
    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain_map[y][x] == road || map->terrain_map[y][x] == bridge) {
                count++;
            }
        }
    }
    coordinate_t start_options[count]; 
    count = 0;
    for (y = 1; y < MAP_HEIGHT; y++) {
        for (x = 1; x < MAP_WIDTH; x++) {
            if (map->terrain_map[y][x] == road || map->terrain_map[y][x] == bridge) {
                start_options[count].x = x;
                start_options[count].y = y;
                count++;
            }
        }
    }

    pc->set_pos(start_options[rand() % count]);
}

void place_npc(heap_t *turn_heap, map *map, trainer_type_e type)
{
    coordinate_t start;
    int valid_character_position;

    start.x = (rand() % MAP_WIDTH - 2) + 1;
    start.y = (rand() % MAP_HEIGHT - 2) + 1;

    valid_character_position = check_trainer_position(map, start, type);

    while(!valid_character_position) {
        start.x = (rand() % MAP_WIDTH - 2) + 1;
        start.y = (rand() % MAP_HEIGHT - 2) + 1;

        valid_character_position = check_trainer_position(map, start, type);
    }

    map->trainer_map[start.y][start.x] = new trainer(type);
    map->trainer_map[start.y][start.x]->set_pos(start);
    heap_insert(turn_heap, map->trainer_map[start.y][start.x]);
}

void map::trainer_map_init(map *map, int num_trainers, pc *pc)
{
    int x, y, npc_count;

    map->set_turn_heap((heap_t *) malloc(sizeof (heap_t)));
    heap_init(map->get_turn_heap(), turn_cmp, nullptr);

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            map->trainer_map[y][x] = nullptr;
        }
    }

    if (pc == nullptr) {
        place_new_pc(map->get_turn_heap(), map);
    } else {
        map->trainer_map[pc->get_pos().y][pc->get_pos().x] = pc;
        map->set_pc_pos(pc->get_pos());
        pc->set_next_turn(0);
    }

    if (num_trainers == 1) {
        place_npc(map->get_turn_heap(), map, (trainer_type_e) (rand() % 7 + 1));
    } else if (num_trainers >= 2) {
        place_npc(map->get_turn_heap(), map, hiker_e);
        place_npc(map->get_turn_heap(), map, rival_e);

        npc_count = 2;
        if (num_trainers >= 7) {
            place_npc(map->get_turn_heap(), map, pacer_e);
            place_npc(map->get_turn_heap(), map, wanderer_e);
            place_npc(map->get_turn_heap(), map, sentry_e);
            place_npc(map->get_turn_heap(), map, explorer_e);
            place_npc(map->get_turn_heap(), map, swimmer_e);
            npc_count = 7;
        }

        while(npc_count < num_trainers) {
            place_npc(map->get_turn_heap(), map, (trainer_type_e) (rand() % 7 + 1));
            npc_count++;
        }
    }
}

class path {
    private:
        heap_node_t *heap_node;
        coordinate_t coordinate;
        terrain_e terrain;
        int terrain_cost;
        int cost;
    public:
        path() : coordinate()
        {
            heap_node = nullptr;
            coordinate.y = 0;
            coordinate.x = 0;
            terrain = edge;
            terrain_cost = INT_MAX;
            cost = INT_MAX;
        }
        ~path() {};
        
        static int calculate_terrain_cost(terrain_e terrain_type, trainer_type_e trainer_type);
        static void dijkstra_path(heap_t *heap, map *map, path path_map[MAP_HEIGHT][MAP_WIDTH], trainer_type_e trainer_type);

        void set_heap_node(heap_node_t *node) { heap_node = node; }
        heap_node_t *get_heap_node() { return heap_node; }
        void set_coordinate(coordinate_t c) { coordinate = c; }
        coordinate_t get_coordinate() { return coordinate; }
        void set_terrain(terrain_e t) { terrain = t; }
        terrain_e get_terrain() { return terrain; }
        void set_terrain_cost(int t_c) { terrain_cost = t_c; }
        int get_terrain_cost() const { return terrain_cost; }
        void set_cost(int c) { cost = c; }
        int get_cost() const { return cost; }
};

int path::calculate_terrain_cost(terrain_e terrain_type, trainer_type_e trainer_type) {
    int terrain_cost = INT_MAX;

    if (terrain_type == road) {
        if (trainer_type != swimmer_e) {
            terrain_cost = 10;
        }
    } else if (terrain_type == bridge) {
        if (trainer_type != swimmer_e) {
            terrain_cost = 10;
        } else {
            terrain_cost = 7;
        }
    } else if (terrain_type == pokemart || terrain_type == pokecenter) {
        if (trainer_type == pc_e) {
            terrain_cost = 10;
        } else if (trainer_type != swimmer_e) {
            terrain_cost = 50;
        }
    } else if (terrain_type == grass) {
        if (trainer_type == hiker_e) {
            terrain_cost = 15;
        } else if (trainer_type != swimmer_e) {
            terrain_cost = 20;
        }
    } else if (terrain_type == clearing) {
        if (trainer_type != swimmer_e) {
            terrain_cost = 10;
        }
    } else if (terrain_type == boulder || terrain_type == tree) {
        if (trainer_type == hiker_e) {
            terrain_cost = 15;
        }
    } else if (terrain_type == water) {
        if (trainer_type == swimmer_e) {
            terrain_cost = 7;
        }
    } else if (terrain_type == gate) {
        if (trainer_type == pc_e) {
            terrain_cost = 10;
        }
    }

    return terrain_cost;
}

void path::dijkstra_path(heap_t *heap, map *map, path path_map[MAP_HEIGHT][MAP_WIDTH], trainer_type_e trainer_type)
{
    if (trainer_type != hiker_e && trainer_type != rival_e && trainer_type != swimmer_e) {
        return;
    }

    coordinate_t coord;
    int x, y;
    path *p;
    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            coord.y = y;
            coord.x = x;
            path_map[y][x].set_coordinate(coord);
        }
    }

    for (y = 0; y < MAP_HEIGHT; y++) { 
        for (x = 0; x < MAP_WIDTH; x++) {
            path_map[y][x].set_cost(INT_MAX);
        }
    }

    int move_cost;
    for (y = 0; y < MAP_HEIGHT; y++) { 
        for (x = 0; x < MAP_WIDTH; x++) {
            path_map[y][x].set_terrain(map->terrain_map[y][x]);

            move_cost = path::calculate_terrain_cost(path_map[y][x].get_terrain(), trainer_type);

            if (map->trainer_map[y][x] != nullptr) {
                if (map->trainer_map[y][x]->get_type() != pc_e) {
                    move_cost = INT_MAX;
                } else {
                    move_cost = 0;
                }
            }

            if (move_cost != INT_MAX) {
                if (move_cost == 0) {
                    path_map[y][x].set_cost(move_cost); 
                }
                path_map[y][x].set_heap_node(heap_insert(heap, &path_map[y][x]));
            } else {
                path_map[y][x].set_heap_node(nullptr);
            }

            path_map[y][x].set_terrain_cost(move_cost);
        }
    }

    while ((p = (path *) heap_remove_min(heap))) { 
        p->set_heap_node(nullptr);

        if ((path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_cost() >
             (p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_terrain_cost()) &&
              p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].set_cost(p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y - 1][p->get_coordinate().x - 1].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y - 1][p->get_coordinate().x    ].get_heap_node()) &&
            (path_map[p->get_coordinate().y - 1][p->get_coordinate().x    ].get_cost() >
             (p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x].get_terrain_cost()) &&
              p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y - 1][p->get_coordinate().x    ].set_cost(p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x    ].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y - 1][p->get_coordinate().x    ].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_cost() >
             (p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_terrain_cost()) &&
              p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].set_cost(p->get_cost() + path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y - 1][p->get_coordinate().x + 1].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_cost() >
             (p->get_cost() + path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_terrain_cost()) &&
              p->get_cost() + path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].set_cost(p->get_cost()+ path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y    ][p->get_coordinate().x - 1].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_cost() >
             (p->get_cost()+ path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_terrain_cost()) &&
              p->get_cost()+ path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].set_cost(p->get_cost()+ path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y    ][p->get_coordinate().x + 1].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_cost() >
             (p->get_cost() + path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_terrain_cost()) &&
              p->get_cost() + path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].set_cost(p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y + 1][p->get_coordinate().x - 1].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_heap_node()) &&
            (path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_cost() >
             (p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_terrain_cost()) &&
              p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].set_cost(p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y + 1][p->get_coordinate().x    ].get_heap_node());
        }
        if ((path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_heap_node()) &&
            (path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_cost() >
             (p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_terrain_cost()) &&
              p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_terrain_cost() > 0)) {
            path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].set_cost(p->get_cost()+ path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_terrain_cost());
            heap_decrease_key_no_replace(heap, path_map[p->get_coordinate().y + 1][p->get_coordinate().x + 1].get_heap_node());
        }
    }
}

class action {
    public:
        static void enter_building(map *map, pc *pc);
        static void trainer_info(map *map, int num_trainers);
        static int battle();
        static char move_pc(map *map, pc *pc, int input); 
        static void move_dijkstra_trainer(heap *path_heap, path path_map[MAP_HEIGHT][MAP_WIDTH], map *map, npc *npc);
        static void move_wanderer_explorer(map *map, npc *npc);
        static void move_swimmer(heap_t *path_heap, path path_map[MAP_HEIGHT][MAP_WIDTH], map *map, npc *m, coordinate_t pc_pos);
        static void move_pacer(map *map, npc *p);
};


int check_random_turn(int random_direction, map *map, coordinate_t pos, trainer_type_e type) {
    int check, terrain_cost;
    path p;

    check = 0;

    if (random_direction == 0) {
        if (type == wanderer_e || type == swimmer_e) {
            if (map->terrain_map[pos.y][pos.x] == map->terrain_map[pos.y - 1][pos.x] && map->trainer_map[pos.y - 1][pos.x] == nullptr) {
                check = 1;
            }
        } else if (type == explorer_e) {
            terrain_cost = p.calculate_terrain_cost(map->terrain_map[pos.y - 1][pos.x], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y - 1][pos.x] == nullptr) {
                check = 1;
            }
        }
    } else if (random_direction == 1) { 
        if (type == wanderer_e || type == swimmer_e) {
            if (map->terrain_map[pos.y][pos.x] == map->terrain_map[pos.y][pos.x + 1] && map->trainer_map[pos.y][pos.x + 1] == nullptr) {
                check = 1;
            }
        } else if (type == explorer_e) {
            terrain_cost = p.calculate_terrain_cost(map->terrain_map[pos.y][pos.x + 1], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y][pos.x + 1] == nullptr) {
                check = 1;
            }
        }
    } else if (random_direction == 2) { 
        if (type == wanderer_e || type == swimmer_e) {
            if (map->terrain_map[pos.y][pos.x] == map->terrain_map[pos.y + 1][pos.x] && map->trainer_map[pos.y + 1][pos.x] == nullptr) {
                check = 1;
            }
        } else if (type == explorer_e) {
            terrain_cost = p.calculate_terrain_cost(map->terrain_map[pos.y + 1][pos.x], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y + 1][pos.x] == nullptr) {
                check = 1;
            }
        }
    }  else if (random_direction == 3) { 
        if (type == wanderer_e || type == swimmer_e) {
            if (map->terrain_map[pos.y][pos.x] == map->terrain_map[pos.y][pos.x - 1] && map->trainer_map[pos.y][pos.x - 1] == nullptr) {
                check = 1;
            }
        } else if (type == explorer_e) {
            terrain_cost = path::calculate_terrain_cost(map->terrain_map[pos.y][pos.x - 1], type);
            if (terrain_cost != INT_MAX && map->trainer_map[pos.y][pos.x - 1] == nullptr) {
                check = 1;
            }
        }
    }

    return check;
}

void random_turn(map *map, trainer *t) {
    int random_direction, valid_direction;
    int num_fail;
    int battle_outcome;

    num_fail = 0;
    random_direction = rand() % 4;
    valid_direction = check_random_turn(random_direction, map, t->get_pos(), t->get_type());
    while (!valid_direction) {
        num_fail++;
        if (num_fail > 20) {
            return;
        }
        random_direction = rand() % 4;
        valid_direction = check_random_turn(random_direction, map, t->get_pos(), t->get_type());
    }

    if (random_direction == 0) { 
        t->set_dir_y(-1);
        t->set_dir_x(0);
        if (map->trainer_map[t->get_pos().y - 1][t->get_pos().x    ] != nullptr &&
            map->trainer_map[t->get_pos().y - 1][t->get_pos().x    ]->get_type() == pc_e) {
            battle_outcome = action::battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                random_turn(map, t);
            } else if (battle_outcome == -1) { 
                t->set_next_turn(-1);
            }
        } else {
            map->trainer_map[t->get_pos().y][t->get_pos().x] = nullptr;
            t->set_pos_y(t->get_pos().y - 1);
            map->trainer_map[t->get_pos().y][t->get_pos().x] = t;
        }
    } else if (random_direction == 1) { 
        t->set_dir_y(0);
        t->set_dir_x(1);
        if (map->trainer_map[t->get_pos().y    ][t->get_pos().x + 1] != nullptr &&
            map->trainer_map[t->get_pos().y    ][t->get_pos().x + 1]->get_type() == pc_e) {
            battle_outcome = action::battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                random_turn(map, t);
            } else if (battle_outcome == -1) { 
                t->set_next_turn(-1);
            }
        } else {
            map->trainer_map[t->get_pos().y][t->get_pos().x] = nullptr;
            t->set_pos_x(t->get_pos().x + 1);
            map->trainer_map[t->get_pos().y][t->get_pos().x] = t;
        }
    } else if (random_direction == 2) { 
        t->set_dir_y(1);
        t->set_dir_x(0);
        if (map->trainer_map[t->get_pos().y + 1][t->get_pos().x    ] != nullptr &&
            map->trainer_map[t->get_pos().y + 1][t->get_pos().x    ]->get_type() == pc_e) {
            battle_outcome = action::battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                random_turn(map, t);
            } else if (battle_outcome == -1) { 
                t->set_next_turn(-1);
            }
        } else {
            map->trainer_map[t->get_pos().y][t->get_pos().x] = nullptr;
            t->set_pos_y(t->get_pos().y + 1);
            map->trainer_map[t->get_pos().y][t->get_pos().x] = t;
        }
    } else if (random_direction == 3) { 
        t->set_dir_y(0);
        t->set_dir_x(-1);
        if (map->trainer_map[t->get_pos().y    ][t->get_pos().x - 1] != nullptr &&
            map->trainer_map[t->get_pos().y    ][t->get_pos().x - 1]->get_type() == pc_e) {
            battle_outcome = action::battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                random_turn(map, t);
            } else if (battle_outcome == -1) { 
                t->set_next_turn(-1);
            }
        } else {
            map->trainer_map[t->get_pos().y][t->get_pos().x] = nullptr;
            t->set_pos_x(t->get_pos().x - 1);
            map->trainer_map[t->get_pos().y][t->get_pos().x] = t;
        }
    }
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);		
    wrefresh(local_win);		

    return local_win;
}

void destroy_win(WINDOW *local_win)
{
    wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(local_win);
    delwin(local_win);
}

void action::enter_building(map *map, pc *pc)
{
    int key;
    WINDOW *building_win;

    if (map->terrain_map[pc->get_pos().y][pc->get_pos().x] == pokecenter || map->terrain_map[pc->get_pos().y][pc->get_pos().x] == pokemart) {
        building_win = create_newwin(24, 80, 0, 0);
        mvwprintw(building_win, 1, 1, "You are in a building. Press '<' to exit.");
        wrefresh(building_win);

        while((key = getch()) != '<')
        {
            wprintw(building_win, "You are in a building. Press '<' to exit.");
            wrefresh(building_win);
        }

        destroy_win(building_win);

        pc->set_next_turn(pc->get_next_turn() + 10);
    }
}

void action::trainer_info(map *map, int num_trainers)
{
    int x, y, npc_rise, npc_run, key, i, shift;
    char trainer_char;
    std::string output;
    const char *trainer_output;
    WINDOW *trainer_win;
    char *trainer_info[num_trainers];

    trainer_win = create_newwin(10, 24, 1, 0);
    i = 0;
    keypad(trainer_win, TRUE);

    for (y = 1; y < MAP_HEIGHT - 1; y++) {
        for (x = 1; x < MAP_WIDTH - 1; x++) {
            if (map->trainer_map[y][x] != nullptr) {
                switch (map->trainer_map[y][x]->get_type())
                {
                    case pc_e:
                        trainer_char = '@';
                        break;
                    case hiker_e:
                        trainer_char = 'h';
                        break;
                    case rival_e:
                        trainer_char = 'r';
                        break;
                    case pacer_e:
                        trainer_char = 'p';
                        break;
                    case wanderer_e:
                        trainer_char = 'w';
                        break;
                    case sentry_e:
                        trainer_char = 's';
                        break;
                    case explorer_e:
                        trainer_char = 'e';
                        break;
                    case swimmer_e:
                        trainer_char = 'm';
                        break;
                }

                npc_rise = map->get_pc_pos().y - map->trainer_map[y][x]->get_pos().y;
                npc_run = map->get_pc_pos().x - map->trainer_map[y][x]->get_pos().x;

                if (npc_rise < 0) { 
                    if (npc_run < 0) { 
                        output.append(1, trainer_char);
                        output.append(", ");
                        output.append(std::to_string(npc_rise * -1));
                        output.append(" south and east ");
                        output.append(std::to_string(npc_run * -1));
                    } else { 
                        output.append(1, trainer_char);
                        output.append(", ");
                        output.append(std::to_string(npc_rise * -1));
                        output.append(" south and west ");
                        output.append(std::to_string(npc_run));
                    }
                } else { 
                    if (npc_run < 0) { 
                        output.append(1, trainer_char);
                        output.append(", ");
                        output.append(std::to_string(npc_rise));
                        output.append(" north and east ");
                        output.append(std::to_string(npc_run * -1));
                    } else { 
                        output.append(1, trainer_char);
                        output.append(", ");
                        output.append(std::to_string(npc_rise));
                        output.append(" north and west ");
                        output.append(std::to_string(npc_run));
                    }
                }

                trainer_info[i] = (char *) malloc(output.size());
                trainer_output = output.c_str();
                strcpy(trainer_info[i], trainer_output);
                i++;
            }
        }
    }

    for (i = 0; i < num_trainers || i < 10; i++) {
        mvwprintw(trainer_win, i, 0, trainer_info[i]);
    }

    wrefresh(trainer_win);

    shift = 0;
    while((key = getch()) != 27)
    {
        switch (key)
        {
            case KEY_DOWN:
                if (num_trainers > 10 && num_trainers - shift > 10) {
                    shift++;
                    for (i = 0; i < 10 && i + shift < num_trainers; i++) {
                        mvwprintw(trainer_win, i, 0, trainer_info[i + shift]);
                    }
                    wrefresh(trainer_win);
                }
                break;
            case KEY_UP:
                if (shift > 0 && num_trainers > 10) {
                    shift--;
                    for (i = 0; i < 10 && i + shift < num_trainers; i++) {
                        mvwprintw(trainer_win, i, 0, trainer_info[i + shift]);
                    }
                    wrefresh(trainer_win);
                }
                break;
            default:
                continue;
        }
    }

    for (i = 0; i < num_trainers; i++) {
        free(trainer_info[i]);
    }

    destroy_win(trainer_win);
}

int action::battle()
{
    int key;
    WINDOW *battle_win;

    battle_win = create_newwin(24, 80, 0, 0);
    mvwprintw(battle_win, 1, 1, "You are in a battle. Press '<' to exit.");
    wrefresh(battle_win);

    while((key = getch()) != '<')
    {
        wprintw(battle_win, "You are in a battle. Press '<' to exit.");
        wrefresh(battle_win);
    }

    destroy_win(battle_win);

    return 0;
}

char action::move_pc(map *map, pc *pc, int input)
{
    if (map->get_turn_heap()) {}
    char ret_val;
    coordinate_t new_pos;
    int battle_outcome;

    ret_val = 0;

    if (input == '7' || input == 'y') {
        new_pos.y = pc->get_pos().y - 1;
        new_pos.x = pc->get_pos().x - 1;
    } else if (input == '8' || input == 'k') {
        new_pos.y = pc->get_pos().y - 1;
        new_pos.x = pc->get_pos().x    ;
    } else if (input == '9' || input == 'u') {
        new_pos.y = pc->get_pos().y - 1;
        new_pos.x = pc->get_pos().x + 1;
    } else if (input == '6' || input == 'l') {
        new_pos.y = pc->get_pos().y    ;
        new_pos.x = pc->get_pos().x + 1;
    } else if (input == '3' || input == 'n') {
        new_pos.y = pc->get_pos().y + 1;
        new_pos.x = pc->get_pos().x + 1;
    } else if (input == '2' || input == 'j') {
        new_pos.y = pc->get_pos().y + 1;
        new_pos.x = pc->get_pos().x    ;
    } else if (input == '1' || input == 'b') {
        new_pos.y = pc->get_pos().y + 1;
        new_pos.x = pc->get_pos().x - 1;
    } else if (input == '4' || input == 'h') {
        new_pos.y = pc->get_pos().y    ;
        new_pos.x = pc->get_pos().x - 1;
    }

    if (path::calculate_terrain_cost(map->terrain_map[new_pos.y][new_pos.x], pc->get_type()) != INT_MAX) { 
        if (map->terrain_map[new_pos.y][new_pos.x] == gate) {
            if (new_pos.y == 0) {
                ret_val = 'n';
            } else if (new_pos.y == MAP_HEIGHT - 1) {
                ret_val = 's';
            } else if (new_pos.x == 0) {
                ret_val = 'w';
            } else if (new_pos.x == MAP_WIDTH - 1) {
                ret_val = 'e';
            }  
        }
        
        if (map->trainer_map[new_pos.y][new_pos.x] != nullptr) {
            if (map->trainer_map[new_pos.y][new_pos.x]->get_next_turn() > -1) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    pc->set_next_turn(-1);
                } else if (battle_outcome == -1) { 
                    map->trainer_map[new_pos.y][new_pos.x]->set_next_turn(-1);
                }
            }
        } else {
            map->trainer_map[pc->get_pos().y][pc->get_pos().x] = nullptr;
            pc->set_pos(new_pos);
            map->trainer_map[pc->get_pos().y][pc->get_pos().x] = pc;
            map->set_pc_pos(pc->get_pos());
        }

        if (pc->get_next_turn() > -1) {
            pc->set_next_turn(pc->get_next_turn() + path::calculate_terrain_cost(map->terrain_map[pc->get_pos().y][pc->get_pos().x], pc->get_type()));
        }
    }

    return ret_val;
}

void action::move_dijkstra_trainer(heap_t *path_heap, path path_map[MAP_HEIGHT][MAP_WIDTH], map *map, npc *npc)
{
    path *cheapest_path;
    int battle_outcome;

    cheapest_path = &path_map[npc->get_pos().y - 1][npc->get_pos().x - 1];
    battle_outcome = 0;

    path::dijkstra_path(path_heap, map, path_map, npc->get_type());

    if (path_map[npc->get_pos().y - 1][npc->get_pos().x].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y - 1][npc->get_pos().x];
    }
    if (path_map[npc->get_pos().y - 1][npc->get_pos().x + 1].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y - 1][npc->get_pos().x + 1];
    }
    if (path_map[npc->get_pos().y][npc->get_pos().x - 1].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y][npc->get_pos().x - 1];
    }
    if (path_map[npc->get_pos().y][npc->get_pos().x + 1].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y][npc->get_pos().x + 1];
    }
    if (path_map[npc->get_pos().y + 1][npc->get_pos().x - 1].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y + 1][npc->get_pos().x - 1];
    }
    if (path_map[npc->get_pos().y + 1][npc->get_pos().x].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y + 1][npc->get_pos().x];
    }
    if (path_map[npc->get_pos().y + 1][npc->get_pos().x + 1].get_cost() < cheapest_path->get_cost()) {
        cheapest_path = &path_map[npc->get_pos().y + 1][npc->get_pos().x + 1];
    }
    if (cheapest_path->get_cost() == 0) {
        cheapest_path = &path_map[npc->get_pos().y    ][npc->get_pos().x    ];

        battle_outcome = battle();

        if (battle_outcome == 1) { 
            map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
        } else if (battle_outcome == -1) { 
            npc->set_next_turn(-1);
        }
    }

    map->trainer_map[npc->get_pos().y][npc->get_pos().x] = nullptr;
    npc->set_pos(cheapest_path->get_coordinate());
    map->trainer_map[npc->get_pos().y][npc->get_pos().x] = npc;
    if (npc->get_next_turn() > -1) {
        npc->set_next_turn(npc->get_next_turn() + cheapest_path->get_terrain_cost());
    }
}

void action::move_wanderer_explorer(map *map, npc *npc)
{
    int move_cost, battle_outcome;
    terrain_e current_terrain;

    current_terrain = map->terrain_map[npc->get_pos().y][npc->get_pos().x];
    if (npc->get_dir().x == 0 && npc->get_dir().y <= 0) {
        move_cost = path::calculate_terrain_cost(map->terrain_map[npc->get_pos().y - 1][npc->get_pos().x], npc->get_type());

        if ((npc->get_type() == wanderer_e && (current_terrain == map->terrain_map[npc->get_pos().y - 1][npc->get_pos().x])) ||
            (npc->get_type() == explorer_e && (move_cost != INT_MAX))) {
            npc->set_dir_y(-1);
            if (map->trainer_map[npc->get_pos().y - 1][npc->get_pos().x    ] == nullptr) {
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = nullptr;
                npc->set_pos_y(npc->get_pos().y - 1);
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = npc;
            } else if (map->trainer_map[npc->get_pos().y - 1][npc->get_pos().x    ]->get_type() == pc_e) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                    random_turn(map, npc);
                } else if (battle_outcome == -1) { 
                    npc->set_next_turn(-1);
                }
            } else {
                random_turn(map, npc);
            }
        } else {
            random_turn(map, npc);
        }
    } else if (npc->get_dir().x >= 0 && npc->get_dir().y == 0) {
        move_cost = path::calculate_terrain_cost(map->terrain_map[npc->get_pos().y][npc->get_pos().x + 1], npc->get_type());

        if ((npc->get_type() == wanderer_e && (current_terrain == map->terrain_map[npc->get_pos().y][npc->get_pos().x + 1])) ||
            (npc->get_type() == explorer_e && (move_cost != INT_MAX))) {
            npc->set_dir_x(1);
            if (map->trainer_map[npc->get_pos().y    ][npc->get_pos().x + 1] == nullptr) {
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = nullptr;
                npc->set_pos_x(npc->get_pos().x + 1);
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = npc;
            }  else if (map->trainer_map[npc->get_pos().y    ][npc->get_pos().x + 1]->get_type() == pc_e) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                    random_turn(map, npc);
                } else if (battle_outcome == -1) { 
                    npc->set_next_turn(-1);
                }
            } else {
                random_turn(map, npc);
            }
        } else {
            random_turn(map, npc);
        }
    } else if (npc->get_dir().x == 0 && npc->get_dir().y >= 0) {
        move_cost = path::calculate_terrain_cost(map->terrain_map[npc->get_pos().y + 1][npc->get_pos().x], npc->get_type());

        if ((npc->get_type() == wanderer_e && (current_terrain == map->terrain_map[npc->get_pos().y + 1][npc->get_pos().x])) ||
            (npc->get_type() == explorer_e && (move_cost != INT_MAX))) {
            npc->set_dir_y(1);
            if (map->trainer_map[npc->get_pos().y + 1][npc->get_pos().x    ] == nullptr) {
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = nullptr;
                npc->set_pos_y(npc->get_pos().y + 1);
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = npc;
            } else if (map->trainer_map[npc->get_pos().y + 1][npc->get_pos().x    ]->get_type() == pc_e) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                    random_turn(map, npc);
                } else if (battle_outcome == -1) { 
                    npc->set_next_turn(-1);
                }
            } else {
                random_turn(map, npc);
            }
        } else {
            random_turn(map, npc);
        }
    } else if (npc->get_dir().x <= 0 && npc->get_dir().y == 0) {
        move_cost = path::calculate_terrain_cost(map->terrain_map[npc->get_pos().y    ][npc->get_pos().x - 1], npc->get_type());

        if ((npc->get_type() == wanderer_e && (current_terrain == map->terrain_map[npc->get_pos().y    ][npc->get_pos().x - 1])) ||
            (npc->get_type() == explorer_e && (move_cost != INT_MAX))) {
            npc->set_dir_x(-1);
            if (map->trainer_map[npc->get_pos().y    ][npc->get_pos().x - 1] == nullptr) {
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = nullptr;
                npc->set_pos_x(npc->get_pos().x - 1);
                map->trainer_map[npc->get_pos().y][npc->get_pos().x] = npc;
            } else if (map->trainer_map[npc->get_pos().y    ][npc->get_pos().x - 1]->get_type() == pc_e) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                    random_turn(map, npc);
                } else if (battle_outcome == -1) { 
                    npc->set_next_turn(-1);
                }
            } else {
                random_turn(map, npc);
            }
        } else {
            random_turn(map, npc);
        }
    }

    if (npc->get_next_turn() > -1) {
        npc->set_next_turn(npc->get_next_turn() + path::calculate_terrain_cost(map->terrain_map[npc->get_pos().y][npc->get_pos().x], npc->get_type()));
    }
}

void action::move_swimmer(heap_t *path_heap, path path_map[MAP_HEIGHT][MAP_WIDTH], map *map, npc *m, coordinate_t pc_pos)
{
    int battle_outcome;
    if ((map->terrain_map[pc_pos.y - 1][pc_pos.x    ] == water ||
         map->terrain_map[pc_pos.y - 1][pc_pos.x    ] == bridge) ||
        (map->terrain_map[pc_pos.y    ][pc_pos.x + 1] == water ||
         map->terrain_map[pc_pos.y    ][pc_pos.x + 1] == bridge) ||
        (map->terrain_map[pc_pos.y + 1][pc_pos.x    ] == water ||
         map->terrain_map[pc_pos.y + 1][pc_pos.x    ] == bridge) ||
        (map->terrain_map[pc_pos.y    ][pc_pos.x - 1] == water ||
         map->terrain_map[pc_pos.y    ][pc_pos.x - 1] == bridge)) {
        action::move_dijkstra_trainer(path_heap, path_map, map, m);
    } else {
        if (m->get_dir().x == 0 && m->get_dir().y <= 0) {
            if (map->terrain_map[m->get_pos().y - 1][m->get_pos().x] == water ||
                map->terrain_map[m->get_pos().y - 1][m->get_pos().x] == bridge) {
                m->set_dir_y(-1);
                if (map->trainer_map[m->get_pos().y - 1][m->get_pos().x] == nullptr) {
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = nullptr;
                    m->set_pos_y(m->get_pos().y - 1);
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = m;
                } else if (map->trainer_map[m->get_pos().y - 1][m->get_pos().x]->get_type() == pc_e) {
                    battle_outcome = battle();

                    if (battle_outcome == 1) {
                        map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                        random_turn(map, m);
                    } else if (battle_outcome == -1) { 
                        m->set_next_turn(-1);
                    }
                } else {
                    random_turn(map, m);
                }
            } else {
                random_turn(map, m);
            }
        } else if (m->get_dir().x >= 0 && m->get_dir().y == 0) {
            if (map->terrain_map[m->get_pos().y][m->get_pos().x + 1] == water ||
                map->terrain_map[m->get_pos().y][m->get_pos().x + 1] == bridge) {
                m->set_dir_x(1);
                if (map->trainer_map[m->get_pos().y][m->get_pos().x + 1] == nullptr) {
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = nullptr;
                    m->set_pos_x(m->get_pos().x + 1);
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = m;
                } else if (map->trainer_map[m->get_pos().y][m->get_pos().x + 1]->get_type() == pc_e) {
                    battle_outcome = battle();

                    if (battle_outcome == 1) { 
                        map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                        random_turn(map, m);
                    } else if (battle_outcome == -1) { 
                        m->set_next_turn(-1);
                    }
                } else {
                    random_turn(map, m);
                }
            } else {
                random_turn(map, m);
            }
        } else if (m->get_dir().x == 0 && m->get_dir().y >= 0) {
            if (map->terrain_map[m->get_pos().y + 1][m->get_pos().x] == water ||
                map->terrain_map[m->get_pos().y + 1][m->get_pos().x] == bridge) {
                m->set_dir_y(1);
                if (map->trainer_map[m->get_pos().y + 1][m->get_pos().x] == nullptr) {
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = nullptr;
                    m->set_pos_y(m->get_pos().y + 1);
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = m;
                } else if (map->trainer_map[m->get_pos().y + 1][m->get_pos().x]->get_type() == pc_e) {
                    battle_outcome = battle();

                    if (battle_outcome == 1) { 
                        map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                        random_turn(map, m);
                    } else if (battle_outcome == -1) { 
                        m->set_next_turn(-1);
                    }
                } else {
                    random_turn(map, m);
                }
            } else {
                random_turn(map, m);
            }
        } else if (m->get_dir().x <= 0 && m->get_dir().y == 0) {
            if (map->terrain_map[m->get_pos().y][m->get_pos().x - 1] == water ||
                map->terrain_map[m->get_pos().y][m->get_pos().x - 1] == bridge) {
                m->set_dir_x(-1);
                if (map->trainer_map[m->get_pos().y][m->get_pos().x - 1] == nullptr) {
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = nullptr;
                    m->set_pos_x(m->get_pos().x - 1);
                    map->trainer_map[m->get_pos().y][m->get_pos().x] = m;
                } else if (map->trainer_map[m->get_pos().y][m->get_pos().x - 1]->get_type() == pc_e) {
                    battle_outcome = battle();

                    if (battle_outcome == 1) { 
                        map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                        random_turn(map, m);
                    } else if (battle_outcome == -1) { 
                        m->set_next_turn(-1);
                    }
                } else {
                    random_turn(map, m);
                }
            } else {
                random_turn(map, m);
            }
        }

        if (m->get_next_turn() > -1) {
            m->set_next_turn(m->get_next_turn() + path::calculate_terrain_cost(map->terrain_map[m->get_pos().y][m->get_pos().x], m->get_type()));
        }
    }
}

void action::move_pacer(map *map, npc *p)
{
    terrain_e current_terrain;
    int battle_outcome;
    int moved;

    current_terrain = map->terrain_map[p->get_pos().y][p->get_pos().x];
    moved = 0;
    if (p->get_dir().x >= 0 && map->terrain_map[p->get_pos().y][p->get_pos().x + 1] == current_terrain) {
        if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1] == nullptr) {
            moved = 1;
            p->set_dir_x(1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = nullptr;
            p->set_pos_x(p->get_pos().x + 1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = p;
        } else if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1]->get_type() == pc_e) {
            battle_outcome = battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
            } else if (battle_outcome == -1) { 
                p->set_next_turn(-1);
            }
        }
    } else if (p->get_dir().x <= 0 && map->terrain_map[p->get_pos().y][p->get_pos().x - 1] == current_terrain) {
        if (map->trainer_map[p->get_pos().y    ][p->get_pos().x - 1] == nullptr) {
            moved = 1;
            p->set_dir_x(-1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = nullptr;
            p->set_pos_x(p->get_pos().x - 1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = p;
        } else if (map->trainer_map[p->get_pos().y    ][p->get_pos().x - 1]->get_type() == pc_e) {
            battle_outcome = battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
            } else if (battle_outcome == -1) { 
                p->set_next_turn(-1);
            }
        }
    }
    if (!moved && p->get_dir().x == -1 && map->terrain_map[p->get_pos().y][p->get_pos().x + 1] == current_terrain) {
        p->set_dir_x(1);
        if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1] == nullptr) {
            moved = 1;
            map->trainer_map[p->get_pos().y][p->get_pos().x] = nullptr;
            p->set_pos_x(p->get_pos().x + 1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = p;
        } else if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1]->get_type() == pc_e) {
            battle_outcome = battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
            } else if (battle_outcome == -1) { 
                p->set_next_turn(-1);
            }
        }
    }
    if (!moved && p->get_dir().x == 1 && map->terrain_map[p->get_pos().y][p->get_pos().x - 1] == current_terrain) { 
        p->set_dir_x(-1);
        if (map->trainer_map[p->get_pos().y    ][p->get_pos().x - 1] == nullptr) {
            moved = 1;
            map->trainer_map[p->get_pos().y][p->get_pos().x] = nullptr;
            p->set_pos_x(p->get_pos().x - 1);
            map->trainer_map[p->get_pos().y][p->get_pos().x] = p;
        } else if (map->trainer_map[p->get_pos().y    ][p->get_pos().x - 1]->get_type() == pc_e) {
            battle_outcome = battle();

            if (battle_outcome == 1) { 
                map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
            } else if (battle_outcome == -1) { 
                p->set_next_turn(-1);
            }
        } else {
            if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1] == nullptr) {
                p->set_dir_x(1);
                moved = 1;
                map->trainer_map[p->get_pos().y][p->get_pos().x] = nullptr;
                p->set_pos_x(p->get_pos().x + 1);
                map->trainer_map[p->get_pos().y][p->get_pos().x] = p;
            } else if (map->trainer_map[p->get_pos().y    ][p->get_pos().x + 1]->get_type() == pc_e) {
                battle_outcome = battle();

                if (battle_outcome == 1) { 
                    map->trainer_map[map->get_pc_pos().y][map->get_pc_pos().x]->set_next_turn(-1);
                } else if (battle_outcome == -1) { 
                    p->set_next_turn(-1);
                }
            }
        }
    }

    if (p->get_next_turn() > -1) {
        p->set_next_turn(p->get_next_turn() + path::calculate_terrain_cost(map->terrain_map[p->get_pos().y][p->get_pos().x], p->get_type()));
    }
}

class world {
    private:
        map *current_map;
        coordinate_t location;

        int num_trainers;
    public:
        map *board[WORLD_HEIGHT][WORLD_WIDTH];
        path hiker_path[MAP_HEIGHT][MAP_WIDTH];
        path rival_path[MAP_HEIGHT][MAP_WIDTH];
        explicit world(int num_t) : location(), board()
        {
            int x, y;
            coordinate_t tmp;
            for (y = 0; y < WORLD_HEIGHT; y++) {
                for (x = 0; x < WORLD_WIDTH; x++) {
                    board[y][x] = nullptr;
                }
            }

            for (y = 0; y < MAP_HEIGHT; y++) {
                for (x = 0; x < MAP_WIDTH; x++) {
                    tmp.y = y;
                    tmp.x = x;
                    hiker_path[y][x].set_heap_node(nullptr);
                    hiker_path[y][x].set_coordinate(tmp);
                    hiker_path[y][x].set_terrain(edge);
                    hiker_path[y][x].set_terrain_cost(INT_MAX);
                    hiker_path[y][x].set_cost(INT_MAX);

                    rival_path[y][x].set_heap_node(nullptr);
                    rival_path[y][x].set_coordinate(tmp);
                    rival_path[y][x].set_terrain(edge);
                    rival_path[y][x].set_terrain_cost(INT_MAX);
                    rival_path[y][x].set_cost(INT_MAX);
                }
            }
            location.x = START_X;
            location.y = START_Y;
            num_trainers = num_t;

            board[START_Y][START_X] = new map();
            current_map = board[START_Y][START_X];
        }
        ~world()
        {
            int x, y;

            for (y = 0; y < WORLD_HEIGHT; y++) {
                for (x = 0; x < WORLD_WIDTH; x++) {
                    if (board[y][x] != nullptr) {
                        board[y][x]->~map();
                    }
                }
            }
        }

        map *get_current_map() { return current_map; }
        void set_current_map(map *m) { current_map = m; }
        coordinate_t get_location() { return location; }
        void set_location_y(int y) { location.y = y; }
        void set_location_x(int x) { location.y = x; }
        int get_num_trainers() const { return num_trainers; }
};



static int32_t path_cmp(const void *key, const void *with) { 
    return ((path *) key)->get_cost() - ((path *) with)->get_cost();
}

chtype view[MAP_HEIGHT][MAP_WIDTH];

void init_terminal(void)
{
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLUE);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
}

void init_view()
{
    int x, y;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            view[y][x] = ' ';
        }
    }
}

void render_view(map *map, coordinate_t location)
{
    int y, x;
    int color;

    clear();

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            switch (map->terrain_map[y][x])
            {
                case clearing:
                    view[y][x] = '.';
                    color = COLOR_GREEN;
                    break;
                case grass:
                    view[y][x] = ':';
                    color = COLOR_GREEN;
                    break;
                case boulder:
                case edge:
                    view[y][x] = '%';
                    color = COLOR_WHITE;
                    break;
                case tree:
                    view[y][x] = '^';
                    color = COLOR_MAGENTA;
                    break;
                case water:
                    view[y][x] = '~';
                    color = COLOR_BLUE;
                    break;
                case road:
                case gate:
                case bridge:
                    view[y][x] = '#';
                    color = COLOR_YELLOW;
                    break;
                case pokemart:
                    view[y][x] = 'M';
                    color = COLOR_CYAN;
                    break;
                case pokecenter:
                    view[y][x] = 'C';
                    color = COLOR_CYAN;
                    break;
            }

            if  (map->trainer_map[y][x] != nullptr) {
                color = COLOR_RED;
                switch (map->trainer_map[y][x]->get_type()) {
                    case pc_e:
                        view[y][x] = '@';
                        break;
                    case hiker_e:
                        view[y][x] = 'h';
                        break;
                    case rival_e:
                        view[y][x] = 'r';
                        break;
                    case pacer_e:
                        view[y][x] = 'p';
                        break;
                    case wanderer_e:
                        view[y][x] = 'w';
                        break;
                    case sentry_e:
                        view[y][x] = 's';
                        break;
                    case explorer_e:
                        view[y][x] = 'e';
                        break;
                    case swimmer_e:
                        view[y][x] = 'm';
                        break;
                }
            }

            attron(COLOR_PAIR(color));
            mvaddch(1 + y, x, view[y][x]);
            attroff(COLOR_PAIR(color));
        }
    }

    mvprintw(22, 1, "(%d, %d)", location.x - START_X, START_Y - location.y);

    refresh();
}

void place_gates(world *world)
{
    if (world->get_location().y == 0) { 
        world->get_current_map()->set_n(-1);
    } else if (world->get_location().y == WORLD_HEIGHT - 1) {
        world->get_current_map()->set_s(-1);
    } 
    
    if (world->get_location().x == 0) { 
        world->get_current_map()->set_w(-1);
    } else if (world->get_location().x == WORLD_WIDTH - 1) {
        world->get_current_map()->set_e(-1);
    }
    if (world->get_location().y > 0 && world->board[world->get_location().y - 1][world->get_location().x] != nullptr) {
        world->get_current_map()->set_n(world->board[world->get_location().y - 1][world->get_location().x]->get_s());
    }
    if (world->get_location().y < WORLD_HEIGHT - 1 && world->board[world->get_location().y + 1][world->get_location().x] != nullptr) {
        world->get_current_map()->set_s(world->board[world->get_location().y + 1][world->get_location().x]->get_n());
    } 
    if (world->get_location().x > 0 && world->board[world->get_location().y][world->get_location().x - 1] != nullptr) {
        world->get_current_map()->set_w(world->board[world->get_location().y][world->get_location().x - 1]->get_e());
    }

    if (world->get_location().x < WORLD_WIDTH - 1 && world->board[world->get_location().y][world->get_location().x + 1] != nullptr) {
        world->get_current_map()->set_e(world->board[world->get_location().y][world->get_location().x + 1]->get_w());
    }
}

void change_map(world *world, pc *pc, char new_map)
{
    int fly_x, fly_y;
    int manhattan_distance;

    world->get_current_map()->trainer_map[pc->get_pos().y][pc->get_pos().x] = nullptr;
    world->get_current_map()->set_pc_turn(pc->get_next_turn());
    
    if (new_map == 'n' && world->get_location().y > 0) {
        world->set_location_y(world->get_location().y - 1);
        pc->set_pos_y(MAP_HEIGHT - 2);
    } else if (new_map == 's' && world->get_location().y < WORLD_HEIGHT - 1) {
        world->set_location_y(world->get_location().y + 1);
        pc->set_pos_y(1);
    } else if (new_map == 'w' && world->get_location().x > 0) {
        world->set_location_x(world->get_location().x - 1);
        pc->set_pos_x(MAP_WIDTH - 2);
    } else if (new_map == 'e' && world->get_location().x < WORLD_WIDTH - 1) {
        world->set_location_x(world->get_location().x + 1);
        pc->set_pos_x(1);
    } else if (new_map == 'f') {
        mvprintw(22, 1, "Fly to: ");
        refresh();
        echo();
        scanw((char *)"%d %d", &fly_x, &fly_y);
        noecho();
        if (fly_x >= -1 * WORLD_WIDTH / 2 && fly_x <= WORLD_WIDTH / 2) {
            world->set_location_x(START_X + fly_x);
        }

        if (fly_y >= -1 * WORLD_HEIGHT / 2 && fly_y <= WORLD_HEIGHT / 2) {
            world->set_location_y(START_Y - fly_y);
        }
    }

    if (world->board[world->get_location().y][world->get_location().x] == nullptr) {
        world->set_current_map(new map());
        world->board[world->get_location().y][world->get_location().x] = world->get_current_map();

        place_gates(world);
        manhattan_distance = abs(world->get_location().x - START_X) + abs(world->get_location().y - START_Y);
        map::generate_map(world->get_current_map(), world->get_current_map()->get_n(), world->get_current_map()->get_s(), world->get_current_map()->get_w(), world->get_current_map()->get_e(), manhattan_distance);

        if (new_map == 'f') {
            map::place_pc(world->get_current_map(), pc);
        }
        map::trainer_map_init(world->get_current_map(), world->get_num_trainers(), pc); 
    } else if (new_map == 'f') {
        world->board[world->get_location().y][world->get_location().x]->trainer_map[pc->get_pos().y][pc->get_pos().x] = nullptr;
        map::place_pc(world->board[world->get_location().y][world->get_location().x], pc);
    }

    world->set_current_map(world->board[world->get_location().y][world->get_location().x]);
    pc->set_next_turn(world->get_current_map()->get_pc_turn());
    world->get_current_map()->trainer_map[pc->get_pos().y][pc->get_pos().x] = pc;
    world->get_current_map()->set_pc_pos(pc->get_pos());
}

int pc_turn(world *world, pc *pc)
{
    int quit_game, valid, key;
    char new_map;

    quit_game = valid = 0;

    while (!valid) {
        key = getch();

        switch (key) {
            case '7':
            case 'y':
            case '8':
            case 'k':
            case '9':
            case 'u':
            case '6':
            case 'l':
            case '3':
            case 'n':
            case '2':
            case 'j':
            case '1':
            case 'b':
            case '4':
            case 'h':
                new_map = action::move_pc(world->get_current_map(), pc, key);
                valid = 1;
                break;
            case 'f':
                new_map = 'f';
                valid = 1;
                break;
            case '5':
            case ' ':
            case '.':
                pc->set_next_turn(pc->get_next_turn() + 15);
                valid = 1;
                break;
            case '>':
                action::enter_building(world->get_current_map(), pc);
                valid = 1;
                break;
            case 't':
                action::trainer_info(world->get_current_map(), world->get_num_trainers());
                render_view(world->get_current_map(), world->get_location());
                valid = 0;
                continue;
            case 'Q':
                quit_game = 1;
                valid = 1;
                break;
            default:
                mvprintw(22, 1, "Use movement keys, info key, or 'Q' to quit.");
                valid = 0;
        }
    }

    if (pc->get_next_turn() < 0) {
        quit_game = 1;
    }

    if (new_map != 0) {
        change_map(world, pc, new_map);
    }

    return quit_game;
}

void game_loop(heap_t *path_heap, world *world)
{
    trainer *t;
    pc *p;
    npc *n;

    int x, y, quit_game;
    path swimmer_path[MAP_HEIGHT][MAP_WIDTH];

    quit_game = 0;

    for (y = 0; y < MAP_HEIGHT; y++) {
        for (x = 0; x < MAP_WIDTH; x++) {
            swimmer_path[y][x] = *new path();
        }
    }

    render_view(world->get_current_map(), world->get_location());

    while (!quit_game) {
        t = (trainer *) heap_remove_min(world->get_current_map()->get_turn_heap());

        if (t->get_type() != pc_e) {
            n = new npc(t);
        }

        switch (t->get_type()) {
            case pc_e:
                p = new pc(t);
                quit_game = pc_turn(world, p);
                t = p;
                break;
            case hiker_e:
                action::move_dijkstra_trainer(path_heap, world->hiker_path, world->get_current_map(), n);
                break;
            case rival_e:
                action::move_dijkstra_trainer(path_heap, world->rival_path, world->get_current_map(), n);
                break;
            case pacer_e:
                action::move_pacer(world->get_current_map(), n);
                break;
            case wanderer_e:
            case explorer_e:
                action::move_wanderer_explorer(world->get_current_map(), n);
                break;
            case sentry_e:
                n->set_next_turn(n->get_next_turn() + 15);
                break;
            case swimmer_e:
                action::move_swimmer(path_heap, swimmer_path, world->get_current_map(), n, world->get_current_map()->get_pc_pos());
                break;
        }
        if (t->get_type() != pc_e) {
            t = n;
        }

        render_view(world->get_current_map(), world->get_location());

        if (t->get_next_turn() > -1) {
            heap_insert(world->get_current_map()->get_turn_heap(), t);
        }

        if (world->get_current_map()->trainer_map[world->get_current_map()->get_pc_pos().y][world->get_current_map()->get_pc_pos().x] != nullptr &&
            world->get_current_map()->trainer_map[world->get_current_map()->get_pc_pos().y][world->get_current_map()->get_pc_pos().x]->get_next_turn() == -1) {
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    int manhattan_distance;
    heap_t path_heap;
    world *w;

    srand(time(nullptr));

    init_terminal();

    init_view();

    heap_init(&path_heap, path_cmp, nullptr);

    if (argc == 3) {
        if (!strcmp(argv[1], "--numtrainers")) {
             w = new world(atoi(argv[2]));
        } else {
            fprintf(stderr, "Usage: \"./play --numtrainers <integer>\" or \"./play\" \n");

            return -1;
        }
    } else if (argc == 1) {
        w = new world(10);
    } else {
        fprintf(stderr, "Usage: \"./play --numtrainers <integer>\" or \"./play\" \n");

        return -1;
    }

    place_gates(w);
    manhattan_distance = abs(w->get_location().x - START_X) + abs(w->get_location().y - START_Y);
    map::generate_map(w->get_current_map(), w->get_current_map()->get_n(), w->get_current_map()->get_s(), w->get_current_map()->get_w(), w->get_current_map()->get_e(), manhattan_distance);
    map::trainer_map_init(w->get_current_map(), w->get_num_trainers(), nullptr); 

    game_loop(&path_heap, w);

    endwin();

    heap_delete(&path_heap);

    return 0;
}



