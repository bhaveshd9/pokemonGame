#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>

#include "heap.h"
#include "poke327.h"
#include "io.h"
#include "parsing.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include <cstdio>
#include <cstring>
#include <cstdlib>

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

world_t world;
int emptyCellCheck(std::string value)
{
    if(value.empty())
    {
        return INT_MAX;
    }
    else
    {
        return stoi(value);
    }
}
std::string isIntMax(int value)
{
  if(value == INT_MAX)
  {
    return "";
  }
  else
  {
    return std::to_string(value);
  }
}
void parseTypeNames(std::ifstream &f)
{
  std::string placeHold = "";
   if(f.good())
    {
        int i = 0;
        std::getline(f, placeHold); // here so the first line isn't in the string stream;
      //  std::cout << placeHold << std::endl;
        while(getline(f, placeHold))
        {

      //  std::cout << placeHold << std::endl; // stores the whole line in placeHold
        
        std::stringstream inputStr(placeHold); //parses the line stored in placeHold

        std::getline(inputStr, placeHold, ',');
        world.typeNames[i].type_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.typeNames[i].local_language_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.typeNames[i].name = placeHold;
        
        std::string outputLine = isIntMax(world.typeNames[i].type_id) + "," + isIntMax(world.typeNames[i].local_language_id) + "," + world.typeNames[i].name;
      //  std::cout << outputLine << std::endl; 
        i++;
        }
    }
    else if(f.fail())
    {
        std::cout << "failed" << std::endl;
    }
}
void parsePokemonTypes(std::ifstream &f)
{
   std::string placeHold = "";
   if(f.good())
    {
        int i = 0;
        std::getline(f, placeHold); // here so the first line isn't in the string stream;
      //  std::cout << placeHold << std::endl;
        while(getline(f, placeHold))
        {

      //  std::cout << placeHold << std::endl; // stores the whole line in placeHold
        

        std::stringstream inputStr(placeHold); //parses the line stored in placeHold

        std::getline(inputStr, placeHold, ',');
        world.pokeTypes[i].pokemon_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeTypes[i].type_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeTypes[i].slot = emptyCellCheck(placeHold);

        std::string outputLine = isIntMax(world.pokeTypes[i].pokemon_id) + "," + isIntMax(world.pokeTypes[i].type_id) + "," + isIntMax(world.pokeTypes[i].slot);
      //  std::cout << outputLine << std::endl;
        i++;
        }
        
    }
    else if(f.fail())
    {
        std::cout << "failed" << std::endl;
    }
}
void parseExperience(std::ifstream &f)
{
   std::string placeHold = "";
   if(f.good())
    {
        int i = 0;
        std::getline(f, placeHold); // here so the first line isn't in the string stream;
      //  std::cout << placeHold << std::endl;
        while(getline(f, placeHold))
        {

      //  std::cout << placeHold << std::endl; // stores the whole line in placeHold

        // std::getline(f,placeHold); // stores the whole line in placeHold
        // std::cout << placeHold << std::endl;

        std::stringstream inputStr(placeHold); //parses the line stored in placeHold

        std::getline(inputStr, placeHold, ',');
        world.exp[i].growth_rate = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.exp[i].level = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.exp[i].experience = emptyCellCheck(placeHold);

        std::string outputLine = isIntMax(world.exp[i].growth_rate) + "," + isIntMax(world.exp[i].level) + "," + isIntMax(world.exp[i].experience);
      //  std::cout << outputLine << std::endl;
        i++;
        }
        
    }
    else if(f.fail())
    {
        std::cout << "failed" << std::endl;
    }
}
void parsePokemonStats(std::ifstream &f)
{
   std::string placeHold = "";
   if(f.good())
    {
        int i = 0;
        std::getline(f, placeHold); // here so the first line isn't in the string stream;
      //  std::cout << placeHold << std::endl;
        while(getline(f, placeHold))
        {

      //  std::cout << placeHold << std::endl; // stores the whole line in placeHold

        // std::getline(f,placeHold); // stores the whole line in placeHold
        // std::cout << placeHold << std::endl;

        std::stringstream inputStr(placeHold); //parses the line stored in placeHold

        std::getline(inputStr, placeHold, ',');
        world.pokeStats[i].pokemon_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeStats[i].stat_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeStats[i].base_stat = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeStats[i].effort = emptyCellCheck(placeHold);

        std::string outputLine = isIntMax(world.pokeStats[i].pokemon_id) + "," + isIntMax(world.pokeStats[i].stat_id) + "," + isIntMax(world.pokeStats[i].base_stat) + "," + isIntMax(world.pokeStats[i].effort);
      //  std::cout << outputLine << std::endl; 
        i++;
        }
    }
    else if(f.fail())
    {
        std::cout << "failed" << std::endl;
    }
}
void parseStats(std::ifstream &f)
{
   std::string placeHold = "";
   if(f.good())
    {
        int i = 0;
        std::getline(f, placeHold); // here so the first line isn't in the string stream;
      //  std::cout << placeHold << std::endl;
        while(getline(f, placeHold))
        {

       // std::cout << placeHold << std::endl; // stores the whole line in placeHold

        // std::getline(f,placeHold); // stores the whole line in placeHold
        // std::cout << placeHold << std::endl;

        std::stringstream inputStr(placeHold); //parses the line stored in placeHold

        std::getline(inputStr, placeHold, ',');
        world.stats[i].id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.stats[i].damage_class_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.stats[i].identifier = placeHold;
        std::getline(inputStr, placeHold, ',');
        world.stats[i].is_battle_only = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.stats[i].game_index = emptyCellCheck(placeHold);

        std::string outputLine = isIntMax(world.stats[i].id) + "," + isIntMax(world.stats[i].damage_class_id) + "," + world.stats[i].identifier + "," + isIntMax(world.stats[i].is_battle_only) + "," + isIntMax(world.stats[i].game_index);
      //  std::cout << outputLine << std::endl;
        i++;
        }
        
    }
    else if(f.fail())
    {
        std::cout << "failed" << std::endl;
    }
}
void parsePokemon(std::ifstream &f)
{
   std::string placeHold = "";
   if(f.good())
    {
        int i = 0;
        std::getline(f, placeHold); // here so the first line isn't in the string stream;
      //  std::cout << placeHold << std::endl;
        while(getline(f, placeHold))
        {

       // std::cout << placeHold << std::endl; // stores the whole line in placeHold

        // std::getline(f,placeHold); // stores the whole line in placeHold
        // std::cout << placeHold << std::endl;

        std::stringstream inputStr(placeHold); //parses the line stored in placeHold

        std::getline(inputStr, placeHold, ',');
        world.pokemon[i].id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokemon[i].identifier = placeHold;
        std::getline(inputStr, placeHold, ',');
        world.pokemon[i].species_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokemon[i].height = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokemon[i].weight = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokemon[i].base_experience = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokemon[i].order = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokemon[i].is_default = emptyCellCheck(placeHold);

        std::string outputLine = isIntMax(world.pokemon[i].id) + "," + world.pokemon[i].identifier + "," + isIntMax(world.pokemon[i].species_id) + "," + isIntMax(world.pokemon[i].height) + "," 
                                + isIntMax(world.pokemon[i].weight) + "," + isIntMax(world.pokemon[i].base_experience) + "," + isIntMax(world.pokemon[i].order) + "," + isIntMax(world.pokemon[i].is_default);
      //  std::cout << outputLine << std::endl;
        i++;
        }
        
    }
    else if(f.fail())
    {
        std::cout << "failed" << std::endl;
    }
}
void parsePokemonMoves(std::ifstream &f)
{
   std::string placeHold = "";
   if(f.good())
    {
        int i = 0;
        std::getline(f, placeHold); // here so the first line isn't in the string stream;
      //  std::cout << placeHold << std::endl;
        while(getline(f, placeHold))
        {

       // std::cout << placeHold << std::endl; // stores the whole line in placeHold

        // std::getline(f,placeHold); // stores the whole line in placeHold
        // std::cout << placeHold << std::endl;

        std::stringstream inputStr(placeHold); //parses the line stored in placeHold

        std::getline(inputStr, placeHold, ',');
        world.pokeMoves[i].pokemon_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeMoves[i].version_group_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeMoves[i].move_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeMoves[i].pokemon_move_method_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeMoves[i].level = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeMoves[i].order = emptyCellCheck(placeHold);

        std::string outputLine = isIntMax(world.pokeMoves[i].pokemon_id) + "," + isIntMax(world.pokeMoves[i].version_group_id) + "," + isIntMax(world.pokeMoves[i].move_id) +  "," + isIntMax(world.pokeMoves[i].pokemon_move_method_id) + "," + isIntMax(world.pokeMoves[i].level) +  "," + isIntMax(world.pokeMoves[i].order);
      //  std::cout << outputLine << std::endl;
        i++;
        }
        
    }
    else if(f.fail())
    {
        std::cout << "failed" << std::endl;
    }
}
void parseMoves(std::ifstream &f)
{
   std::string placeHold = "";
   if(f.good())
    {
        int i = 0;
        std::getline(f, placeHold); // here so the first line isn't in the string stream;
      //  std::cout << placeHold << std::endl;
        while(getline(f, placeHold))
        {

      //  std::cout << placeHold << std::endl; // stores the whole line in placeHold

        // std::getline(f,placeHold); // stores the whole line in placeHold
        // std::cout << placeHold << std::endl;

        std::stringstream inputStr(placeHold); //parses the line stored in placeHold

        std::getline(inputStr, placeHold, ',');
        world.moves[i].id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].identifier = placeHold;
        std::getline(inputStr, placeHold, ',');
        world.moves[i].generation_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].type_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].power = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].pp = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].accuracy = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].priority = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].target_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].damage_class_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].effect_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].effect_chance = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].contest_type_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].contest_effect_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.moves[i].super_contest_effect_id = emptyCellCheck(placeHold);

        std::string outputLine = isIntMax(world.moves[i].id) + "," + world.moves[i].identifier + "," + isIntMax(world.moves[i].generation_id) +","+ isIntMax(world.moves[i].type_id) + "," + isIntMax(world.moves[i].power) +","
                                  + isIntMax(world.moves[i].pp) +"," + isIntMax(world.moves[i].accuracy) +"," + isIntMax(world.moves[i].priority) +"," + isIntMax(world.moves[i].target_id) + ","
                                  + isIntMax(world.moves[i].damage_class_id) +"," + isIntMax(world.moves[i].effect_id) + "," + isIntMax(world.moves[i].effect_chance) +"," + isIntMax(world.moves[i].contest_type_id) + ","
                                  + isIntMax(world.moves[i].contest_effect_id) + "," + isIntMax(world.moves[i].super_contest_effect_id);
        
      //  std::cout << outputLine << std::endl;
        i++;
        }
        
    }
    else if(f.fail())
    {
        std::cout << "failed" << std::endl;
    }
}
void parsePokemonSpecies(std::ifstream &f)
{
   std::string placeHold = "";
   if(f.good())
    {
        int i = 0;
        std::getline(f, placeHold); // here so the first line isn't in the string stream;
      //  std::cout << placeHold << std::endl;
        while(getline(f, placeHold))
        {

       // std::cout << placeHold << std::endl; // stores the whole line in placeHold

        // std::getline(f,placeHold); // stores the whole line in placeHold
        // std::cout << placeHold << std::endl;

        std::stringstream inputStr(placeHold); //parses the line stored in placeHold

        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].identifier = placeHold;
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].generation_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].evolves_from_species_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].evolution_chain_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].color_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].shape_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].habitat_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].gender_rate = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].capture_rate = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].base_happiness = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].is_baby = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].hatch_counter = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].has_gender = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].growth_rate_id = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].forms_switchable = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].is_legendary = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].is_mythical = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].order = emptyCellCheck(placeHold);
        std::getline(inputStr, placeHold, ',');
        world.pokeSpecies[i].conquest_order = emptyCellCheck(placeHold);

        std::string outputLine = isIntMax(world.pokeSpecies[i].id) +"," + world.pokeSpecies[i].identifier + "," + isIntMax(world.pokeSpecies[i].generation_id) + "," + isIntMax(world.pokeSpecies[i].evolves_from_species_id) + ","
                                + isIntMax(world.pokeSpecies[i].evolution_chain_id) + "," + isIntMax(world.pokeSpecies[i].color_id) + "," + isIntMax(world.pokeSpecies[i].shape_id) + "," + isIntMax(world.pokeSpecies[i].habitat_id) +","
                                + isIntMax(world.pokeSpecies[i].gender_rate) +"," + isIntMax(world.pokeSpecies[i].capture_rate) +"," + isIntMax(world.pokeSpecies[i].base_happiness) +"," + isIntMax(world.pokeSpecies[i].is_baby) + ","
                                + isIntMax(world.pokeSpecies[i].hatch_counter) +","+ isIntMax(world.pokeSpecies[i].has_gender) +"," + isIntMax(world.pokeSpecies[i].growth_rate_id) +"," + isIntMax(world.pokeSpecies[i].forms_switchable) + ","
                                + isIntMax(world.pokeSpecies[i].is_legendary) +"," + isIntMax(world.pokeSpecies[i].is_mythical) +"," + isIntMax(world.pokeSpecies[i].order) + "," + isIntMax(world.pokeSpecies[i].conquest_order);
      //  std::cout << outputLine << std::endl;
        i++;
        }
        
    }
    else if(f.fail())
    {
        std::cout << "failed" << std::endl;
    }
}
void npcPokemonSpawn(std::vector<WildPokemon> pokemonTeam)
{
  
  WildPokemon p;
  int index = rand() % 1093;
  p.name = world.pokemon[index].identifier;
  p.species_id = world.pokemon[index].species_id;
  determinePokemonLevel(&p);
  determinePokemonMoves(&p);
  determinePokemonStats(&p, world.pokemon[index]);
  determineGenderAndShiny(&p);
  pokemonTeam.push_back(p);

  int chance = rand() % 10 + 1;
  if(chance <= 6)
  {
    int numPokemon = (world.pc.pokemonTeam.size() + 1);
    int counter = 0;
    while(counter < numPokemon)
    {
      index = rand() % 1093;
      p.name = world.pokemon[index].identifier;
      p.species_id = world.pokemon[index].species_id;
      determinePokemonLevel(&p);
      determinePokemonMoves(&p);
      determinePokemonStats(&p, world.pokemon[index]);
      determineGenderAndShiny(&p);
      pokemonTeam.push_back(p);
      counter++;
    }
  }
}
pair_t all_dirs[8] = {
  { -1, -1 },
  { -1,  0 },
  { -1,  1 },
  {  0, -1 },
  {  0,  1 },
  {  1, -1 },
  {  1,  0 },
  {  1,  1 },
};

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

