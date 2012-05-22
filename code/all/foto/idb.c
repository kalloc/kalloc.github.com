#include "idb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zdb.h"

#define BLOCK_SIZE 4096

IDB idb_open (char *filename)
{
	IDB idb;

	idb = (IDB)malloc(sizeof *idb);

	idb->zdb = zdb_read(filename);	
	idb->filename = strdup(filename);
	idb->writer = 1;
	return idb;
}

IDB idb_open_reader (char *filename)
{
	IDB idb;

	idb = (IDB)malloc(sizeof *idb);

	idb->zdb = zdb_read(filename);
	idb->writer = 0;
	return idb;
}

void idb_close (IDB idb)
{
	if (idb->writer) {
		zdb_write(idb->zdb,idb->filename);
		free(idb->filename);
	}
	zdb_free(idb->zdb);
}

int idb_put (IDB dbf, char *key, char *value)
{
	zdb_put (dbf->zdb,key,value);
	return 0;
}

char *idb_get (IDB dbf, char *key)
{
	return (zdb_get(dbf->zdb,key));
}

int idb_del (IDB dbf, char *key)
{
	zdb_del(dbf->zdb,key);
	return 0;
}
