#include "input_reader.h"
#include <vector>
#include <string>
#include <libpq-fe.h>
#include <boost/uuid/uuid.hpp>             // For UUID
#include <boost/uuid/string_generator.hpp> // For string_generator
#include <boost/uuid/uuid_serialize.hpp>   // For string_generator
#include <boost/uuid/uuid_io.hpp>          // For to_string

std::vector<InputPost> InputReaderDB::read()
{
    std::vector<InputPost> posts;
    PGresult *res;

    res = PQexec(conn, "SELECT * FROM posts");

    int nFields = PQnfields(res);
    int nRows = PQntuples(res);

    for (int i = 0; i < nRows; i++)
    {
        posts.push_back(this->hydratePost(res, nFields, i));
    }

    PQclear(res);

    return posts;
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