#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>


typedef struct Pokemon {
    int id;
    std::string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    bool is_default; // 1 is true, 0 is false
}PokemonFile;
typedef struct Moves {
    int id;
    std::string identifier;
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;
}Moves;
typedef struct PokemonMoves {
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;
} PokemonMoves;
typedef struct PokemonSpecies {
    int id;
    std::string identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    bool is_baby;
    int hatch_counter;
    bool has_gender;
    int growth_rate_id;
    int forms_switchable;
    bool is_legendary;
    bool is_mythical;
    int order;
    int conquest_order;
} PokemonSpecies;
typedef struct Experience {
    int growth_rate;
    int level;
    int experience;
} Experience;
typedef struct TypeNames {
    int type_id;
    int local_language_id;
    std::string name;
} TypeNames;
typedef struct PokemonStats {
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;
}PokemonStats;
typedef struct Stats {
    int id;
    int damage_class_id;
    std::string identifier;
    bool is_battle_only;
    int game_index;
} Stats;
typedef struct PokemonTypes {
    int pokemon_id;
    int type_id;
    int slot;
} PokemonTypes;

class WildPokemon {
    public :
    int uniqueID;
    Moves learnedMoves[2];
    int hp;
    int attack;
    int defense;
    int special_attack;
    int special_defense;
    int speed;
    int accuracy;
    int evasion;
    bool shiny;
    std::string gender;
    int level;
    std::string name;
    int species_id;
    int currHP;
    int next_turn_cost;
    int capturedStatus;
    int chosen_move;
    Moves chosen_fighting_move;
};

void determineGenderAndShiny(WildPokemon *p);
void determinePokemonStats(WildPokemon *p, Pokemon pokemonTemp);
void determinePokemonMoves(WildPokemon *p);
void determinePokemonLevel(WildPokemon *p);