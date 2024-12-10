/*
Create background spectrum (for both low-res QL data & high-res raw data) 
*/

#include <stdio.h>
#include <string.h>
#include <math.h>

#define M  (12000)   /* maximum number of spectrum in a file */
#define NL (1024)    /* number of freq. bin of low-res data  */
#define NH (16384)   /* number of freq. bin of high-res data */

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
static double ConvertTodB(double linearValue);
static int get_bg_low(double *data_r, int sz);
static int get_bg_high(double *data_r, int sz);
static int save_data(double *data_h, int sz_h, double *data_l, int sz_l, double bias, int er_flg);

double data[NH];

/*-----------------------------------------------------------*/
void main(void)
{

  int  ret;
  int  yy,mm,dd;    /* date : yy:YYYY, mm:MM, dd:DD */
  int  s1,d2;
  int  er_flg = 0;

  double data_h[NH];
  double data_l[NL];
  double bias;

  /* input date */
  printf ("Input Date (JST) (YYYY MM DD)\n");
  scanf  ("%d %d %d",&sp.yy, &sp.mm, &sp.dd);
  printf ("Input bias [dB]\n");
  scanf  ("%lf",&bias);

  /* find time range */
  ret = find_time_range();
  if(ret){
    printf("file open error in find_time_range\n");
    return;
  }

  /* find low-res data background */
  ret = get_bg_low(data_l, sizeof(data_l));
  if(ret){
    printf("file open error in get_bg_low\n");
    er_flg ++;
  }

  /* find high-res data background */
  ret = get_bg_high(data_h, sizeof(data_h));
  if(ret){
    printf("file open error in get_bg_high\n");
    er_flg += 2;
  }

  /* save data */
  ret = save_data(data_h, sizeof(data_h), data_l, sizeof(data_l), bias, er_flg);

  return;
}
 
/*------------------------------------------------------------------*/
/* find time range
   Error code : 1 --- file open error
*/

static int find_time_range()
{

  int yy, mm, dd, s1, s2;
  char fname[128];

  int tio[6];     /* observation start & end time (hh nn ss hh nn ss) */
  int tia[6];     /* analysis start & end time (hh nn ss hh nn ss) */
 
  int sec_os, sec_oe, sec_as, sec_ae;

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

  sp.hs = tia[0];
  sp.ns = tia[1];
  sp.ss = tia[2];
  sp.he = tia[3];
  sp.ne = tia[4];
  sp.se = tia[5];
  
  return (0);
}

/*------------------------------------------------------------------*/
/* convert raw value to absolute power (dBm) */
double ConvertTodB(double linearValue)
{
  if(linearValue>0.0){
    return(-96.28+10.0*log10(linearValue+1.0));
  }else{
    return -100.0;
  }
}

/*------------------------------------------------------------------*/
/* find low-res data background                                     */
static int get_bg_low(double *data_r, int sz)
{

  int yy, mm, dd, s1, s2;

  int    i;
  int    cnt = 0;

  char fname[128];
  FILE *fp;

  /* local variables */
  yy = sp.yy;
  mm = sp.mm;
  dd = sp.dd;
  s1 = sp.s1;
  s2 = sp.s2;

  /* init */
  sz = sizeof(data_r);

  /* data file */
  printf("\n--- low resolution data ---\n");
  sprintf(fname,"../Debug/%04d%02d%02d_QLcheck.txt",yy,mm,dd);
  printf(" reading %s from %d to %d\n",fname,s1,s2);
  
  fp = fopen (fname,"rb");
  if (fp == NULL){
    printf("File Open Error : %s\n",fname);
    return (1);
  }

  while(!feof(fp)){
    fread( data, sizeof(double), NL, fp );

    if (cnt > s1){
      for(i=0;i<NL;i++){			  
	data[i] = ConvertTodB(data[i]);
	if(data_r[i] > data[i]) data_r[i] = data[i];
      }
    }

    cnt ++;

    if (cnt > s2) break;

  }

  return (0);

}

