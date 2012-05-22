#include "zdb.h"

typedef struct {
	zdb_t *zdb;
	char *filename;
	int writer;
} *IDB;

IDB idb_open (char *filename);
IDB idb_open_reader (char *filename);
void idb_close (IDB idb);
int idb_put (IDB dbf, char *key, char *value);
char *idb_get (IDB dbf, char *key);
int idb_del (IDB dbf, char *key);
