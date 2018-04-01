#include "gps.h"

#ifdef GPS_TESTING
#include "string.h"
#include <stdio.h>
#else
#include "dmesg.h"
#include "printf.h"
#endif


float degrees_and_decimal_minutes_to_decimal_degrees(int degrees, int minutes, int mindec){
  float min = (float)minutes + (float)mindec/10000;
  /*printf("min: %f\n",min);*/
  /*printf("min/60: %f\n",min/60);*/
  float out = (float)degrees +min/60;
  /*printf("%d* %d' +%d\n",degrees,minutes,mindec);*/
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
  char *testdata="\x00\x01\x00\x01\x00\x0A\x2a\x00\x32\x25\x0A\x19\x87\x02\x06\x00\x4B\x00";
  /*printf("sizeof(gps_t) == %d\n", sizeof(gps_t));*/
  memcpy((void *)&test,testdata,sizeof(gps_t));
  float lat = degrees_and_decimal_minutes_to_decimal_degrees( test.latdeg, test.latmin, test.latmindec);
  float lon = degrees_and_decimal_minutes_to_decimal_degrees( test.londeg, test.lonmin, test.lonmindec);
  printf("Got:    \tlat = %f lon = %f\n",lat,lon);
  printf("Expect~: \tlat = %f lon = %f\n",10.71584833333333,10.41774333333333);
  printf("alt = %d, expect 75\n",test.altitude_m);

}
#endif
