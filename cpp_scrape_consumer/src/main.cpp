#include <libpq-fe.h>
#include <iostream>
#include <map>
#include <chrono>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <boost/uuid/uuid.hpp>             // For UUID
#include <boost/uuid/string_generator.hpp> // For string_generator
#include <boost/uuid/uuid_serialize.hpp>   // For string_generator
#include <boost/uuid/uuid_io.hpp>          // For to_string
#include <future>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "reader/input_reader.h"
#include "config/config.h"
#include "http/http_client.h"
#include "db/postgres_client.h"

int testDummyParse(HttpClient webClient, int i, std::string id)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    printf("%d Current time: %s\n", i, std::ctime(&now_time));

    nlohmann::json result = webClient.get("/api/raw_post/id/" + id);

    printf("result message: %s\n", result.at("data").at("content").get<std::string>().c_str());

    return 0;
}

int main()
{
    Config config("../.env");
    config.init();
    std::cout << "DB_CONNINFO: " << config.get("DB_CONNINFO") << std::endl;

    PostgresClient client(config);

    InputReaderDB reader(client.getDbConnection());

    HttpClient webClient(config);

    std::vector<InputPost> posts = reader.read();
    std::string source_ids;

    std::cout << "Hello, World!" << std::endl;

    for (auto &post : posts)
    {
        source_ids += "\"" + boost::uuids::to_string(post.SourceID) + "\",";
    }

    std::cout << source_ids.substr(0, source_ids.size() - 1) << std::endl;

    std::vector<Source> sources = reader.getSourcesByIds(source_ids.substr(0, source_ids.size() - 1));
    std::unordered_map<boost::uuids::uuid, Source> post_source_map;
    for (auto &post : posts)
    {
        auto it = std::find_if(sources.begin(), sources.end(), [post](const Source &source)
                               { return source.ID == post.SourceID; });

        if (it != sources.end())
        {
            post_source_map[post.ID] = *it;
        }
    }
    std::vector<std::future<int>> parsingTasks;
    for (int i = 0; i < posts.size();)
    {
        parsingTasks.clear();
        std::cout << "size: " << std::min(5, (int)(posts.size() - i)) << " " << (int)(posts.size() - i) << std::endl;
        for (int j = 0; j < std::min(5, (int)(posts.size() - i)); j++, i++)
        {
            // printf("Post ID: %d Name: %s\n", i, (posts[i].Title).c_str());
            std::cout << "Post ID: " << boost::uuids::to_string(posts[i].ID) << std::endl;

            parsingTasks.push_back(std::async(std::launch::async, testDummyParse, webClient, i, boost::uuids::to_string(posts[i].ID)));
        }
        for (auto &task : parsingTasks)
        {
            task.wait();
        }
    }

    return 0;
}
