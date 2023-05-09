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
latlon get_current_position( gps_t gps ){
  latlon here;
  here.lat = degrees_and_decimal_minutes_to_decimal_degrees( gps.latdeg, gps.latmin, gps.latmindec);
  here.lon = degrees_and_decimal_minutes_to_decimal_degrees( gps.londeg, gps.lonmin, gps.lonmindec);
  return here;
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
  printf("\nlat = %d.%d\nlon = %d.%d\nalt = %d\n",latint,latrem,lonint,lonrem,gps_data.altitude_m);
  printf("speed = %d knots\n",gps_data.speed_knots);
  printf("speed = %d mph\n",(int)(gps_data.speed_knots*1.15078));
}
#else
void gps_dump_dmesg(){
  printf("This device does not have GPS.\n");
}
#endif
#ifdef GPS_TESTING
int test_get_current_position(gps_t testdata, latlon expected_latlon){
  int errors =0;
  latlon out = get_current_position(testdata);
  if( out.lat != expected_latlon.lat || out.lon != expected_latlon.lon ){
    printf("Got \t\t%f, %f\nexpected \t%f, %f\n",out.lat,out.lon,expected_latlon.lat,expected_latlon.lon);
    errors++;
  }
  return errors;
}
void main(){
  //(gcc gps.c -o gps -DGPS_TESTING; ./gps)
  gps_t gps;
  char *testdata="\x00\x01\x00\x01\x00\x0A\x2a\x00\x32\x25\x0A\x19\x87\x02\x06\x00\x4B\x00";
  /*printf("sizeof(gps_t) == %d\n", sizeof(gps_t));*/
  memcpy((void *)&gps,testdata,sizeof(gps_t));
  latlon expected;
  expected.lat = 10.715870;
  expected.lon = -10.417745;

  /*float lat = degrees_and_decimal_minutes_to_decimal_degrees( gps.latdeg, gps.latmin, gps.latmindec);*/
  /*float lon = degrees_and_decimal_minutes_to_decimal_degrees( gps.londeg, gps.lonmin, gps.lonmindec);*/
  /*printf("Got:    \tlat = %f lon =  %f\n",lat,lon);*/
  /*printf("Expect~: \tlat = %f lon = %f\n",10.71584833333333,-10.41774333333333);*/
  /*printf("alt = %d, expect 75\n",gps.altitude_m);*/

  int errors =0;
  errors += test_get_current_position( gps, expected );

  printf("Completed with %d errors\n",errors);

}
#endif
