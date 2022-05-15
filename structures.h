
struct battlelog_player
{
public:
    int id;
    std::string tag;
    std::string player_name;
    int brawler_id;
    std::string brawler_name;
    int brawler_power;
    int brawler_trophies;
};


struct battlelog_3v3
{
public:
    int id;  //in database
    std::string battleTime;
    int event_id; //in brawl stars
    std::string event_mode;
    std::string event_map;

    std::string battle_mode;
    std::string battle_type;
    std::string battle_result;
    int battle_duration;
    int battle_trophy_change;
    int star_player_id;
    battlelog_player players_in_game[2][3]; //players id
};

struct battlelog_solo
{
public:
    int id;  //in database
    std::string battleTime;
    int event_id; //in brawl stars
    std::string event_mode;
    std::string event_map;

    std::string battle_mode;
    std::string battle_type;
    int battle_trophy_change;
    battlelog_player players_in_game[10]; //players id
};

struct battlelog_duo
{
public:
    int id;  //in database
    std::string battleTime;
    int event_id; //in brawl stars
    std::string event_mode;
    std::string event_map;

    std::string battle_mode;
    std::string battle_type;
    battlelog_player teams[5][2]; //players id
};

struct player
{
    std::string tag;
    std::string name;
    std::string last_update;
    std::string name_color;
    int icon_id;
    int trophies;
    int highest_trophies;
    int exp_level;
    int exp_points;
    bool is_qualified;
    int victories_3v3;
    int victories_solo;
    int victories_duo;
    int best_robo_rumble_time;
    int best_time_as_big_brawler;
    std::string club_tag;
    std::string club_name;

};