static int32_t edge_penalty(int8_t x, int8_t y)
{
  return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}

static void dijkstra_path(map_t *m, pair_t from, pair_t to)
{
  static path_t path[MAP_Y][MAP_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  int32_t x, y;

  if (!initialized) {
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }

  while ((p = (path_t *) heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        mapxy(x, y) = ter_path;
        heightxy(x, y) = 0;
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1)))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1)))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }
}

static int build_paths(map_t *m)
{
  pair_t from, to;

  /*  printf("%d %d %d %d\n", m->n, m->s, m->e, m->w);*/

  if (m->e != -1 && m->w != -1) {
    from[dim_x] = 1;
    to[dim_x] = MAP_X - 2;
    from[dim_y] = m->w;
    to[dim_y] = m->e;

    dijkstra_path(m, from, to);
  }

  if (m->n != -1 && m->s != -1) {
    from[dim_y] = 1;
    to[dim_y] = MAP_Y - 2;
    from[dim_x] = m->n;
    to[dim_x] = m->s;

    dijkstra_path(m, from, to);
  }

  if (m->e == -1) {
    if (m->s == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->w == -1) {
    if (m->s == -1) {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->n == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->s == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    }

    dijkstra_path(m, from, to);
  }

  return 0;
}

static int gaussian[5][5] = {
  {  1,  4,  7,  4,  1 },
  {  4, 16, 26, 16,  4 },
  {  7, 26, 41, 26,  7 },
  {  4, 16, 26, 16,  4 },
  {  1,  4,  7,  4,  1 }
};

static int smooth_height(map_t *m)
{
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  /*  FILE *out;*/
  uint8_t height[MAP_Y][MAP_X];

  memset(&height, 0, sizeof (height));

  /* Seed with some values */
  for (i = 1; i < 255; i += 20) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (height[y][x]);
    height[y][x] = i;
    if (i == 1) {
      head = tail = (queue_node_t *) malloc(sizeof (*tail));
    } else {
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);
  */
  
  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = height[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1]) {
      height[y - 1][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !height[y][x - 1]) {
      height[y][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1]) {
      height[y + 1][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !height[y - 1][x]) {
      height[y - 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < MAP_Y && !height[y + 1][x]) {
      height[y + 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1]) {
      height[y - 1][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < MAP_X && !height[y][x + 1]) {
      height[y][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1]) {
      height[y + 1][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);

  out = fopen("smoothed.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->height, sizeof (m->height), 1, out);
  fclose(out);
  */

  return 0;
}

static void find_building_location(map_t *m, pair_t p)
{
  do {
    p[dim_x] = rand() % (MAP_X - 3) + 1;
    p[dim_y] = rand() % (MAP_Y - 3) + 1;

    if ((((mapxy(p[dim_x] - 1, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] - 1, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x] + 2, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] + 2, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] - 1) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] - 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] + 2) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 2) == ter_path)))   &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_center))) &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_path)))) {
          break;
    }
  } while (1);
}

