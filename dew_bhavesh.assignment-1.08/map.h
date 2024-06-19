#ifndef MAP_H
#define MAP_H

#include "heap.h"
#include "trainer.h"

typedef enum {
    grass,
    tall_grass,
    boulder,
    edge,
    tree,
    willow,
    water,
    road,
    pokemart,
    pokecenter,
    gate,
    bridge,
} terrain_e;

class map {
    private:
        int n, s, w, e; // gates

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
                    terrain_map[y][x] = grass;
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

void generate_map(map *map, int n, int s, int w, int e, int manhattan_distance);
void trainer_map_init(map *map, Data *data, int num_trainers, pc *pc, Pokemon *starter, int manhattan_distance);
void place_pc(map *map, pc *pc);
void place_npc(heap_t *turn_heap, map *map, Data *data, trainer_type_e type, int manhattan_distance);

#endif
