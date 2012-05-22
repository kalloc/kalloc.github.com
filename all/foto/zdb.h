/*
 *  ZDB ver 0.1 beta (the fast key/value database) [header file]
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

#ifndef __ZDB_HEADER
#define __ZDB_HEADER

typedef struct {
	char **keys;
	char **values;
	int count;
} zdb_t;


// Malloc memory for zdb_t item.
zdb_t *zdb_malloc();

// Put key/value pair (overwrite if exists) in zdb_t item.
void zdb_put(zdb_t *zdb, char *key, char *value);

// Get value from zdb_t item.
char *zdb_get(zdb_t *zdb, char *key);

// Delete key/value pair (if existst) from zdb_t item.
int zdb_del(zdb_t *zdb, char *key);

// Free zdb_t item.
int zdb_free(zdb_t *zdb);

// Read zdb_t item from file.
zdb_t *zdb_read(char *file);

// Write zdb_t item into file.
int zdb_write(zdb_t *zdb, char *file);

#endif // __ZDB_HEADER