static int place_pokemart(map_t *m)
{
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_mart;

  return 0;
}

static int place_center(map_t *m)
{  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_center;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_center;

  return 0;
}

static int map_terrain(map_t *m, int8_t n, int8_t s, int8_t e, int8_t w)
{
  int32_t i, x, y;
  queue_node_t *head, *tail, *tmp;
  //  FILE *out;
  int num_grass, num_clearing, num_mountain, num_forest,num_water, num_total;
  terrain_type_t type;
  int added_current = 0;
  
  num_grass = rand() % 4 + 2;
  num_clearing = rand() % 4 + 2;
  num_mountain = rand() % 2 + 1;
  num_forest = rand() % 2 + 1;
   num_water = rand() % 2 + 1;
  num_total = num_grass + num_clearing + num_mountain + num_forest +num_water;

  memset(&m->map, 0, sizeof (m->map));

  /* Seed with some values */
  for (i = 0; i < num_total; i++) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (m->map[y][x]);
    if (i == 0) {
      type = ter_grass;
    } else if (i == num_grass) {
      type = ter_clearing;
    } else if (i == num_grass + num_clearing) {
      type = ter_mountain;
    } else if (i == num_grass + num_clearing + num_mountain) {
      type = ter_forest;
    } else if (i == num_grass + num_clearing + num_mountain + num_forest) {
      type = ter_water;
    }
    m->map[y][x] = type;
    if (i == 0) {
      head = tail = (queue_node_t *) malloc(sizeof (*tail));
    } else {
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */

  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = m->map[y][x];
    
    if (x - 1 >= 0 && !m->map[y][x - 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x - 1] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x - 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y - 1 >= 0 && !m->map[y - 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y - 1][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y - 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y + 1 < MAP_Y && !m->map[y + 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y + 1][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y + 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (x + 1 < MAP_X && !m->map[y][x + 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x + 1] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x + 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    added_current = 0;
    tmp = head;
    head = head->next;
    free(tmp);
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (y == 0 || y == MAP_Y - 1 ||
          x == 0 || x == MAP_X - 1) {
        mapxy(x, y) = ter_boulder;
      }
    }
  }

  m->n = n;
  m->s = s;
  m->e = e;
  m->w = w;

  if (n != -1) {
    mapxy(n,         0        ) = ter_exit;
    mapxy(n,         1        ) = ter_path;
  }
  if (s != -1) {
    mapxy(s,         MAP_Y - 1) = ter_exit;
    mapxy(s,         MAP_Y - 2) = ter_path;
  }
  if (w != -1) {
    mapxy(0,         w        ) = ter_exit;
    mapxy(1,         w        ) = ter_path;
  }
  if (e != -1) {
    mapxy(MAP_X - 1, e        ) = ter_exit;
    mapxy(MAP_X - 2, e        ) = ter_path;
  }

  return 0;
}

static int place_boulders(map_t *m)
{
  int i;
  int x, y;

  for (i = 0; i < MIN_BOULDERS || rand() % 100 < BOULDER_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_forest && m->map[y][x] != ter_path) {
      m->map[y][x] = ter_boulder;
    }
  }

  return 0;
}

static int place_trees(map_t *m)
{
  int i;
  int x, y;
  
  for (i = 0; i < MIN_TREES || rand() % 100 < TREE_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_mountain && m->map[y][x] != ter_path &&
        m->map[y][x] != ter_water) {
      m->map[y][x] = ter_tree;
    }
  }

  return 0;
}

void rand_pos(pair_t pos)
{
  pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
  pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;
}

void new_hiker()
{
  pair_t pos;
  npc *c;

  do {
    rand_pos(pos);
  } while (world.hiker_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]         ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4            ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_hiker;
  c->mtype = move_hiker;
  c->dir[dim_x] = 0;
  c->dir[dim_y] = 0;
  c->defeated = 0;
  c->symbol = 'h';
  c->next_turn = 0;

  
  give_npc_pokemon(c);

  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;

  //  printf("Hiker at %d,%d\n", pos[dim_x], pos[dim_y]);
}

void new_rival()
{
  pair_t pos;
  npc *c;

  do {
    rand_pos(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.rival_dist[pos[dim_y]][pos[dim_x]] < 0        ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]         ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4            ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_rival;
  c->mtype = move_rival;
  c->dir[dim_x] = 0;
  c->dir[dim_y] = 0;
  c->defeated = 0;
  c->symbol = 'r';
  c->next_turn = 0;
 
  give_npc_pokemon(c);

  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

void new_swimmer()
{
  pair_t pos;
  npc *c;
   
  do {
    rand_pos(pos);
  } while (world.cur_map->map[pos[dim_y]][pos[dim_x]] != ter_water ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_swimmer;
  c->mtype = move_swim;
  c->dir[dim_x] = 0;
  c->dir[dim_y] = 0;
  c->defeated = 0;
  c->symbol = 's';
  c->next_turn = 0;
  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

void new_char_other()
{
  pair_t pos;
  npc *c;

  do {
    rand_pos(pos);
  } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
           world.rival_dist[pos[dim_y]][pos[dim_x]] < 0        ||
           world.cur_map->cmap[pos[dim_y]][pos[dim_x]]         ||
           pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4            ||
           pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c = new npc;
  c->pos[dim_y] = pos[dim_y];
  c->pos[dim_x] = pos[dim_x];
  c->ctype = char_other;
  switch (rand() % 4) {
  case 0:
    c->mtype = move_pace;
    c->symbol = 'p';
    break;
  case 1:
    c->mtype = move_wander;
    c->symbol = 'w';
    break;
  case 2:
    c->mtype = move_sentry;
    c->symbol = 's';
    break;
  case 3:
    c->mtype = move_walk;
    c->symbol = 'n';
    break;
  }
  rand_dir(c->dir);
  c->defeated = 0;
  c->next_turn = 0;
  
  
  give_npc_pokemon(c);

  heap_insert(&world.cur_map->turn, c);
  world.cur_map->cmap[pos[dim_y]][pos[dim_x]] = c;
}

void place_characters()
{
  world.cur_map->num_trainers = 2;

  //Always place a hiker and a rival, then place a random number of others
  new_hiker();
  new_rival();
  do {
    //higher probability of non- hikers and rivals
    switch(rand() % 10) {
    case 0:
      new_hiker();
      break;
    case 1:
     new_rival();
      break;
    default:
      new_char_other();
      break;
    }
    /* Game attempts to continue to place trainers until the probability *
     * roll fails, but if the map is full (or almost full), it's         *
     * impossible (or very difficult) to continue to add, so we abort if *
     * we've tried MAX_TRAINER_TRIES times.                              */
  } while (++world.cur_map->num_trainers < MIN_TRAINERS ||
           ((rand() % 100) < ADD_TRAINER_PROB));
}

void init_pc()
{
  int x, y;

  do {
    x = rand() % (MAP_X - 2) + 1;
    y = rand() % (MAP_Y - 2) + 1;
  } while (world.cur_map->map[y][x] != ter_path);

  world.pc.pos[dim_x] = x;
  world.pc.pos[dim_y] = y;
  world.pc.symbol = '@';

  world.cur_map->cmap[y][x] = &world.pc;
  world.pc.next_turn = 0;

  heap_insert(&world.cur_map->turn, &world.pc);
}

void place_pc()
{
  character *c;

  if (world.pc.pos[dim_x] == 1) {
    world.pc.pos[dim_x] = MAP_X - 2;
  } else if (world.pc.pos[dim_x] == MAP_X - 2) {
    world.pc.pos[dim_x] = 1;
  } else if (world.pc.pos[dim_y] == 1) {
    world.pc.pos[dim_y] = MAP_Y - 2;
  } else if (world.pc.pos[dim_y] == MAP_Y - 2) {
    world.pc.pos[dim_y] = 1;
  }

  world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = &world.pc;

  if ((c = (character *) heap_peek_min(&world.cur_map->turn))) {
    world.pc.next_turn = c->next_turn;
  } else {
    world.pc.next_turn = 0;
  }
}

// New map expects cur_idx to refer to the index to be generated.  If that
// map has already been generated then the only thing this does is set
// cur_map.
int new_map(int teleport)
{
  int d, p;
  int e, w, n, s;
  int x, y;
  
  if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]]) {
    world.cur_map = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]];
    place_pc();

    return 0;
  }

  world.cur_map                                             =
    world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]] =
    (map_t *) malloc(sizeof (*world.cur_map));

  smooth_height(world.cur_map);
  
  if (!world.cur_idx[dim_y]) {
    n = -1;
  } else if (world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]) {
    n = world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]->s;
  } else {
    n = 3 + rand() % (MAP_X - 6);
  }
  if (world.cur_idx[dim_y] == WORLD_SIZE - 1) {
    s = -1;
  } else if (world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]) {
    s = world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]->n;
  } else  {
    s = 3 + rand() % (MAP_X - 6);
  }
  if (!world.cur_idx[dim_x]) {
    w = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]) {
    w = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]->e;
  } else {
    w = 3 + rand() % (MAP_Y - 6);
  }
  if (world.cur_idx[dim_x] == WORLD_SIZE - 1) {
    e = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]) {
    e = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]->w;
  } else {
    e = 3 + rand() % (MAP_Y - 6);
  }
  
  map_terrain(world.cur_map, n, s, e, w);
     
  place_boulders(world.cur_map);
  place_trees(world.cur_map);
  build_paths(world.cur_map);
  d = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
       abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)));
  p = d > 200 ? 5 : (50 - ((45 * d) / 200));
  //  printf("d=%d, p=%d\n", d, p);
  if ((rand() % 100) < p || !d) {
    place_pokemart(world.cur_map);
  }
  if ((rand() % 100) < p || !d) {
    place_center(world.cur_map);
  }

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      world.cur_map->cmap[y][x] = NULL;
    }
  }

  heap_init(&world.cur_map->turn, cmp_char_turns, delete_character);

  if ((world.cur_idx[dim_x] == WORLD_SIZE / 2) &&
      (world.cur_idx[dim_y] == WORLD_SIZE / 2)) {
    init_pc();
  } else {
    place_pc();
  }

  if (teleport) {
    do {
      world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = NULL;
      world.pc.pos[dim_x] = rand_range(1, MAP_X - 2);
      world.pc.pos[dim_y] = rand_range(1, MAP_Y - 2);
    } while (world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] ||
             (move_cost[char_pc][world.cur_map->map[world.pc.pos[dim_y]]
                                                   [world.pc.pos[dim_x]]] ==
              INT_MAX)                                                      ||
             world.rival_dist[world.pc.pos[dim_y]][world.pc.pos[dim_x]] < 0);
    world.cur_map->cmap[world.pc.pos[dim_y]][world.pc.pos[dim_x]] = &world.pc;
  }

  pathfind(world.cur_map);
  
  place_characters();

  return 0;
}

