#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <ctime>
#include <mutex>
#include <fstream>
#include <semaphore>
#include <pqxx/pqxx>
#include <json/json.h>
#include "structures.h"

using namespace std;
using namespace pqxx;
using namespace chrono;

constexpr auto KEY = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzUxMiIsImtpZCI6IjI4YTMxOGY3LTAwMDAtYTFlYi03ZmExLTJjNzQzM2M2Y2NhNSJ9.eyJpc3MiOiJzdXBlcmNlbGwiLCJhdWQiOiJzdXBlcmNlbGw6Z2FtZWFwaSIsImp0aSI6IjcxMzA2ZDFjLWIzMWItNDg2OC04MjAxLTA5YTFkMTFhZjAwZCIsImlhdCI6MTY1MjY0MjAxMiwic3ViIjoiZGV2ZWxvcGVyLzMyYWI1YWE1LTQ4Y2YtYjVhMy1jMWQ5LTJlNGY5Mjk1ZDdjOCIsInNjb3BlcyI6WyJicmF3bHN0YXJzIl0sImxpbWl0cyI6W3sidGllciI6ImRldmVsb3Blci9zaWx2ZXIiLCJ0eXBlIjoidGhyb3R0bGluZyJ9LHsiY2lkcnMiOlsiMTc4LjE3Ni43Ny4yNiJdLCJ0eXBlIjoiY2xpZW50In1dfQ.wxhSakggJ9p6dPODmJDZvG2HRcyOh_FyGxGtTgzYhPePwWaaj5J03WJiA6E8Q_mEfX5PCyjrR8fEziDWEY0PGA";
constexpr auto BRAWL_STARS_URL = "https://api.brawlstars.com/v1/players/";
constexpr auto BATTLELOG ="/battlelog";

typedef vector<string> container;

container players;
mutex locker;
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


string setRequest()
{
    string request = BRAWL_STARS_URL;
    string player = "";

    locker.lock();

    player = players.at(players.size() - 1);
    players.pop_back();

    locker.unlock();

    request += player;
    request += BATTLELOG;

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

void tetu(Json::Value battlelogs)
{
    Json::Value battle;
    string mode = "";
    string time = "";


    for (int i = 0; i < battlelogs["items"].size(); ++i)
    {
        battle = battlelogs["items"][i];

        time = battlelogs["items"][i]["battleTime"].asString();
        mode = battle["battle"]["mode"].asString();
        if (mode == "soloShowdown" || mode == "gemGrab" || mode == "heist" || mode == "bounty")
        {
            if (mode == "soloShowdown")
            {
                battlelog_solo log;

                log.battleTime = time;
                log.event_mode = mode;

                log.event_id = battlelogs["items"][i]["event"]["id"].asInt();
                log.event_map = battlelogs["items"][i]["event"]["map"].asString();
                log.battle_mode = battle["battle"]["mode"].asString();
                log.battle_type = battle["battle"]["type"].asString();
                log.battle_trophy_change = battle["battle"]["trophyChange"].asInt();

                for (int player = 0; player < battle["battle"]["players"].size(); ++player)
                {
                    log.players_in_game[player].tag = battle["battle"]["players"][player]["tag"].asString();
                    log.players_in_game[player].player_name = battle["battle"]["players"][player]["name"].asString();
                    log.players_in_game[player].brawler_id = battle["battle"]["players"][player]["brawler"]["id"].asInt();
                    log.players_in_game[player].brawler_name = battle["battle"]["players"][player]["brawler"]["name"].asString();
                    log.players_in_game[player].brawler_power = battle["battle"]["players"][player]["brawler"]["power"].asInt();
                    log.players_in_game[player].brawler_trophies = battle["battle"]["players"][player]["brawler"]["trophies"].asInt();
                }
            }
            else
            {
                battlelog_3v3 log;

                log.battleTime = time;

                log.event_id = battlelogs["items"][i]["event"]["id"].asInt();
                log.event_mode = mode;
                log.event_map = battlelogs["items"][i]["event"]["map"].asString();

                log.battle_mode=mode;

                log.battle_type = battle["battle"]["type"].asString();
                log.battle_result=battle["battle"]["result"].asString();
                log.battle_duration=battle["battle"]["duration"].asInt();
                log.battle_trophy_change = battle["battle"]["trophyChange"].asInt();

                for (int teams=0; teams<battle["battle"]["teams"].size(); ++teams)
                {
                    for (int player=0; player<battle["battle"]["teams"][teams].size(); ++player)
                    {
                        log.players_in_game[teams][player].tag=battle["battle"]["teams"][teams][player]["tag"].asString();
                        log.players_in_game[teams][player].player_name=battle["battle"]["teams"][teams][player]["name"].asString();
                        log.players_in_game[teams][player].brawler_name=battle["battle"]["teams"][teams][player]["brawler"]["name"].asString();
                        log.players_in_game[teams][player].brawler_id=battle["battle"]["teams"][teams][player]["brawler"]["id"].asInt();
                        log.players_in_game[teams][player].brawler_power=battle["battle"]["teams"][teams][player]["brawler"]["power"].asInt();
                        log.players_in_game[teams][player].brawler_trophies=battle["battle"]["teams"][teams][player]["brawler"]["trophies"].asInt();
                    }
                }
                cout<<"";
            }
        }
    }
}
void SendToDB()
{
    //TODO:send structure to DB
}


int j=0;
counting_semaphore<70> url_locker(70);

void thread_task()
{
    string json, url;
    container new_players;

    while (!players.empty())
    {
        url = setRequest();

        url_locker.acquire();
        json = GetRequest(url);
        url_locker.release();



        ParseJson(json);

        locker.lock();

        locker.unlock();
    }
}

void Create_threads()
{
    for (int i=0; i<70; ++i)
    {
        threadVector.emplace_back(thread_task);
    }
};

string create_request_to_db()
{
    string res="SELECT tag "
               "FROM players "
               "WHERE last_update > current_date - interval '10' day "
               "OR "
               "last_update = '11-11-1111 00:00:00'";
    return res;
}

void watch_and_get()
{
    connection C("postgresql://andrey:123@127.0.0.1:5432/brawl_skills_db");
    work quree{C};
    string db_request = create_request_to_db();
    Create_threads();
    //while (true)
    {
        result tags{quree.exec(db_request)};
        for (auto i:tags)
            players.push_back(i[0].c_str());
        for (auto &i:threadVector)
            i.join();
    }
    //

}

int main()
{
    players.push_back("%23pqc2rvvoq");
    //thread main_thread(watch_and_get);
    //main_thread.join();




    string request = setRequest();
    string json = GetRequest(request);
    Json::Value battlelogs = ParseJson(json);
    Json::Value battle;
    tetu(battlelogs);
    //Create_threads();

   /* for(auto& i: threadVector)
    {
        i.join();
    }
    */


    return 0;
}