#ifndef INPUT_READER_H
#define INPUT_READER_H

// Include necessary headers
#include <string>
#include <vector>
#include <libpq-fe.h>
#include <boost/uuid/uuid.hpp>

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
    InputPost hydratePost(PGresult *res, int nFields, int i);

private:
    PGconn *conn;
};

#endif // INPUT_READER_H