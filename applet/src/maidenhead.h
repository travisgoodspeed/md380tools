#ifndef _MAIDENHEAD_H
#define _MAIDENHEAD_H

typedef struct latlon {
	float lat;
	float lon;
} latlon;
latlon maidenhead_locator_to_latlon( char * loc );
void latlon_to_maidenhead_locator( latlon in, char * maidenhead_out, int precision );
float distance_between_maidenhead_locators_in_subsquares(char*a,char*b);
int maidenhead_locators_are_adjacent( char *a, char *b);

#endif
