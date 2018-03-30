#define MAIDENHEAD_TESTING
#define LOUD

#include "maidenhead.h"
#include "math.h"
#include "string.h"

#ifdef MAIDENHEAD_TESTING
#include <stdio.h>
#include <stdlib.h>
#endif

latlon maidenhead_locator_to_latlon( char * loc ){
    latlon returnthis;

    returnthis.lat = 0;
    returnthis.lon = 0;

    return returnthis;
}
void maidenheadgriddiv(float thing, float maxthingval, int maxprecision, char * out ){
    //expects "out" to be 2*maxprecision in size
    //determines whether this thing is lat or lon based on "maxthingval"
    //supports extended arbitrary precision by continuing the 10, 24 pattern
    int div;
    char c;
    int ifwearelat = maxthingval==180;
    for(int i =0; i < maxprecision; i++ ){
        int offset;
        int t;
        if(i==0){//first (field) is divisions of 18 with a capital letter
            div=18;
            c='A';
        }else if(i%2==1){//squares and every odd precision level are divisions of 10 
            div=10;
            c='0';
        } else { //and every even level that isn't 0 is divisions of 24
            div=24;
            c='a';//could be capital, but it's becoming more common to see these lowercase.
        }
        maxthingval /= div;
        t = thing/maxthingval;
        thing -= t*maxthingval;
        offset = ifwearelat? i*2+1: i*2;
        out[offset] = t + c;
    }
    return;
}
void latlon_to_maidenhead_locator( latlon in, char * maidenhead_out, int precision ){
    float lon = in.lon + 180;
    float lat = in.lat + 90;
    maidenheadgriddiv(lon, 360, precision ,maidenhead_out);
    maidenheadgriddiv(lat, 180, precision ,maidenhead_out);
    return;
}
float distance_between_maidenhead_locators_in_km(char*a,char*b){
}
float distance_between_maidenhead_locators_in_subsquares(char*a,char*b){
    //only supports down to subsquares
    /*
    -------------------------------------------
    |             |             |             |
    |   FN42ab    |   FN42bb    |   FN42cb    |
    |             |             |             |
    |-------------|-------------|-------------|
    |             |             |             |
    |   FN42aa    |   FN42ba    |   FN42ca    |
    |             |             |             |
    -------------------------------------------
    */
    //find x distance
    //find y distance
    //return sqrt( x**2 + y**2 )
    //
    // assume six character maidenhead max
    /*
    The first pair (a field) encodes with base 18 and the letters "A" to "R".
        18 zones of longitude of 20° each, and 18 zones of latitude 10° each
    The second pair (square) encodes with base 10 and the digits "0" to "9".
        1° of latitude by 2° of longitude
    The third pair (subsquare) encodes with base 24 and the letters "a" to "x".
        2.5' of latitude by 5' of longitude
    The fourth pair (extended square) encodes with base 10 and the digits "0" to "9". (we don't use this (yet?))
    */
    int scale[]={240,24,1};
    int lonsubsquarediff=0;
    int latsubsquarediff=0;
    for( int i =0; i < 6; i++){ //6 because 6 characters, 3 levels
        int chardiff = b[i] - a[i]; //so a->c should be positive 'motion'
        int subsquare_diff = chardiff * scale[i/2]; 
        if( i % 2 == 0 ){ // a[0,2,4] are the lon
            lonsubsquarediff += subsquare_diff;
        } else {// a[1,3,5] are the lat
            latsubsquarediff += subsquare_diff;
        }
    }
    if( lonsubsquarediff >= 2160 ){
        lonsubsquarediff -= 4320; //double check this in morning
    }
    return sqrt( (float)latsubsquarediff*latsubsquarediff + (float)lonsubsquarediff*lonsubsquarediff);

}
int maidenhead_locators_are_adjacent( char *a, char *b){
    return distance_between_maidenhead_locators_in_subsquares(a,b) < 2; 
    //includes diagonals: if we dont want diagonals change to == 1
}

