#ifndef IO_H
# define IO_H
#include <stdlib.h>
#include <iostream>
typedef struct character character_t;
typedef int16_t pair_t[2];
enum pokemonStatus { wild_owned, pc_owned, npc_owned };
void io_init_terminal(void);
void io_reset_terminal(void);
void io_display(void);
void io_handle_input(pair_t dest);
void io_queue_message(const char *format, ...);
void io_battle(character_t *aggressor, character_t *defender);
void give_pc_pokemon();
void give_npc_pokemon(character_t *c);
#endif
