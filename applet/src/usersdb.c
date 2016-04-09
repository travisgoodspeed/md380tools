/*! \file usersdb.c
\brief There is the functionality 
       which dmr id to the entry from the users.csv in flash reads
       the first line is the size im byte     
*/
            

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "md380.h" 
       
char * getdata(char * dest, char * src, int number) {
  spiflash_read(dest, (unsigned int) src, number);
                                               
  return(dest);
}


void trim(char *in, char *out, int max) {
  int i=0;
  char buffer[256];
  getdata(buffer, in, 256);
  while ( buffer[i] != '\n') {
    out[i]= buffer[i];
    i++;
    if (i > (max-2)) { 
      out[i]='\0';
      return;
    }
  }
  out[i]='\0';
}


void trimtonextl(char **p) {
  char buffer[256];
  char *pp;
  pp=*p;
 
  getdata(buffer, pp, 256);  
  while ( buffer[pp-*p] != '\n') pp++;
  pp++;
  *p=pp;
}

void trimtopreviousl(char  **p) {
  char buffer[256];
  char *pp;
  pp=*p;
       
  getdata(buffer, pp - 255, 256);     
  while ( buffer[pp-*p+255]  != '\n') pp--;
  pp--;
  while ( buffer[pp-*p+255]  != '\n') pp--;
  pp++;
  *p=pp;
}

int getfirstnumber(char * p) {
  char buffer[256];
  return (atoi(getdata(buffer, p, 10)));
}

int  find_dmr(char* str, unsigned int dmr_search, char * dmr_first, char * dmr_last, int maxstrlen) {
  unsigned int dmr_first_id, dmr_last_id, new_id;
  char * p;
  
  dmr_first_id = getfirstnumber(dmr_first);
  dmr_last_id  = getfirstnumber(dmr_last);

  if ( dmr_first_id == dmr_search ) {
    trim(dmr_first, str, maxstrlen);       
    return (1);
    }

  if ( dmr_last_id  == dmr_search ) {
    trim(dmr_last, str, maxstrlen);
    return (1);
    }

  p=(dmr_last - dmr_first) / 2 + dmr_first;
  trimtonextl(&p);
  new_id=getfirstnumber(p);

  if ( dmr_first_id == dmr_last_id || new_id == dmr_first_id || new_id == dmr_last_id) {
    str[0]='\0';
    return (0);
  }
 
  trimtonextl(&p);      
  if ( getfirstnumber(p) == dmr_search) {
    trim(p,str,maxstrlen);                                                   
    return (1);
  }
  
  if ( new_id > dmr_search) {
    dmr_last = (dmr_last - dmr_first) / 2 + dmr_first;
    trimtonextl(&dmr_last);          
  } 
  else {
    dmr_first = (dmr_last - dmr_first) / 2 + dmr_first;
    trimtonextl(&dmr_first);
  }
  return (find_dmr( str, dmr_search,  dmr_first,  dmr_last, maxstrlen));
}

int find_dmr_user(char * str, int dmr_search, char *data, int maxstrlen) {
  unsigned int datasize;
  char * data_start;
  char * data_end;

  datasize=getfirstnumber(data);
  data_start=data;
  trimtonextl(&data_start);
  data_end=data_start+datasize;
  trimtopreviousl(&data_end);

  return(find_dmr(str, dmr_search, data_start, data_end , 80));

}

