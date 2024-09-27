#include "input_reader.h"
#include <vector>
#include <string>
#include <iostream>

#include <libpq-fe.h>
#include <boost/uuid/uuid.hpp>             // For UUID
#include <boost/uuid/string_generator.hpp> // For string_generator
#include <boost/uuid/uuid_serialize.hpp>   // For string_generator
#include <boost/uuid/uuid_io.hpp>          // For to_string

std::vector<InputPost> InputReaderDB::read()
{
    std::vector<InputPost> posts;
    PGresult *res;

    res = PQexec(conn, "SELECT * FROM raw_posts");

    int nFields = PQnfields(res);
    int nRows = PQntuples(res);

    for (int i = 0; i < nRows; i++)
    {
        posts.push_back(this->hydratePost(res, nFields, i));
    }

    PQclear(res);

    return posts;
}

std::vector<Source> InputReaderDB::getSourcesByIds(std::string source_ids)
{
    std::vector<Source> sources;
    PGresult *res;

    std::string query = "SELECT * FROM sources WHERE id = ANY($1::uuid[])";
    // Format source_ids as a PostgreSQL array
    std::string formatted_source_ids = "{" + source_ids + "}";
    const char *paramValues[1] = {formatted_source_ids.c_str()};

    res = PQexecParams(conn,
                       query.c_str(),
                       1,    // number of parameters
                       NULL, // parameter types
                       paramValues,
                       NULL, // parameter lengths
                       NULL, // parameter formats
                       0);   // result format

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        PQclear(res);
        throw std::runtime_error("Query failed: " + std::string(PQerrorMessage(conn)));
    }

    int nFields = PQnfields(res);
    int nRows = PQntuples(res);

    for (int i = 0; i < nRows; i++)
    {
        Source source;
        for (int j = 0; j < nFields; j++)
        {
            char *field_name = PQfname(res, j);
            char *value = PQgetvalue(res, i, j);
            source.setField(field_name, value);
        }
        sources.push_back(source);
    }

    PQclear(res);

    return sources;
}

InputPost InputReaderDB::hydratePost(PGresult *res, int nFields, int i)
{
    InputPost post = {};

    for (int j = 0; j < nFields; j++)
    {
        char *field_name = PQfname(res, j);
        char *value = PQgetvalue(res, i, j);
        if (std::strcmp(field_name, "id") == 0)
        {
            post.ID = boost::uuids::string_generator()(value);
        }
        else if (std::strcmp(field_name, "title") == 0)
        {
            post.Title = value;
        }
        else if (std::strcmp(field_name, "text") == 0)
        {
            post.Text = value;
        }
        else if (std::strcmp(field_name, "link") == 0)
        {
            post.Link = value;
        }
        else if (std::strcmp(field_name, "source_id") == 0)
        {
            post.SourceID = boost::uuids::string_generator()(value);
        }
    }

    return post;
}