#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sqlite3.h>

const char *result;
static int callback(void *data, int argc, char **argv, char **azColName)
{
  result = argv[1] ? argv[1] : "NULL";
  // std::cout << result << std::endl;
  return 0;
}

const char *db_exec(const char *sql)
{
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  // const char *sql;

  /* Open database */
  rc = sqlite3_open("pirtimer.db", &db);
  if (rc)
  {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    return (0);
  }
  else
  {
    // fprintf(stderr, "Opened database successfully\n");
  }

  // sql = "SELECT * FROM config WHERE title = '" + "timeout" + "'";
  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
  if (rc != SQLITE_OK)
  {
    std::cout << "SQL error: " << sqlite3_errmsg(db) << "\n";
    sqlite3_free(zErrMsg);
  }

  sqlite3_close(db);
  return result;
}