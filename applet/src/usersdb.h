
#ifndef _USERSDB_H
#define _USERSDB_H

#define BSIZE 100

typedef struct {
    char buffer[BSIZE];
    char *id ;
    char *callsign ;
    char *firstname ;
    char *name ;
    char *place ;
    char *state ;
    char *country ;
} user_t ;

/**
 * lookup the a user given their ID (dmr_search) in the database.
 * The function returns 1 for success and 0 for "not found".
 */
int usr_find_by_dmrid( user_t *up, int dmrid );

#endif
