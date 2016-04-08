#include <stdio.h>
#include <stdlib.h>
#include <string.h>
       

#define DATASIZE 10000000
#define FILENAME "data.txt"
char data[DATASIZE];

char * getdata(char * dest, char * src, int number) {
  int i;
  for (i=0;i<number;i++) dest[i]=src[i];
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

int getdmrid(char * p) {
  char buffer[256];
  return (atoi(getdata(buffer, p, 10)));
}

int  find_dmr(char* str, unsigned int dmr_search, char * dmr_first, char * dmr_last, int maxstrlen) {
  unsigned int dmr_first_id, dmr_last_id, new_id;
  char * p;
  
  dmr_first_id = getdmrid(dmr_first);
  dmr_last_id  = getdmrid(dmr_last);

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
  new_id=getdmrid(p);

  if ( dmr_first_id == dmr_last_id || new_id == dmr_first_id || new_id == dmr_last_id) {
    str[0]='\0';
    return (0);
  }
 
  trimtonextl(&p);      
  if ( getdmrid(p) == dmr_search) {
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

int main(int argc,char **argv) {
  char buffer[80];
  FILE * f;
  unsigned int datasize;
  char * data_start;
  char * data_end;
  unsigned int dmr_search; //, dmr_first_id, dmr_last_id;
  char str[80];
  int ret, i,ii;  
  if (argc < 2 ) {
    printf("enter %s <id>\n",argv[0]);
    return(0);
    }       

  dmr_search=atoi(argv[1]);

  f=fopen(FILENAME,"r");
  if (f==NULL) {
    printf("can't open %s\n",FILENAME);
    return(0);
    }
  
  fread(data, 1, DATASIZE, f);
  datasize=atoi(getdata(buffer, data, 10));
  data_start=data;
  trimtonextl(&data_start);
  
  data_end=data_start+datasize;
  trimtopreviousl(&data_end);
  
  ret=find_dmr(str, dmr_search, data_start, data_end , 80);
  ii=0;
  for (i=0;i<strlen(str);i++) {
    if (str[i] == ',') {
    buffer[ii++]='\0';
    printf("%s\n",buffer);
    ii=0;
    } else {
    buffer[ii++]=str[i];
    }
  }
    
  printf("%s\n",ret==1?str:"not in list");
  printf("%d\n",strlen(str));
  fclose(f);   
     
return(1);  
}