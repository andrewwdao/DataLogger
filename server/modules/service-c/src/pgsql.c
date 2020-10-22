#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <libpq-fe.h>

#include <pgsql.h>

char* reconn_info = NULL;
PGconn* conn = NULL;

void pgsql_close_connection()
{
	if (conn != NULL)
		PQfinish(conn);
}

int pgsql_set_secure_search_path()
{
	/* Set always-secure search path, so malicious users can't take control. */
	PGresult* res = PQexec(conn,
                 		   "SELECT pg_catalog.set_config('search_path', '', false)");
	
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SET failed: %s", PQerrorMessage(conn));
        PQclear(res);
        pgsql_close_connection();

		return PGSQL_STATUS_ERR;
    }
	PQclear(res);
	return PGSQL_STATUS_OK;
}

int pgsql_open_connection(const char* conninfo)
{
	reconn_info = (char*)realloc(reconn_info,sizeof(char)*strlen(conninfo)+1);
	strcpy(reconn_info, conninfo);

	conn = PQconnectdb(conninfo);

	if (PQstatus(conn) != CONNECTION_OK) {
		printf("Can't connect to database\n%s\n", PQerrorMessage(conn));
		pgsql_close_connection();
		conn = NULL;
		return PGSQL_STATUS_ERR;
	}

	return PGSQL_STATUS_OK;
}

void pgsql_recover_connection()
{
	pgsql_close_connection();

	if (pgsql_open_connection(reconn_info) == PGSQL_STATUS_OK) {
		printf("CONNECTION RECOVERED!\n");
	} else {
		printf("CONNECTION COULD NOT RECOVERED!\n");
	}
}

PGresult* pgsql_exec_query(const char* query)
{
	if (PQstatus(conn) != CONNECTION_OK) {
		printf("Connection lost, recovering...\n");
		pgsql_recover_connection();
	}

	if (conn == NULL) return NULL;

	PGresult* res = PQexec(conn, query);

	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		fprintf(stderr, "Exec query failed: %s", PQerrorMessage(conn));
		PQclear(res);
		// pgsql_close_connection();
		
		return NULL;
	}

	return res;
}

int pgsql_exec_insert(char* table, char** cols, char** vals, int n)
{
	int max_buffer_len = 300;
	char query[max_buffer_len];
	char* end_of_buffer = query;

	end_of_buffer += snprintf(end_of_buffer, max_buffer_len - (end_of_buffer - query),
							 "INSERT INTO %s(", table);

	for (int i = 0; i < n; i++)
		end_of_buffer += snprintf(end_of_buffer, max_buffer_len - (end_of_buffer - query),
								  "%s%c", cols[i], (i == n-1 ? ')' : ','));

	end_of_buffer += snprintf(end_of_buffer, max_buffer_len - (end_of_buffer - query),
							 " VALUES (");
	
	for (int i = 0; i < n; i++)
		end_of_buffer += snprintf(end_of_buffer, max_buffer_len - (end_of_buffer - query),
								  "%s%c", vals[i], (i == n-1 ? ')' : ','));

	printf("QUERY: %s\n", query);

	PGresult* res = pgsql_exec_query(query);

	if (res == NULL) {
		printf("Insertion failed!\n");
		return PGSQL_STATUS_ERR;
	}

	PQclear(res);
	return PGSQL_STATUS_OK;
}

void pgsql_spam(long int from)
{
	time(&from);
	struct tm ts;
	char timestamp_buf[100];

	while (from++) {
		char query[200];
		ts = *localtime(&from);
		strftime(timestamp_buf, sizeof(timestamp_buf),
				"%Y-%m-%dT%H:%M:%S%Z\n", &ts);
		
		snprintf(query, 200, "INSERT INTO data(station_id, time, payload) VALUES (1, '%s', '{}')", timestamp_buf);
		pgsql_exec_query(query);
	}
}
