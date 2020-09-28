#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <libpq-fe.h>

void exit_nicely(PGconn* conn) {
	PQfinish(conn);
	exit(1);
}

int main() {
	const char* conninfo;
	PGconn* conn;
	PGresult* res;
	int nFields;

	conninfo = "dbname = test";
	conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		printf("Can't connect to database\n%s\n", PQerrorMessage(conn));
		exit_nicely(conn);
	}

	/* Set always-secure search path, so malicious users can't take control. */
	// res = PQexec(conn,
    //              "SELECT pg_catalog.set_config('search_path', '', false)");
	
	// if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    //     fprintf(stderr, "SET failed: %s", PQerrorMessage(conn));
    //     PQclear(res);
    //     exit_nicely(conn);
    // }
	// PQclear(res);

	
	time_t now;
	time(&now);
	struct tm ts;
	char timestamp_buf[100];

	while (now++) {
		char query[200];

		// time(&now);

		// now += (long int)rand() % 100000000;
		// srand(now);

		ts = *localtime(&now);
		strftime(timestamp_buf, sizeof(timestamp_buf),
				"%Y-%m-%dT%H:%M:%S%Z\n", &ts);
		// printf("TIME: %s", timestamp_buf);
		
		snprintf(query, 200, "INSERT INTO data(station_id, time, payload) VALUES (1, '%s', '{}')", timestamp_buf);
		res = PQexec(conn, query);

		if (PQresultStatus(res) != PGRES_COMMAND_OK)
		{
			fprintf(stderr, "INSERT failed: %s", PQerrorMessage(conn));
			PQclear(res);
			exit_nicely(conn);
		}
		PQclear(res);
	}
	
	return 0;
}
