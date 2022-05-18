#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <ctime>
#include <mutex>
#include <string>
#include <fstream>
#include <semaphore>
#include <set>
#include <pqxx/pqxx>
#include <json/json.h>
#include "structures.h"

using namespace std;
using namespace pqxx;
using namespace chrono;

constexpr auto KEY = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzUxMiIsImtpZCI6IjI4YTMxOGY3LTAwMDAtYTFlYi03ZmExLTJjNzQzM2M2Y2NhNSJ9.eyJpc3MiOiJzdXBlcmNlbGwiLCJhdWQiOiJzdXBlcmNlbGw6Z2FtZWFwaSIsImp0aSI6IjBmMWUzOGNmLTNhMzMtNDRkZS05NjNhLTYwMjlmOGFmMTIyMSIsImlhdCI6MTY1Mjg3ODcwNCwic3ViIjoiZGV2ZWxvcGVyLzMyYWI1YWE1LTQ4Y2YtYjVhMy1jMWQ5LTJlNGY5Mjk1ZDdjOCIsInNjb3BlcyI6WyJicmF3bHN0YXJzIl0sImxpbWl0cyI6W3sidGllciI6ImRldmVsb3Blci9zaWx2ZXIiLCJ0eXBlIjoidGhyb3R0bGluZyJ9LHsiY2lkcnMiOlsiMTc4LjE3Ni43Ny40MCJdLCJ0eXBlIjoiY2xpZW50In1dfQ.KQzxN-3peKjtOkK0T0w3XBvuZqQo4IeouZR4BiL8zCdVYrkZ9kASVYCIbupgVrL98zyXGMiJQ_RVX5DCe5f64w";
constexpr auto BRAWL_STARS_URL = "https://api.brawlstars.com/v1/players/";
constexpr auto BATTLELOG ="/battlelog";

vector<string> players;
mutex locker;
mutex db;
vector<thread> threadVector;




void Cleanup(CURL *curl, curl_slist *headers)
{
    if (curl)
        curl_easy_cleanup(curl);
    if (headers)
        curl_slist_free_all(headers);
    curl_global_cleanup();
}

/*This is our callback function which will be executed by curl on perform.
here we will copy the data received to our struct
ptr : points to the data received by curl.
size : is the count of data items received.
nmemb : number of bytes
data : our pointer to hold the data.
*/
size_t curl_callback(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*)ptr, size * nmemb);
    return size * nmemb;
}

/*
Send a url from which you want to get the json data
*/
string GetRequest(const string url)
{
    curl_global_init(CURL_GLOBAL_ALL); // sets the program environment
    CURL *curl = curl_easy_init(); // initialize curl
    std::string response_string;
    struct curl_slist *headers = NULL; //linked-list string structure
    if (!curl)
    {
        cout << "ERROR : Curl initialization\n" << endl;
        Cleanup(curl, headers);
        return NULL;
    }
    string header= "Authorization: Bearer ";
    header+=KEY;
    headers = curl_slist_append(headers, header.c_str());
    headers = curl_slist_append(headers, "User-Agent: libcurl-agent/1.0");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Cache-Control: no-cache");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    //ignore ssl verification if you don't have the certificate or don't want secure communication
    // you don't need these two lines if you are dealing with HTTP url
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); //maximum time allowed for operation to finish, its in seconds
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // pass the url
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback); // pass our call back function to handle received data
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string); // pass the variable to hold the received data


    CURLcode status = curl_easy_perform(curl); // execute request
    if (status != 0)
    {
        cout << "Error: Request failed on URL : " << url << endl;
        cout << "Error Code: " << status << " Error Detail : " << curl_easy_strerror(status) << endl;
        Cleanup(curl, headers);
        return NULL;
    }

    // do the clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    curl_global_cleanup();

    return response_string;
}


string setRequest(string& player_tag)
{
    string request = BRAWL_STARS_URL;
    string player = "";

    locker.lock();

    player = players.at(players.size() - 1);
    players.pop_back();

    locker.unlock();

    request += player;
    request += BATTLELOG;

    player_tag=player;

    return request;
}

