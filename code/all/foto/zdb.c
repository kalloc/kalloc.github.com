/*

 *  ZDB ver 0.1 beta (the fast key/value database)
 *  Copyright (C) 2006  Denis Slyusarev & ainmarh labs.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "zdb.h"

zdb_t *zdb_malloc()
{
	zdb_t *zdb;
	zdb = (zdb_t *)malloc(sizeof(zdb_t));
	zdb->count = 0;
	return zdb;
}

void zdb_put(zdb_t *zdb, char *key, char *value)
{
	int i;
	size_t lkey,lvalue;

	if (!zdb) return;
	
	lkey = strlen(key)+1;
	lvalue = strlen(value)+1;
	
	if (zdb->count == 0) {
		zdb->count = 1;
		zdb->keys = (char **)malloc(sizeof(char *));
		zdb->keys[0] = (char *)malloc(lkey);
		memcpy(zdb->keys[0],key,lkey);
		zdb->values = (char **)malloc(sizeof(char *));
		zdb->values[0] = (char *)malloc(lvalue);
		memcpy(zdb->values[0],value,lvalue);
		return;
	}

	for (i=0;i<zdb->count;i++) {
		if (!strcmp(zdb->keys[i],key)) {
			free(zdb->values[i]);
			zdb->values[i] = (char *)malloc(lvalue);
			memcpy(zdb->values[i],value,lvalue);
			return;
		}
	}

	zdb->keys = (char **)realloc(zdb->keys,sizeof(char *)*(zdb->count+1));
	zdb->values = (char **)realloc(zdb->values,sizeof(char *)*(zdb->count+1));
	zdb->keys[zdb->count] = (char *)malloc(lkey);
	zdb->values[zdb->count] = (char *)malloc(lvalue);
	memcpy(zdb->keys[zdb->count],key,lkey);
	memcpy(zdb->values[zdb->count],value,lvalue);
	(zdb->count)++;
}

char *zdb_get(zdb_t *zdb, char *key)
{
	int i;

	if (!zdb) return (NULL);
	
	for (i=0;i<zdb->count;i++)
		if (!strcmp(zdb->keys[i],key)) return (zdb->values[i]);
	return NULL;
}

int zdb_del(zdb_t *zdb, char *key)
{
	int i,found = -1;

	if (!zdb) return 0;
	
	for (i=0;i<zdb->count;i++)
		if (!strcmp(zdb->keys[i],key)) {
			found = i;
			break;
		}

	if (found<0) return 0;

	if (found == (zdb->count-1)) {
		free(zdb->keys[zdb->count-1]);
		free(zdb->values[zdb->count-1]);
		zdb->keys = (char **)realloc(zdb->keys,sizeof(char *)*(zdb->count-1));
		zdb->values = (char **)realloc(zdb->values,sizeof(char *)*(zdb->count-1));
		(zdb->count)--;
		return 1;
	}

	free(zdb->keys[found]);
	free(zdb->values[found]);
	zdb->keys[found] = zdb->keys[zdb->count-1];
	zdb->values[found] = zdb->values[zdb->count-1];
	zdb->keys = (char **)realloc(zdb->keys,sizeof(char *)*(zdb->count-1));
	zdb->values = (char **)realloc(zdb->values,sizeof(char *)*(zdb->count-1));
	(zdb->count)--;
	return 1;
}

int zdb_free(zdb_t *zdb)
{
	int i;

	if (!zdb) return 0;
	
	if (zdb->count>0) {
		for (i=0;i<zdb->count;i++) {
			free(zdb->keys[i]);
			free(zdb->values[i]);
		}

		free(zdb->keys);
		free(zdb->values);
	}
	free(zdb);
	zdb = NULL;
	return 1;
}

zdb_t *zdb_read(char *file)
{
	FILE *fl;
	zdb_t *zdb;
	char *buf,*ptr;
	int i;
	struct stat st;
	size_t len;

	fl = fopen(file,"r");
	
	if (!fl) return zdb_malloc();
	fstat(fileno(fl),&st);
	buf = (char *)malloc(st.st_size);
	fread(buf,st.st_size,1,fl);
	fclose(fl);

	zdb = (zdb_t *)malloc(sizeof(zdb_t));
	zdb->count = (int)(*buf);
	if (zdb->count == 0) return zdb;
	
	zdb->keys = malloc(sizeof(char *)*(zdb->count));
	zdb->values = malloc(sizeof(char *)*(zdb->count));
	ptr = buf+(sizeof(int));

	for (i=0;i<zdb->count;i++) {
		len = strlen(ptr)+1;
		zdb->keys[i] = strdup(ptr);ptr+=len;
		len = strlen(ptr)+1;
		zdb->values[i] = strdup(ptr);ptr+=len;
	}
	return (zdb);
}

int zdb_write(zdb_t *zdb, char *file)
{
	FILE *fl;
	int i;
	char path[256];

	snprintf (path,sizeof(path),"%s.tmp",file);

	fl = fopen(path,"w");
	
	if (!fl) return 0;

	fwrite (&(zdb->count),sizeof(int),1,fl);
	for (i=0;i<zdb->count;i++) {
		fwrite (zdb->keys[i],1+strlen(zdb->keys[i]),1,fl);
		fwrite (zdb->values[i],1+strlen(zdb->values[i]),1,fl);
	}
	fclose(fl);
	rename (path,file);
	return 1;
}
