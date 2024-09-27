#ifndef INPUT_READER_H
#define INPUT_READER_H

// Include necessary headers
#include <string>
#include <vector>
#include <libpq-fe.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/string_generator.hpp> // For string_generator
#include <boost/uuid/uuid_serialize.hpp>   // For string_generator
#include <boost/uuid/uuid_io.hpp>          // For to_string

struct InputPost
{
    boost::uuids::uuid ID;
    std::string Title;
    std::string Text;
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
    }
};

class InputReader
{
public:
    // Read the input from the given file
    virtual std::vector<InputPost> read() = 0;
};

class InputReaderDB : public InputReader
{
public:
    InputReaderDB(PGconn *conn) : conn(conn)
    {
    }
    std::vector<InputPost> read();
    std::vector<Source> getSourcesByIds(std::string source_ids);
    InputPost hydratePost(PGresult *res, int nFields, int i);

private:
    PGconn *conn;
};

#endif // INPUT_READER_H