Json::Value ParseJson(string json)
{
    Json::Value root;
    Json::Reader parser;
    if (!parser.parse(json, root))
    {
        cout<< "Couldn't parse"<< endl;
    }
    return root;


}
void send_request_players_3v3(battlelog_3v3 log)
{
    string res="";
    stringstream ss;
    connection C("postgresql://andrey:123@localhost:5432/brawl_skills_db");
    work quree{C};
    result a;

    for (auto & team : log.players_in_game)
    {
        for (auto & player : team)
        {
            ss.str("");
            res="";
            ss << "INSERT INTO battlelog_players (tag, name, brawler_id, brawler_name, brawler_power, brawler_trophies) "
                  "VALUES ('" << player.tag << "', '" << player.player_name
               << "', '" << player.brawler_id<< "', '" << player.brawler_name
               << "', '" << player.brawler_power << "', '" << player.brawler_trophies
               << "')";
            res= ss.str();
            a = {quree.exec(res)};
        }
    }
    quree.commit();
}

vector<int> get_players_id(battlelog_3v3 &log)
{

    vector<int> ids;
    string res;
    stringstream ss;
    connection C("postgresql://andrey:123@localhost:5432/brawl_skills_db");
    work quree{C};
    result response;

    for (auto & team : log.players_in_game)
    {
        for (auto & player : team)
        {
            ss.str("");
            ss << "SELECT id "
                  "FROM battlelog_players "
                  "WHERE tag='" << player.tag << "' AND"
                                                 " name='" << player.player_name << "' AND"
                                                                                    " brawler_id='" << player.brawler_id
               << "' AND "
                  " brawler_name='" << player.brawler_name << "' AND "
                                                              " brawler_power='" << player.brawler_power << "' AND "
                                                                                                            " brawler_trophies='"
               << player.brawler_trophies << "' ";
            res = ss.str();
            response= {quree.exec(res)};
            for (auto const &row: response)
            {
                for (auto const &field: row)
                {
                    ids.push_back(atoi(field.c_str()));
                }
            }
        }

    }
    return ids;
}

void send_battlelog(battlelog_3v3 &log, vector<int> ids)
{
    string res = "";
    stringstream ss;
    connection C("postgresql://andrey:123@localhost:5432/brawl_skills_db");
    work quree{C};

    ss.clear();
    ss << "INSERT INTO battlelogs "
          "VALUES (DEFAULT, '" << log.battleTime << "', '" << log.event_id << "', '" <<
       log.event_mode << "', '" << log.event_map << "', '" <<
       log.battle_mode << "', '" << log.battle_type << "', '" <<
       log.battle_result << "', '" << log.battle_duration << "', '" <<
       log.battle_trophy_change << "', '{{" <<
       log.teams[0][0] << ", " << log.teams[0][1] << ", " << log.teams[0][2] << "}, {" <<
       log.teams[1][0] << ", " << log.teams[1][1] << ", " << log.teams[1][2] << "}}')";

    res=ss.str();

    //try
    {
        quree.exec(res);
        quree.commit();
    }
    //catch (...)
    {}

}

void setup_new_players(battlelog_3v3 log)
{
    db.lock();
    connection C("postgresql://andrey:123@localhost:5432/brawl_skills_db");
    work quree{C};
    stringstream ss;
    string res = "";
    result response;
    bool contains=false;

    for (auto &team : log.players_in_game)
    {
        for (auto &player : team)
        {
            contains = false;
            ss.str("");
            res = "";

            ss << "SELECT tag "
                  "FROM players "
                  "WHERE tag='" << player.tag << "'";
            res = ss.str();

            response = quree.exec(res);

            for (auto const &row: response)
            {
                for (auto const &field: row)
                {
                    contains = true;
                }
            }

            if (!contains)
            {
                ss.str("");
                res = "";
                ss << "INSERT INTO players (tag, last_update_battlelog) "
                      "VALUES ('" << player.tag << "', '11-11-1111 00:00:00')";
                res = ss.str();
                quree.exec(res);
            }
        }
    }
    quree.commit();
    db.unlock();
}

void update_time(battlelog_3v3 log)
{
    connection C("postgresql://andrey:123@localhost:5432/brawl_skills_db");
    work quree{C};
    string res= "";
    stringstream ss;

    ss <<"UPDATE players "
         "SET last_update_battlelog=CURRENT_DATE "
         "WHERE tag='"<< log.tag << "'";
    res=ss.str();
    quree.exec(res);

}