#ifdef MAIDENHEAD_TESTING
int test_maidenhead_distances(char * a,char * b,float expected_subsquare_distance){
    int errors = 0;
    float d = distance_between_maidenhead_locators_in_subsquares(a,b);
    if( d != expected_subsquare_distance ){
        printf("\nError in gridsubsquare distance calculation for %s and %s\n\tGot %f but expected %f\n",a,b,d,expected_subsquare_distance);
        errors += 1;
    }
    if( expected_subsquare_distance < 2 && ! maidenhead_locators_are_adjacent(a,b) ){
        printf("Error in gridsubsquare adjacency calculation\n\tGot false but expected true\n");
        errors += 1;
    }
    if( expected_subsquare_distance >= 2 && maidenhead_locators_are_adjacent(a,b) ){
        printf("Error in gridsubsquare adjacency calculation\n\tGot true but expected false\n");
        errors += 1;
    }
#ifdef LOUD
    printf("%s to %s: %f subsquares expected %f\n\tadjacent: %s\n",a,b,d,expected_subsquare_distance,maidenhead_locators_are_adjacent(a,b)?"true":"false");
#endif
    return errors;
}
int test_latlon_to_maidenhead_locator(latlon in, char * expected_maidenhead ){
    int errors = 0;
    char * out = malloc(strlen(expected_maidenhead) +1 );
    int levels = strlen(expected_maidenhead)/2;
    latlon_to_maidenhead_locator( in, out, levels );
    if( strlen(out) != 2*levels ){
        printf("Incorrect precision for latlon to maidenhead where expected_maidenhead == %s\n"
                "\tGot %s\n",expected_maidenhead,out);
        errors++;
    }
    if( strncmp(out,expected_maidenhead, levels*2) != 0 ){
        printf("Bad maidenhead out for latlon to maidenhead where expected_maidenhead == %s\n",expected_maidenhead);
        errors++;
    }
#ifdef LOUD
    printf("latlon to maidenhead: %f,%f -> %s, expected %s\n",in.lat,in.lon,out,expected_maidenhead);
#endif
    free(out);
    return errors;
}
void test(){
    int errors = 0;
    errors += test_maidenhead_distances( "FN42aa", "FN42ab", 1);
    errors += test_maidenhead_distances( "FN42aa", "FN42ba", 1);
    errors += test_maidenhead_distances( "FN42aa", "FN42bb", sqrt(2));
    errors += test_maidenhead_distances( "FN42ca", "FN42aa", 2);
    errors += test_maidenhead_distances( "FN43aa", "FN42aa", 24);
    errors += test_maidenhead_distances( "FN42aa", "FN42aa", 0);

    //longitude wrap-around, double check me!
    errors += test_maidenhead_distances( "AA00aa", "IA00aa", 1920); 
    errors += test_maidenhead_distances( "AA00aa", "IA90xa", 2159); 
    errors += test_maidenhead_distances( "AA00aa", "JA00aa", 2160); //dead opposite long, if I'm right
    errors += test_maidenhead_distances( "AA00aa", "JA00ba", 2159); 
    errors += test_maidenhead_distances( "AA00aa", "KA00aa", 1920); 
    errors += test_maidenhead_distances( "AA00aa", "RA90xa", 1); 
    errors += test_maidenhead_distances( "AA00aa", "RA90wa", 2); 

    errors += test_maidenhead_distances( "AA00aa", "AR09ax", 4319); 
        //max diff in latitude, but double check in morning
    
    latlon in;
    in.lat = 0;
    in.lon = 0;
    errors += test_latlon_to_maidenhead_locator(in,"JJ00");
    errors += test_latlon_to_maidenhead_locator(in,"JJ00aa");
    in.lon = -71.32457;
    in.lat = 42.65148;
    errors += test_latlon_to_maidenhead_locator(in,"FN42");
    errors += test_latlon_to_maidenhead_locator(in,"FN42ip");
    errors += test_latlon_to_maidenhead_locator(in,"FN42ip16");
    errors += test_latlon_to_maidenhead_locator(in,"FN42ip16bi");
        
    printf("\nCompleted tests with %d errors.\n",errors);
}

void main(){
    /*
    (gcc maidenhead.c -o maidenhead -lm;./maidenhead)
    */
    test();
}
#endif
