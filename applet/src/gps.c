#include "gps.h"

#ifdef GPS_TESTING
#include "string.h"
#include <stdio.h>
#else
#include "dmesg.h"
#include "printf.h"
#endif


float degrees_and_decimal_minutes_to_decimal_degrees(int degrees, int minutes, int minuteparts){
  float out = (float)degrees + ((float)minutes+(float)minuteparts/10000)/60;
  return out;
}
#if defined(FW_S13_020)
void gps_dump_dmesg(){
  printf("\ngps_data");
  printhex((char *) &gps_data, 18);
  float lat = degrees_and_decimal_minutes_to_decimal_degrees( gps_data.latdeg, gps_data.latmin, gps_data.latmindec);
  float lon = degrees_and_decimal_minutes_to_decimal_degrees( gps_data.londeg, gps_data.lonmin, gps_data.lonmindec);
  int latint = lat;
  int latrem = (lat - latint)*1000000;
  int lonint = lon;
  int lonrem = (lon - lonint)*1000000;
  // %f appears not implemented?
  printf("\nlat = %d.%d\nlon = %d.%d\nalt = %d",latint,latrem,lonint,lonrem,gps_data.altitude_m);
}
#else
void gps_dump_dmesg(){
  printf("This device does not have GPS.\n");
}
#endif
#ifdef GPS_TESTING
void main(){
  //(gcc gps.c -o gps -DGPS_TESTING; ./gps)
  gps_t test;
  /*char *testdata="\x00\x01\x00\x01\x00\x2a\x2a\x00\x1a\x26\x47\x19\x6a\x03\x03\x00\xb7\x00";*/
  char *testdata="\x00\x01\x00\x01\x00\x2a\x2a\x00\xd8\x24\x47\x19\xc4\x02\x04\x00\x8e\x00";
  memcpy((void *)&test,testdata,18);
  float lat = degrees_and_decimal_minutes_to_decimal_degrees( test.latdeg, test.latmin, test.latmindec);
  float lon = degrees_and_decimal_minutes_to_decimal_degrees( test.londeg, test.lonmin, test.lonmindec);
  printf("lat = %f\nlon = %f\n",lat,lon);
  int latint = lat;
  int latrem = (lat - latint)*1000000;
  int lonint = lon;
  int lonrem = (lon - lonint)*1000000;
  printf("lat = %d.%d\nlon = %d.%d\nalt = %d\n",latint,latrem,lonint,lonrem,test.altitude_m);
  printf("sizeof(gps_t) == %d\n", sizeof(gps_t));

}
#endif