void tetu(Json::Value battlelogs, battlelog_3v3 log)
{
    Json::Value battle;
    string mode = "";
    vector<int> ids;
    string name="";
    string name_utf8="";
    string time = "";


    for (int i = 0; i < battlelogs["items"].size(); ++i)
    {
        battle = battlelogs["items"][i];

        time = battlelogs["items"][i]["battleTime"].asString();
        mode = battle["battle"]["mode"].asString();
        if (mode == "gemGrab" || mode == "heist" || mode == "bounty" || mode == "brawlBall" || mode == "knockout")
        {


            log.battleTime = time;

            log.event_id = battlelogs["items"][i]["event"]["id"].asInt();
            log.event_mode = mode;
            log.event_map = battlelogs["items"][i]["event"]["map"].asString();
            remove(log.event_map.begin(), log.event_map.end(), '\'');

            log.battle_mode = mode;

            log.battle_type = battle["battle"]["type"].asString();
            log.battle_result = battle["battle"]["result"].asString();
            log.battle_duration = battle["battle"]["duration"].asInt();
            log.battle_trophy_change = battle["battle"]["trophyChange"].asInt();

            for (int teams = 0; teams < battle["battle"]["teams"].size(); ++teams)
            {
                for (int player = 0; player < battle["battle"]["teams"][teams].size(); ++player)
                {
                    name_utf8="";
                    log.players_in_game[teams][player].tag = battle["battle"]["teams"][teams][player]["tag"].asString();
                    name = battle["battle"]["teams"][teams][player]["name"].asString();
                    remove(name.begin(), name.end(), '\'');
                    for (wchar_t ch:name)
                    {
                        if ( uint(ch) < 0x7F)
                            name_utf8 += ch;
                    }
                    log.players_in_game[teams][player].player_name = name_utf8;
                    log.players_in_game[teams][player].brawler_name = battle["battle"]["teams"][teams][player]["brawler"]["name"].asString();
                    log.players_in_game[teams][player].brawler_id = battle["battle"]["teams"][teams][player]["brawler"]["id"].asInt();
                    log.players_in_game[teams][player].brawler_power = battle["battle"]["teams"][teams][player]["brawler"]["power"].asInt();
                    log.players_in_game[teams][player].brawler_trophies = battle["battle"]["teams"][teams][player]["brawler"]["trophies"].asInt();
                }
            }
            send_request_players_3v3(log);
            ids = get_players_id(log);

            for (int i = 0; i < 2; ++i)
                for (int j = 0; j < 3; ++j)
                    log.teams[i][j] = ids.at((i * 2) + j);

            send_battlelog(log, ids);
            setup_new_players(log);
            update_time(log);


        }
    }
}


    int j=0;
    counting_semaphore<70> url_locker(70);


string create_request_to_db(int command)
{
    string res="";

    if (command==1)
    {
        res = "SELECT tag "
              "FROM players "
              "WHERE last_update_battlelog >= NOW() - '1 day'::INTERVAL "
              "OR "
              "last_update_battlelog = '11-11-1111 00:00:00' "
              "OR "
              "last_update_battlelog IS NULL "
              "LIMIT 20";
    }
    return res;
}

void thread_task()
{
    bool first=true;
    connection C("postgresql://andrey:123@localhost:5432/brawl_skills_db");
    work quree{C};
    string text="";
    string db_request = create_request_to_db(1);
    do
    {
        result tags{quree.exec(db_request)};

        for (auto const &row: tags)
        {
            for (auto const &field: row)
            {
                text = field.c_str();
                text="%23"+text.substr(1);
                replace(text.begin(), text.end(), '\'', '\"');
                if (text.size() >= 7)
                    players.push_back(text);
            }
        }

        string json="";
        string url="";
        Json::Value value;
        string tag;

        while (!players.empty())
        {
            url = setRequest(tag);

            url_locker.acquire();
            json = GetRequest(url);
            url_locker.release();
            battlelog_3v3 log;
            log.tag=tag;


            value = ParseJson(json);
            tetu(value, log);
        }
    }while(!players.empty());
}

void Create_threads()
{
    for (int i=0; i<70; ++i)
    {
        threadVector.emplace_back(thread_task);
    }
    for (auto &i:threadVector)
        i.join();

}




int main()
{
    //Create_threads();
    thread_task();
/*
    connection C("postgresql://andrey:123@localhost:5432/brawl_skills_db");

    string request = setRequest();
    string json = GetRequest(request);
    Json::Value battlelogs = ParseJson(json);
    tetu(battlelogs);
*/
    return 0;
}