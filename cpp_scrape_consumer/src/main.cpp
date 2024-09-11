#include <libpq-fe.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <chrono>
#include <vector>
#include <boost/uuid/uuid.hpp>             // For UUID
#include <boost/uuid/string_generator.hpp> // For string_generator
#include <boost/uuid/uuid_serialize.hpp>   // For string_generator
#include <boost/uuid/uuid_io.hpp>          // For to_string

struct Post
{
    boost::uuids::uuid ID;
    std::string Title;
    std::string Description;
    boost::uuids::uuid SourceID;
    std::string Link;
    std::chrono::system_clock::time_point CreatedAt;
    std::chrono::system_clock::time_point UpdatedAt;
};

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
        else if (strcmp(name, "user_id") == 0)
        {
            UserId = boost::uuids::string_generator()(value);
        }
    }
};

using namespace std;

std::map<std::string, std::string> parseEnvFile(const std::string &filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        cerr << "Failed to open file: " << filename << endl;
        return {};
    }

    std::map<std::string, std::string> envMap;
    std::string line;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            envMap[key] = value;
        }
    }

    file.close();

    return envMap;
}
int main()
{
    std::map<std::string, std::string> env = parseEnvFile(".env");
    cout << "DB_CONNINFO: " << env["DB_CONNINFO"] << endl;
    // Get the database connection string from the parsed environment variables
    const char *conninfo = env["DB_CONNINFO"].c_str();
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK)
    {
        cerr << "Connection to database failed: " << PQerrorMessage(conn) << endl;
        PQfinish(conn);
        return 1;
    }

    cout << "Connected to database successfully!" << endl;
    cout << "Hello, World!" << endl;

    PGresult *res;

    res = PQexec(conn, "SELECT * FROM sources");

    int nFields = PQnfields(res);
    int nRows = PQntuples(res);

    cout << "We have " << nRows << " rows and " << nFields << " columns." << endl;
    vector<Source> sources;
    for (int i = 0; i < nRows; i++)
    {
        Source source = {};
        for (int j = 0; j < nFields; j++)
        {
            cout << PQfname(res, j) << ": " << PQgetvalue(res, i, j) << " | " << endl;
            source.setField(PQfname(res, j), PQgetvalue(res, i, j));
        }
        sources.push_back(source);
        cout << endl;
    }

    for (auto source : sources)
    {
        cout << "ID: " << boost::uuids::to_string(source.ID) << endl;
        cout << "Name: " << source.Name << endl;
        cout << "Link: " << source.Link << endl;
        cout << "Source Type ID: " << boost::uuids::to_string(source.SourceTypeID) << endl;
        cout << "User ID: " << boost::uuids::to_string(source.UserId) << endl;
        cout << endl;
    }

    PQclear(res);
    PQfinish(conn);
    return 0;
}
