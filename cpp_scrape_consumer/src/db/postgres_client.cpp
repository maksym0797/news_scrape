#include "postgres_client.h"
#include <libpq-fe.h>
#include <iostream>
#include <string>

PostgresClient::PostgresClient(Config &config)
{
    // Get the database connection string from the parsed environment variables
    this->conninfo_str = config.get("DB_CONNINFO");
}

PostgresClient::~PostgresClient()
{
    PQfinish(conn);
}

PGconn *PostgresClient::getDbConnection()
{
    if (this->conn != nullptr)
    {
        return this->conn;
    }
    const char *conninfo = this->conninfo_str.c_str();
    this->conn = PQconnectdb(conninfo);
    if (PQstatus(conn) != CONNECTION_OK)
    {
        std::cerr << "Connection to database failed: " << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        return nullptr;
    }
    return this->conn;
}
