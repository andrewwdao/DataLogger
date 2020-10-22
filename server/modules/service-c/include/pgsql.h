#include <libpq-fe.h>

#define PGSQL_STATUS_OK  0
#define PGSQL_STATUS_ERR 1

int pgsql_open_connection(const char* conninfo);
void pgsql_close_connection();
int pgsql_set_secure_search_path();
PGresult* pgsql_exec_query(const char* query);
int pgsql_exec_insert(char* table, char** cols, char** vals, int n);
void pgsql_spam(long int from);