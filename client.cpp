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
#include <boost/beast.hpp>
#include "structures.h"

using namespace std;
using namespace pqxx;
using namespace chrono;

constexpr auto KEY = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzUxMiIsImtpZCI6IjI4YTMxOGY3LTAwMDAtYTFlYi03ZmExLTJjNzQzM2M2Y2NhNSJ9.eyJpc3MiOiJzdXBlcmNlbGwiLCJhdWQiOiJzdXBlcmNlbGw6Z2FtZWFwaSIsImp0aSI6IjUxMjg2MDAwLTI5ZGItNDM5Ni1hNWQ2LThiNDcxMjY5MjA2OCIsImlhdCI6MTY0OTY3Mzk1NSwic3ViIjoiZGV2ZWxvcGVyLzMyYWI1YWE1LTQ4Y2YtYjVhMy1jMWQ5LTJlNGY5Mjk1ZDdjOCIsInNjb3BlcyI6WyJicmF3bHN0YXJzIl0sImxpbWl0cyI6W3sidGllciI6ImRldmVsb3Blci9zaWx2ZXIiLCJ0eXBlIjoidGhyb3R0bGluZyJ9LHsiY2lkcnMiOlsiMTc4LjE3Ni43NC4yNDkiXSwidHlwZSI6ImNsaWVudCJ9XX0.iq7epIha0sjE0lUmLAQfNhPKUvACkzf5Wcn0RcZrakfhm342ZCygnng7Fd5nQZmtcRNpKalH4hJE7IPn50VCWg";
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


container GetTags2()
{
    container tags;
    return tags;
}

int connect_to_db()
{
    try
    {
        connection C("postgresql://andrey:123@127.0.0.1:5432/brawl_skills_db");
        if (C.is_open())
        {
            cout << "Opened database successfully: " << C.dbname() << endl;
        }
        else
        {
            cout << "Can't open database" << endl;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        cerr << e.what() << std::endl;
        return 1;
    }
}



container GetTags(string json)
{
    int pos=0;
    string player="";
    container new_players;

    pos=json.find('#');
    while(pos!=-1)
    {
        json=json.substr(pos);
        pos=json.find('"');
        player= "%23"+json.substr(1, pos-1);
        new_players.push_back(player);
        json=json.substr(pos);
        pos=json.find('#');
    }
    return new_players;
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

void ParseJson(string json)
{
    //TODO:Get all fields to new structure with those fields
    //TODO:change return type to new structure
}

void SendToDB()
{
    //TODO:send structure to DB
}

mutex console;


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

        console.lock();

        ofstream json_out;
        json_out.open("/home/andrey/jsons.txt", ios::app);
        json_out << "THREAD " << j << endl << json << endl << endl;
        ++j;
        console.unlock();


        new_players = GetTags(json);

        locker.lock();
        for (auto i: new_players)
            players.push_back(i);
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
    //players.push_back("%23UR2UL8YR");
    thread main_thread(watch_and_get);
    main_thread.join();



/*
    string request = setRequest();
    string json = GetRequest(request);
    players = GetTags(json);

    Create_threads();

    for(auto& i: threadVector)
    {
        i.join();
    }

    connect_to_db();
*/

    return 0;
}