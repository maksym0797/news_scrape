#ifndef POSTGRES_CLIENT_H
#define POSTGRES_CLIENT_H
#include <libpq-fe.h>
#include <string>
#include "../config/config.h"

class PostgresClient
{
public:
    PostgresClient(Config &config);
    ~PostgresClient();

    PGconn *getDbConnection();

private:
    std::string conninfo_str;
    PGconn *conn;
};

#endif // POSTGRES_CLIENT_H