/*
static void print_map()
{
  int x, y;
  int default_reached = 0;

  printf("\n\n\n");

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.cur_map->cmap[y][x]) {
        putchar(world.cur_map->cmap[y][x]->symbol);
      } else {
        switch (world.cur_map->map[y][x]) {
        case ter_boulder:
        case ter_mountain:
          putchar('%');
          break;
        case ter_tree:
        case ter_forest:
          putchar('^');
          break;
        case ter_path:
          putchar('#');
          break;
        case ter_mart:
          putchar('M');
          break;
        case ter_center:
          putchar('C');
          break;
        case ter_grass:
          putchar(':');
          break;
        case ter_clearing:
          putchar('.');
          break;
        default:
          default_reached = 1;
          break;
        }
      }
    }
    putchar('\n');
  }

  if (default_reached) {
    fprintf(stderr, "Default reached in %s\n", __FUNCTION__);
  }
}
*/

// The world is global because of its size, so init_world is parameterless
void init_world()
{
  world.quit = 0;
  world.cur_idx[dim_x] = world.cur_idx[dim_y] = WORLD_SIZE / 2;
  new_map(0);

}

void delete_world()
{
  int x, y;

  //Only correct because current game never leaves the initial map
  //Need to iterate over all maps in 1.05+
  heap_delete(&world.cur_map->turn);

  for (y = 0; y < WORLD_SIZE; y++) {
    for (x = 0; x < WORLD_SIZE; x++) {
      if (world.world[y][x]) {
        free(world.world[y][x]);
        world.world[y][x] = NULL;
      }
    }
  }
}

