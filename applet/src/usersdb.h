/**
 * lookup the a user given their ID (dmr_search) in the database.
 * The function returns 1 for success and 0 for "not found".
 * outsize refers to the size of the output buffer.
 */

#ifndef _USERSDB_H
#define _USERSDB_H

#define BSIZE 100

extern int find_dmr_user(char *outstr, int dmr_search,
                         const char *data, int outsize);

extern uint8_t get_dmr_user_field(uint8_t field, char *outstr, int dmr_search, int outsize);

#endif
