#include <libpq-fe.h>
#include <iostream>
#include <map>
#include <chrono>
#include <vector>
#include <boost/uuid/uuid.hpp>             // For UUID
#include <boost/uuid/string_generator.hpp> // For string_generator
#include <boost/uuid/uuid_serialize.hpp>   // For string_generator
#include <boost/uuid/uuid_io.hpp>          // For to_string
#include "reader/input_reader.h"
#include "config/config.h"
#include "http/http_client.h"
#include "db/postgres_client.h"

struct Source
{
    boost::uuids::uuid ID;
    std::string Name;
    std::string Link;
    boost::uuids::uuid SourceTypeID;
    boost::uuids::uuid UserId;

    void setField(char *name, char *value)
    {
        if (strcmp(name, "id") == 0)
        {
            ID = boost::uuids::string_generator()(value);
        }
        else if (strcmp(name, "name") == 0)
        {
            Name = value;
        }
        else if (strcmp(name, "link") == 0)
        {
            Link = value;
        }
        else if (strcmp(name, "source_type_id") == 0)
        {
            SourceTypeID = boost::uuids::string_generator()(value);
        }
    }
};

int main()
{
    Config config("../../.env");
    config.init();
    std::cout << "DB_CONNINFO: " << config.get("DB_CONNINFO") << std::endl;

    PostgresClient client(config);

    InputReaderDB reader(client.getDbConnection());

    std::vector<InputPost> posts = reader.read();

    std::cout << "Hello, World!" << std::endl;

    for (auto post : posts)
    {
        std::cout << "ID: " << boost::uuids::to_string(post.ID) << std::endl;
    }

    return 0;
}