void print_hiker_dist()
{
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.hiker_dist[y][x] == INT_MAX) {
        printf("   ");
      } else {
        printf(" %5d", world.hiker_dist[y][x]);
      }
    }
    printf("\n");
  }
}

void print_rival_dist()
{
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.rival_dist[y][x] == INT_MAX || world.rival_dist[y][x] < 0) {
        printf("   ");
      } else {
        printf(" %02d", world.rival_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

void leave_map(pair_t d)
{
  if (d[dim_x] == 0) {
    world.cur_idx[dim_x]--;
  } else if (d[dim_y] == 0) {
    world.cur_idx[dim_y]--;
  } else if (d[dim_x] == MAP_X - 1) {
    world.cur_idx[dim_x]++;
  } else {
    world.cur_idx[dim_y]++;
  }
  new_map(0);
}

void game_loop()
{
  character *c;
  pair_t d;
  bool is_pc;

  while (!world.quit) {
    c = (character *) heap_remove_min(&world.cur_map->turn);
    is_pc = dynamic_cast<npc *>(c) == NULL;

    move_func[is_pc ? move_pc : ((npc *) c)->mtype](c, d);

    world.cur_map->cmap[c->pos[dim_y]][c->pos[dim_x]] = NULL;
    if (is_pc && (d[dim_x] == 0 || d[dim_x] == MAP_X - 1 ||
                  d[dim_y] == 0 || d[dim_y] == MAP_Y - 1)) {
      leave_map(d);
      d[dim_x] = c->pos[dim_x];
      d[dim_y] = c->pos[dim_y];
    }
    world.cur_map->cmap[d[dim_y]][d[dim_x]] = c;

    if (is_pc) {
      pathfind(world.cur_map);
    }

    c->next_turn += move_cost[is_pc ? char_pc : ((npc *) c)->ctype]
                             [world.cur_map->map[d[dim_y]][d[dim_x]]];

    c->pos[dim_y] = d[dim_y];
    c->pos[dim_x] = d[dim_x];

    heap_insert(&world.cur_map->turn, c);
  }
}

void usage(char *s)
{
  fprintf(stderr, "Usage: %s [-s|--seed <seed>]\n", s);

  exit(1);
}

int main(int argc, char *argv[])
{
  struct timeval tv;
  uint32_t seed;
  int long_arg;
  int do_seed;
  //  char c;
  //  int x, y;
  int i;

  do_seed = 1;
  
 // std::string base = getenv("HOME") + "/.poke327/pokedex/pokedex/data/csv/";
  
  std::string userInput;
 // std::string file ="/share/cs327/pokedex/pokedex/data/csv/";
  std::string file = "/share/cs327/pokedex/pokedex/data/csv/pokemon.csv";
  std::ifstream f;

  f.open(file);
  parsePokemon(f);
  f.clear();
  f.close();
  
  file = "/share/cs327/pokedex/pokedex/data/csv/moves.csv";
  f.open(file);
  parseMoves(f);
  f.clear();
  f.close();

  file = "/share/cs327/pokedex/pokedex/data/csv/pokemon_moves.csv";
  f.open(file);
  parsePokemonMoves(f);
  f.clear();
  f.close();

  file = "/share/cs327/pokedex/pokedex/data/csv/pokemon_species.csv";
  f.open(file);
  parsePokemonSpecies(f);
  f.clear();
  f.close();

  file = "/share/cs327/pokedex/pokedex/data/csv/experience.csv";
  f.open(file);
  parseExperience(f);
  f.clear();
  f.close();

  file = "/share/cs327/pokedex/pokedex/data/csv/type_names.csv";
  f.open(file);
  parseTypeNames(f);
  f.clear();
  f.close();

  file = "/share/cs327/pokedex/pokedex/data/csv/pokemon_stats.csv";
  f.open(file);
  parsePokemonStats(f);
  f.clear();
  f.close();

  file = "/share/cs327/pokedex/pokedex/data/csv/stats.csv";
  f.open(file);
  parseStats(f);
  f.clear();
  f.close();

  file = "/share/cs327/pokedex/pokedex/data/csv/pokemon_types.csv";
  f.open(file);
  parsePokemonTypes(f);
  f.clear();
  f.close();

  if (argc > 1) {
    for (i = 1, long_arg = 0; i < argc; i++, long_arg = 0) {
      if (argv[i][0] == '-') { /* All switches start with a dash */
        if (argv[i][1] == '-') {
          argv[i]++;    /* Make the argument have a single dash so we can */
          long_arg = 1; /* handle long and short args at the same place.  */
        }
        switch (argv[i][1]) {
        case 's':
          if ((!long_arg && argv[i][2]) ||
              (long_arg && strcmp(argv[i], "-seed")) ||
              argc < ++i + 1 /* No more arguments */ ||
              !sscanf(argv[i], "%u", &seed) /* Argument is not an integer */) {
            usage(argv[0]);
          }
          do_seed = 0;
          break;
        default:
          usage(argv[0]);
        }
      } else { /* No dash */
        usage(argv[0]);
      }
    }
  }

  if (do_seed) {
    /* Allows me to start the game more than once *
     * per second, as opposed to time().          */
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  printf("Using seed: %u\n", seed);
  srand(seed);

  io_init_terminal();
  init_world();
  /* print_hiker_dist(); */
  
  /*
  do {
    print_map();  
    printf("Current position is %d%cx%d%c (%d,%d).  "
           "Enter command: ",
           abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
           abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S',
           world.cur_idx[dim_x] - (WORLD_SIZE / 2),
           world.cur_idx[dim_y] - (WORLD_SIZE / 2));
    scanf(" %c", &c);
    switch (c) {
    case 'n':
      if (world.cur_idx[dim_y]) {
        world.cur_idx[dim_y]--;
        new_map();
      }
      break;
    case 's':
      if (world.cur_idx[dim_y] < WORLD_SIZE - 1) {
        world.cur_idx[dim_y]++;
        new_map();
      }
      break;
    case 'e':
      if (world.cur_idx[dim_x] < WORLD_SIZE - 1) {
        world.cur_idx[dim_x]++;
        new_map();
      }
      break;
    case 'w':
      if (world.cur_idx[dim_x]) {
        world.cur_idx[dim_x]--;
        new_map();
      }
      break;
     case 'q':
      break;
    case 'f':
      scanf(" %d %d", &x, &y);
      if (x >= -(WORLD_SIZE / 2) && x <= WORLD_SIZE / 2 &&
          y >= -(WORLD_SIZE / 2) && y <= WORLD_SIZE / 2) {
        world.cur_idx[dim_x] = x + (WORLD_SIZE / 2);
        world.cur_idx[dim_y] = y + (WORLD_SIZE / 2);
        new_map();
      }
      break;
    case '?':
    case 'h':
      printf("Move with 'e'ast, 'w'est, 'n'orth, 's'outh or 'f'ly x y.\n"
             "Quit with 'q'.  '?' and 'h' print this help message.\n");
      break;
    default:
      fprintf(stderr, "%c: Invalid input.  Enter '?' for help.\n", c);
      break;
    }
  } while (c != 'q');

  */
  give_pc_pokemon();
  
  game_loop();
  
  delete_world();

  io_reset_terminal();
  
  return 0;
}
