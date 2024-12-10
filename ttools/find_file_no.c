/*
Create background spectrum (for both low-res QL data & high-res raw data) 
*/

#include <stdio.h>
#include <string.h>
#include <math.h>

/* global struct */
typedef struct param
{
  int yy;
  int mm;
  int dd;
  int hs;
  int ns;
  int ss;
  int he;
  int ne;
  int se;
  int s1;
  int s2;
  int fs;
  int fe;
} PARAM;

PARAM sp;

/* proto-type decl. */
static int find_time_range();

/*-----------------------------------------------------------*/
void main(void)
{

  int  ret;
  int  yy,mm,dd;    /* date : yy:YYYY, mm:MM, dd:DD */
  int  s1,d2;

  /* input date */
  printf ("Input Date (JST) (YYYY MM DD)\n");
  scanf  ("%d %d %d",&sp.yy, &sp.mm, &sp.dd);

  /* find time range */
  ret = find_time_range();
  if(ret){
    printf("file open error in find_time_range\n");
    return;
  }

  /* show result */
  printf("DN in low-res  data : %d - %d\n",sp.s1,sp.s2);
  printf("FN of high-res data : %d - %d\n",sp.fs,sp.fe);

  return;
}
 
/*------------------------------------------------------------------*/
/* find time range
   Error code : 1 --- file open error
*/

static int find_time_range()
{

  int yy, mm, dd;
  char fname[128];

  int tio[6];     /* observation start & end time (hh nn ss hh nn ss) */
  int tia[6];     /* analysis start & end time (hh nn ss hh nn ss) */
 
  int sec_os, sec_oe, sec_as, sec_ae;

  int dl  = 120; /* length of a raw data file (sec) */

  FILE *fp;

  /* local variables */
  yy = sp.yy;
  mm = sp.mm;
  dd = sp.dd;

  /* find observation start and end time */

  sprintf(fname,"../Debug/%04d%02d%02d/Qlhead_%04d%02d%02d.txt",yy,mm,dd,yy,mm,dd);
  fp = fopen (fname,"r");
  if (fp == NULL){
    printf("File Open Error : %s\n",fname);
    return (1);
  }
  fscanf(fp, "%d", &tio[0]);
  fscanf(fp, "%d", &tio[1]);
  fscanf(fp, "%d", &tio[2]);
  fscanf(fp, "%d", &tio[3]);
  fscanf(fp, "%d", &tio[4]);
  fscanf(fp, "%d", &tio[5]);
  fclose (fp);

  printf("Observation time : %02d:%02d:%02d - %02d:%02d:%02d\n",
	 tio[0], tio[1], tio[2], tio[3], tio[4], tio[5]);

  sec_os = tio[0] * 3600 + tio[1] * 60 + tio[2];
  sec_oe = tio[3] * 3600 + tio[4] * 60 + tio[5];

  /* input analysis period */
  printf ("Input start time (JST) (hh nn ss): ");
  scanf ("%d %d %d", &tia[0], &tia[1], &tia[2]);
  printf ("Input end time   (JST) (hh nn ss): ");
  scanf ("%d %d %d", &tia[3], &tia[4], &tia[5]);

  sec_as = tia[0] * 3600 + tia[1] * 60 + tia[2];
  sec_ae = tia[3] * 3600 + tia[4] * 60 + tia[5];

  /* find time range */
  sp.s1 = sec_as - sec_os;
  sp.s2 = sec_ae - sec_os;

  /* data file number */
  sp.fs = (int)(sp.s1 / dl);
  sp.fe = (int)(sp.s2 / dl) + 1;

  sp.hs = tia[0];
  sp.ns = tia[1];
  sp.ss = tia[2];
  sp.he = tia[3];
  sp.ne = tia[4];
  sp.se = tia[5];
  
  return (0);
}
/*------------------------------------------------------------------*/