/*------------------------------------------------------------------*/
/* find high-res data background                                    */
static int get_bg_high(double *data_r, int sz)
{

  int yy, mm, dd, s1, s2;

  int    i,j,is;
  int    dl  = 120; /* length of a raw data file (sec) */

  int    fs,fe;     /* start & end number of input file (YYYYMMDD_N.txt) */
  int    ns,ne;
  int    nf = 1;

  char fname[128];
  FILE *fp;

  int cnt_inv=0;

#if 0
  FILE *fp_;
  int  flg = 0;
  int i_;
#endif

  /* local variables */
  yy = sp.yy;
  mm = sp.mm;
  dd = sp.dd;
  s1 = sp.s1;
  s2 = sp.s2;

  /* init */
  memset (data_r, 0x0, sz);

  /* data file number */
  fs = (int)(s1 / dl);
  ns = s1 % dl;
  fe = (int)(s2 / dl) + 1;
  ne = s2 % dl;

  printf("\n--- high resolution data ---\n");
  printf(" file number %d-%d\n",fs,fe);
  printf(" read first file from %d\n",ns);
  printf(" read last  file to   %d\n",ne);

  /* data file */
  for (j=fs;j<fe+1;j++){

    sprintf(fname,"../Debug/%04d%02d%02d_%d.txt",yy,mm,dd,j);

    printf("  reading %s\n",fname);

    fp = fopen (fname,"rb");
    if (fp == NULL){
      printf("File Open Error : %s\n",fname);
    }else{
      nf = 0;

      while(!feof(fp)){

	fread( data, sizeof(double), NH, fp );
#if 0
	if(!flg){
	   flg = 1;
           fp_ = fopen("tmp.out","w");
	   for(i_=0;i_<NH;i_++){
              fprintf(fp_,"%d %f\n",i_,data[i_]);
	   }
           fclose (fp_);
	}
#endif
	
	for(i=0;i<NH;i++){

	  is = i/100;
	  if (i==fe && is>ne) break;
	  if (i==fs && is<ns){
	    /* skip */
	  }else{
            if(data[i] > 0.0) {
	       data[i] = ConvertTodB(data[i]);
	       if(data_r[i] > data[i]) data_r[i] = data[i];
            }else{
               cnt_inv ++;
            }
	  }

	}
      }  /* while */
    }
  }

  sp.fs = fs;
  sp.fe = fe;

  printf("number of invalid data : %d\n",cnt_inv);

  return nf;

}
/*------------------------------------------------------------------*/

static int save_data(double *data_h, int sz_h, double *data_l, int sz_l, double bias, int er_flg){

  FILE *fp;
  char fname[128];
  int i,sz;

  if (er_flg == 3) return 0;

  /* save log */
  sprintf (fname,"make_amateras_bg.log");
  fp = fopen (fname,"a");
  fprintf (fp,"Date of source data    : %04d/%02d/%02d\n",sp.yy,sp.mm,sp.dd);
  fprintf (fp,"Start & end time       : %02d:%02d:%02d - %02d:%02d:%02d\n",
	   sp.hs,sp.ns,sp.ss,sp.he,sp.ne,sp.se);
  fprintf (fp,"Second from obs start  : %d - %d\n",sp.s1,sp.s2);
  fprintf (fp,"File number of raw data: %d - %d\n",sp.fs,sp.fe);
  fprintf (fp,"Bias subtracted from BG data : %f\n",bias);
  if (er_flg == 1){
	fprintf (fp,"no qsun_bg_low.out\n");
  }
  if (er_flg == 2){
	fprintf (fp,"no qsun_bg_high.out\n");
  }
  fprintf (fp,"\n");
  fclose (fp);

  /* save low-res data */
  if (er_flg == 1){
	// skip
  }else{
    sprintf (fname,"qsun_bg_low.out");
    fp = fopen (fname,"w");
    sz = sz_l/sizeof(double);
    printf("save sz = %d\n",sz);
    for (i=0;i<sz;i++){
      fprintf(fp,"%f\n",data_l[i]-bias);
    }
    fclose (fp);
  }

  /* save high-res data */
  if (er_flg == 2){
	// skip
  }else{
    sprintf (fname,"qsun_bg_high.out");
    fp = fopen (fname,"w");
    sz = sz_h/sizeof(double);
    printf("save sz = %d\n",sz);
    for (i=0;i<sz;i++){
      fprintf(fp,"%f\n",data_h[i]-bias);
    }
    fclose (fp);
  }

  return (0);

}
/*------------------------------------------------------------------*/
