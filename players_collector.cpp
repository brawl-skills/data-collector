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

constexpr auto KEY = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzUxMiIsImtpZCI6IjI4YTMxOGY3LTAwMDAtYTFlYi03ZmExLTJjNzQzM2M2Y2NhNSJ9.eyJpc3MiOiJzdXBlcmNlbGwiLCJhdWQiOiJzdXBlcmNlbGw6Z2FtZWFwaSIsImp0aSI6IjBmMWUzOGNmLTNhMzMtNDRkZS05NjNhLTYwMjlmOGFmMTIyMSIsImlhdCI6MTY1Mjg3ODcwNCwic3ViIjoiZGV2ZWxvcGVyLzMyYWI1YWE1LTQ4Y2YtYjVhMy1jMWQ5LTJlNGY5Mjk1ZDdjOCIsInNjb3BlcyI6WyJicmF3bHN0YXJzIl0sImxpbWl0cyI6W3sidGllciI6ImRldmVsb3Blci9zaWx2ZXIiLCJ0eXBlIjoidGhyb3R0bGluZyJ9LHsiY2lkcnMiOlsiMTc4LjE3Ni43Ny40MCJdLCJ0eXBlIjoiY2xpZW50In1dfQ.KQzxN-3peKjtOkK0T0w3XBvuZqQo4IeouZR4BiL8zCdVYrkZ9kASVYCIbupgVrL98zyXGMiJQ_RVX5DCe5f64w";
constexpr auto BRAWL_STARS_URL = "https://api.brawlstars.com/v1/players/";

mutex locker;
vector<thread> threadVector;
int cnt=70;


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


string setRequest(vector<string> players)
{
    string request = BRAWL_STARS_URL;
    string player = "";

    locker.lock();
    if (!players.empty())
    {
        player=players.at(players.size()-1);
    }
    locker.unlock();

    request += player;

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



void set_player_information(player_log log)
{
    connection C("postgresql://andrey:123@localhost:5432/brawl_skills_db");
    work quree{C};
    stringstream ss;
    string res = "";

    ss << "UPDATE players "
          "SET name='"<<log.name << "', last_update_player=CURRENT_DATE, name_color='"<<
          log.name_color<< "', icon_id='" << log.icon_id<<"', trophies='"<<
          log.trophies<< "', highest_trophies='" << log.highest_trophies<<"', exp_level='"<<
          log.exp_level<< "', exp_points='" << log.exp_points<<"', is_qualified_from_championship_challenge='"<<
          log.is_qualified<< "', the_3vs3_victories='" << log.victories_3v3<<"', solo_victories='"<<
          log.victories_solo<< "', duo_victories='" << log.victories_duo<<"', best_robo_rumble_time='"<<
          log.best_robo_rumble_time<< "', best_time_as_big_brawler='" << log.best_time_as_big_brawler<<"', club_tag='"<<
          log.club_tag<< "', club_name='" << log.club_name<<"' "
            "WHERE tag='" << log.tag << "'";
    res=ss.str();

    quree.exec(res);
    quree.commit();
    C.disconnect();
}

void update_time(player_log log)
{
    connection C("postgresql://andrey:123@localhost:5432/brawl_skills_db");
    work quree{C};
    string res= "";
    stringstream ss;

    ss <<"UPDATE players "
         "SET last_update_player=CURRENT_DATE "
         "WHERE tag='"<< log.tag << "'";
    res=ss.str();
    quree.exec(res);
    C.disconnect();
}

void tetu(Json::Value const& player)
{
    player_log log;
    string name;
    string name_utf_8;
    log.tag=player["tag"].asString();

    name = player["name"].asString();
    remove(name.begin(), name.end(), '\'');
    for (char i: name)
        if ((uint)i<0x7F)
            name_utf_8+=i;
    log.name=name_utf_8;
    log.name_color=player["nameColor"].asString();
    log.icon_id= player["icon"]["id"].asInt();
    log.trophies= player["trophies"].asInt();
    log.highest_trophies= player["highestTrophies"].asInt();
    log.exp_level= player["expLevel"].asInt();
    log.exp_points= player["expPoints"].asInt();
    log.is_qualified= player["isQualifiedFromChampionshipChallenge"].asBool();
    log.victories_3v3= player["3vs3Victories"].asInt();
    log.victories_solo= player["soloVictories"].asInt();
    log.victories_duo= player["duoVictories"].asInt();
    log.best_robo_rumble_time= player["bestRoboRumbleTime"].asInt();
    log.best_time_as_big_brawler= player["bestTimeAsBigBrawler"].asInt();

    name=player["club"]["name"].asString();
    remove(name.begin(), name.end(), '\'');

    for (char i: name)
        if ((uint)i<0x7F)
            name_utf_8+=i;
    log.club_name=name_utf_8;

    log.club_tag=player["club"]["tag"].asString();

    set_player_information(log);
    update_time(log);
}


int j=0;
counting_semaphore<35> url_locker(35);


string create_request_to_db(int command)
{
    string res="";

    if (command==1)
    {
        res = "SELECT tag "
              "FROM players "
              "WHERE "
              "last_update_player IS NULL "
              "LIMIT 20";
    }
    return res;
}
mutex iter;
counting_semaphore<24> semaphore(24);
void thread_task()
{
    vector<string> players;
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

        while (!players.empty())
        {
            url = setRequest(players);
            url_locker.acquire();
            json = GetRequest(url);
            url_locker.release();
            players.pop_back();

            value = ParseJson(json);
            semaphore.acquire();
            tetu(value);
            semaphore.release();
        }
    }while(true);
}

void Create_threads()
{
    for (int i=0; i<70; ++i)
    {
        threadVector.emplace_back(thread_task);
    }
}


int main()
{
    Create_threads();
    for (auto &i :threadVector)
      i.join();
      //thread_task();
      /*
    connection C("postgresql://andrey:123@localhost:5432/brawl_skills_db");

    string request = setRequest();
    string json = GetRequest(request);
    Json::Value battlelogs = ParseJson(json);
    tetu(battlelogs);
  */
    return 0;
}
