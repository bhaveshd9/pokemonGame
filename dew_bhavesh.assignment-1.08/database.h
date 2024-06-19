#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>

class PokemonData {
public:
    int id;
    std::string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;
    PokemonData(int id,
            std::string identifier,
            int species_id,
            int height,
            int weight,
            int base_experience,
            int order,
            int is_default) 
    {
        this->id = id;
        this->identifier = identifier;
        this->species_id = species_id;
        this->height = height;
        this->weight = weight;
        this->base_experience = base_experience;
        this->order = order;
        this->is_default = is_default;
    }
};

class PokemonType {
public:
    int pokemon_id;
    int type_id;
    int slot;
    PokemonType(int pokemon_id,
                int type_id,
                int slot)
    {
        this->pokemon_id = pokemon_id;
        this->type_id = type_id;
        this->slot = slot;
    }
};

class TypeName {
public:
    int type_id;
    int local_language_id;
    std::string name;
    TypeName(int type_id,
             int local_language_id,
             std::string name)
    {
        this->type_id = type_id;
        this->local_language_id = local_language_id;
        this->name = name;
    }
};

class PokemonSpecies {
public:
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
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;
    PokemonSpecies(int id, 
                   std::string identifier, 
                   int generation_id,
                   int evolves_from_species_id,
                   int evolution_chain_id,
                   int color_id,
                   int shape_id,
                   int habitat_id,
                   int gender_rate,
                   int capture_rate,
                   int base_happiness,
                   int is_baby,
                   int hatch_counter,
                   int has_gender_differences,
                   int growth_rate_id,
                   int forms_switchable,
                   int is_legendary,
                   int is_mythical,
                   int order,
                   int conquest_order)
    {
        this->id = id;
        this->identifier = identifier;
        this->generation_id = generation_id;
        this->evolves_from_species_id = evolves_from_species_id;
        this->evolution_chain_id = evolution_chain_id;
        this->color_id = color_id;
        this->shape_id = shape_id;
        this->habitat_id = habitat_id;
        this->gender_rate = gender_rate;
        this->capture_rate = capture_rate;
        this->base_happiness = base_happiness;
        this->is_baby = is_baby;
        this->hatch_counter = hatch_counter;
        this->has_gender_differences = has_gender_differences;
        this->growth_rate_id = growth_rate_id;
        this->forms_switchable = forms_switchable;
        this->is_legendary = is_legendary;
        this->is_mythical = is_mythical;
        this->order = order;
        this->conquest_order = conquest_order;
    }
};

class PokemonStat {
public:
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;
    PokemonStat(int pokemon_id,
                int stat_id,
                int base_stat,
                int effort)
    {
        this->pokemon_id = pokemon_id;
        this->stat_id = stat_id;
        this->base_stat = base_stat;
        this->effort = effort;
    }
};

class Stat {
public:
    int id;
    int damage_class_id;
    std::string identifier;
    int is_battle_only;
    int game_index;
    Stat(int id,
         int damage_class_id,
         std::string identifier,
         int is_battle_only,
         int game_index)
    {
        this->id = id;
        this->damage_class_id = damage_class_id;
        this->identifier = identifier;
        this->is_battle_only = is_battle_only;
        this->game_index = game_index;
    }
};

class PokemonMove {
public:
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;
    PokemonMove(int pokemon_id,
                int version_group_id,
                int move_id,
                int pokemon_move_method_id,
                int level,
                int order)
    {
        this->pokemon_id = pokemon_id;
        this->version_group_id = version_group_id;
        this->move_id = move_id;
        this->pokemon_move_method_id = pokemon_move_method_id;
        this->level = level;
        this->order = order;
    }
};

class Move {
public:
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
    Move(int id,
         std::string identifier,
         int generation_id,
         int type_id,
         int power,
         int pp,
         int accuracy,
         int priority,
         int target_id,
         int damage_class_id,
         int effect_id,
         int effect_chance,
         int contest_type_id,
         int contest_effect_id,
         int super_contest_effect_id)
    {
        this->id = id;
        this->identifier = identifier;
        this->generation_id = generation_id;
        this->type_id = type_id;
        this->power = power;
        this->pp = pp;
        this->accuracy = accuracy;
        this->priority = priority;
        this->target_id = target_id;
        this->damage_class_id = damage_class_id;
        this->effect_id = effect_id;
        this->effect_chance = effect_chance;
        this->contest_type_id = contest_type_id;
        this->contest_effect_id = contest_effect_id;
        this->super_contest_effect_id = super_contest_effect_id;
    }
};

class Experience {
public:
    int growth_rate_id;
    int level;
    int experience;
    Experience(int growth_rate_id,
               int level,
               int experience)
    {
        this->growth_rate_id = growth_rate_id;
        this->level = level;
        this->experience = experience;
    }
};

class Data {
public:
    std::vector<PokemonData> pokemon_data;
    std::vector<PokemonType> pokemon_types;
    std::vector<TypeName> type_names;
    std::vector<PokemonSpecies> pokemon_species;
    std::vector<PokemonStat> pokemon_stats;
    std::vector<Stat> stats;
    std::vector<PokemonMove> pokemon_moves;
    std::vector<Move> moves;
    std::vector<Experience> experience;
    int get_data(Data &data);
    int get_pokemon_data(Data &data);
    int get_pokemon_types(Data &data);
    int get_type_names(Data &data);
    int get_pokemon_species(Data &data);
    int get_pokemon_stats(Data &data);
    int get_stats(Data &data);
    int get_pokemon_moves(Data &data);
    int get_moves(Data &data);
    int get_experience(Data &data);
    ~Data() 
    {
        pokemon_data.clear();
        pokemon_types.clear();
        type_names.clear();
        pokemon_species.clear();
        pokemon_stats.clear();
        stats.clear();
        pokemon_moves.clear();
        moves.clear();
        experience.clear();
    };
};

#endif
