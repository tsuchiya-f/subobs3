/*
 IPRT/AMATERAS Level1 Data QL
 author : F. Tsuchiya @ Oct. 8, 2013
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cpgplot.h"

#define iitate

#define NA (2)     /* time decimation */
#define NB (6)      /* freq decimation */
#define M (12000)   /* number of time step (100/sec * 120sec) */
#define N (6554)    /* frequency channel of each pol */
#define n  N/NB
#define m  M/NA

/* proto-type decl. */
static void get_data_s ( );
static void init_pgplot(char * );
static void plot_wedge(float , float );
static void plot_data_s( );

unsigned short int dat[N];
unsigned short dr[m*n];
unsigned short dl[m*n];
unsigned short br[n], bl[n];
float img[m*n];
long l_dr[m*n],l_dl[m*n];
float f_br[n],f_bl[n],f_fq[n];

/* public variable */
char ver[] = "v1";
#ifdef iitate
char dir[] = "./";
#else
char dir[] = "/db/IPRT-SUN/DATA1/";
#endif
int ncol = 64;
float vpx1 = 0.15, vpx2 = 0.75, vpy1 = 0.15, vpy2 = 0.9;
float bpx1 = 0.77, bpx2 = 0.85, bpy1 = 0.15, bpy2 = 0.9;
char infile[128];

/*----------------------------------------------------------*
 * main routine
 *----------------------------------------------------------*/
int main(int argc, char *argv[]){

  char          cmd[128];
  FILE          *fp;
  int            np = 0;

  /* input data file */
  if (argc != 2){
    printf ("usage $ main yyyymmdd\n");
    return;
  }

  sprintf(cmd,"ls -tr %s%s/*_Lv1_16bit*.*>filelist_s.dat",dir,argv[1]);
  system(cmd);
  fp = fopen("filelist_s.dat","r");

  /* plot data */
  init_pgplot(argv[1]);

  while(1){

    fscanf(fp,"%s",infile);
    if (feof(fp)) break;
    printf("input %s\n",infile);

    get_data_s();

    plot_data_s();
    cpgpage();

    np ++;
  }

  cpgend();

  fclose (fp);

  /* make pdf */
  if (np){
    sprintf(cmd,"ps2pdf IPRT-SUN-Lv1_16bit-%s-%s.ps",argv[1],ver);
    printf("%s\n",cmd);
    system (cmd);
    sprintf(cmd,"scp IPRT-SUN-Lv1_16bit-%s-%s.pdf iprt@radio.gp.tohoku.ac.jp:/db/IPRT-SUN/BROWSE/pdf/",argv[1],ver);
    printf("%s\n",cmd);
    system (cmd);
  }
  sprintf(cmd,"rm -f IPRT-SUN-Lv1_16bit-%s-%s.ps",argv[1],ver);
  system (cmd);

  return;

}

/*----------------------------------------------------------*
 *  Get Data
 *  (read every NA spectra)
 *----------------------------------------------------------*/
static void get_data_s(){

  FILE *fp;
  int i,j;
  int ii;

  memset (dr,0x00,sizeof(dr));
  memset (dl,0x00,sizeof(dl)); 
  memset (br,0xff,sizeof(br));
  memset (bl,0xff,sizeof(bl));

  fp = fopen(infile,"rb");

  for(i=0;i<m;i++){

    {
      long l_ftell;
      l_ftell = ftell(fp);
      //      printf("1 %d\n",l_ftell);
    }

    // R
    fread (dat, sizeof(unsigned short int), N, fp);
    for (j=0;j<n;j++){
      ii = j + i*n;
      dr[ii] = dat[j*NB];
      if (dr[ii] < br[j] && dr[ii] != 0) br[j] = dr[ii];
    }

    // SKIP
    fseek(fp, N*(NA-1)*sizeof(short int), SEEK_CUR);

  }

  for(i=0;i<m;i++){

    {
      long l_ftell;
      l_ftell = ftell(fp);
      //      printf("2 %d\n",l_ftell);
    }

    // L
    fread (dat, sizeof(unsigned short int), N, fp);
    for (j=0;j<n;j++){
      ii = j + i*n;
      dl[ii] = dat[j*NB];
      if (dl[ii] < bl[j] && dl[ii] != 0) bl[j] = dl[ii];
    }

    {
      long l_ftell;
      l_ftell = ftell(fp);
      //      printf("3 %d\n",l_ftell);
    }

    // SKIP
    fseek(fp, N*(NA-1)*sizeof(short int), SEEK_CUR);

  }

  fclose (fp);
  
  return;
}

/*----------------------------------------------------------* 
 *  Init PGPLOT
 *----------------------------------------------------------*/
static void init_pgplot(char* str)
{

  float c1,c2,c3;
  int   i;
  char  ps[64];

  sprintf(ps,"IPRT-SUN-Lv1_16bit-%s-%s.ps/cps",str,ver);
  printf("output %s\n",ps);
  cpgbeg (0,ps,1,2);
  //  cpgbeg (0,"/xs",1,2);

  cpgsch(2.0);

  /* color code */ 
  c1 = 360.0;
  c3 = 1.0;
  for (i=0;i<ncol-1;i++){
    c2 = (float)i/(float)(ncol-2);
    cpgshls(16+i,c1,c2,c3);
  }
  c1 = 120.0;
  c2 = 0.5;
  c3 = 1.0;
  cpgshls(15+ncol,c1,c2,c3); // red = max value

  /*
  c2 = 0.5;
  c3 = 1.0;
  for (i=0;i<ncol;i++){
    c1 = 360.0-240.0*(float)i/(float)(ncol-1);
    cpgshls(16+i,c1,c2,c3);
  }
  */

  return;
}

/*----------------------------------------------------------*
 *  Plot wedge
 *----------------------------------------------------------*/
static void plot_wedge(float fmin, float fmax)
{

  int j;
  float y1,y2;

  cpgsvp(bpx2+0.01,bpx2+0.02,vpy1,vpy2);
  cpgswin(0.0,1.0,fmin,fmax);

  for(j=0;j<ncol;j++){
    y1 = fmin + (float)j/(float)ncol*(fmax-fmin); 
    y2 = y1 + (fmax-fmin)/(float)ncol;
    cpgsci(16+j);
    cpgrect(0.0,1.0,y1,y2);
  }
  cpgsci(1);
  cpgmtxt("R",2.5,1.0,1.0,"[dB]");

  cpgbox("BC",0.0,0,"BCM",0.0,0);

  return;
}

/*----------------------------------------------------------*
 *  Plot data
 *----------------------------------------------------------*/
static void plot_data_s(){

  int i,j,k,k2;
  float tr[]={0.0,1.0,0.0,0.0,0.0,1.0};
  float ave=0.0,sgm=0.0;
  float fmin,fmax;
  char  label[128];

  float x1,x2,y1,y2;
  int   ic;

  float fa=0.001;

  tr[1] = 120.0/((float)m);
  tr[3] = 100.0;
  tr[5] = 400.0/((float)n);
  /*
  tr[1] = 120.0/(12000.0/10.0);
  tr[3] = 100.0;
  tr[5] = 400.0/(6554.0/6.0);
  */

  /* subtract baseline */
  for (i=0;i<m;i++){
    for (j=0;j<n;j++){
      k = i*n + j;
      l_dr[k] = (long)dr[k]-(long)br[j];
      l_dl[k] = (long)dl[k]-(long)bl[j];
    }
  }
  for (j=0;j<n;j++){
    f_br[j] = (float)br[j]*fa; // [dB]
    f_bl[j] = (float)bl[j]*fa; // [dB]
    f_fq[j] = 100.0 + (float)j/(float)(n-1)*400.0; // [MHz]
  }

  /* search range */
  for (i=0;i<m;i++){
    for (j=0;j<n;j++){
      k = i*n + j;
      ave += (float)l_dr[k]*fa; // [dB]
      ave += (float)l_dl[k]*fa; // [dB]
    }
  }
  ave /= (float)(m*n*2);

  for (i=0;i<m;i++){
    for (j=0;j<n;j++){
      k = i*n + j;
      sgm += ((float)l_dr[k]*fa-ave)*((float)l_dr[k]*fa-ave);
      sgm += ((float)l_dl[k]*fa-ave)*((float)l_dl[k]*fa-ave);
    }
  }
  sgm = sqrt(sgm/(float)(m*n*2));

  fmin = -1.5;
  fmax = ave + 6*sgm;
  //  fmin = ave - 3*sgm;
  //  fmax = 25.5;

  /* plot RCP */

  for (i=0;i<m;i++){
    for (j=0;j<n;j++){
      k = i*n + j;
      k2 = i + j*m;
      img[k2] = (float)l_dr[k]*fa; // [dB]
      if (img[k2]<fmin) img[k2] = fmin;
      if (img[k2]>fmax) img[k2] = fmax;
    }
  }

  //**********************************************//
  /*
  {
    char fn0[256];
    FILE *fp0;
      
    sprintf(fn0,"bp.dat");
    fp0 = fopen(fn0,"w");
    for (i=0;i<m;i++){
      for (j=0;j<n;j++){
	//k = j + i*n;
	//fprintf(fp0,"%d %d %d\n",i,j,dl[k]);
	k = j*m + i;
	x1 = (float)i     * tr[1];
	y1 = tr[3] + (float)j      * tr[5];
	fprintf(fp0,"%f %f %f\n",x1,y1,img[k]);
      }
      fprintf(fp0,"\n");
    }
    fclose(fp0);
  }
  */
  //**********************************************//

  cpgpanl(1,1);
  cpgsvp(vpx1,vpx2,vpy1,vpy2);
  cpgswin(0.0,120.0,100.0,500.0);
  cpgbox("BCINTS",0.0,0,"BCINTS",0.0,0);

  cpgmtxt("T",0.5,0.0,0.0,infile);
  cpgmtxt("B",2.5,1.0,1.0,"Time [sec]");
  cpgmtxt("L",2.5,1.0,1.0,"Freq. [MHz]");
  cpgscir(16,15+ncol);
  cpgimag(img,m,n,1,m,1,n,fmin,fmax,tr);

  cpgsvp(bpx1,bpx2,bpy1,bpy2);
  cpgswin(0.0,50.0,100.0,500.0);
  cpgbox("BCINTS",0.0,0,"BCITS",0.0,0);
  cpgmtxt("B",2.5,1.0,1.0,"Min[dB]");
  cpgmtxt("T",0.5,1.0,1.0,"RCP");
  cpgline(n,f_br,f_fq);

  plot_wedge(fmin,fmax);

  /* plot LCP */

  for (i=0;i<m;i++){
    for (j=0;j<n;j++){
      k = i*n + j;
      k2 = i + j*m;
      img[k2] = (float)l_dl[k]*fa; // [dB]
      if (img[k2]<fmin) img[k2] = fmin;
      if (img[k2]>fmax) img[k2] = fmax;
    }
  }

  //**********************************************//
  /*
  {
    char fn0[256];
    FILE *fp0;
      
    sprintf(fn0,"bp2.dat");
    fp0 = fopen(fn0,"w");
    for (i=0;i<m;i++){
      for (j=0;j<n;j++){
	//k = j + i*n;
	//fprintf(fp0,"%d %d %d\n",i,j,dl[k]);
	k = j*m + i;
	x1 = (float)i     * tr[1];
	y1 = tr[3] + (float)j      * tr[5];
	fprintf(fp0,"%f %f %f\n",x1,y1,img[k]);
      }
      fprintf(fp0,"\n");
    }
    fclose(fp0);
  }
  */
  //**********************************************//

  cpgpanl(1,2);
  cpgsvp(vpx1,vpx2,vpy1,vpy2);
  cpgswin(0.0,120.0,100.0,500.0);
  cpgbox("BCINTS",0.0,0,"BCINTS",0.0,0);

  cpgmtxt("B",2.5,1.0,1.0,"Time [sec]");
  cpgmtxt("L",2.5,1.0,1.0,"Freq. [MHz]");

  cpgscir(16,15+ncol);
  cpgimag(img,m,n,1,m,1,n,fmin,fmax,tr);

  cpgsvp(bpx1,bpx2,bpy1,bpy2);
  cpgswin(0.0,50.0,100.0,500.0);
  cpgbox("BCINTS",0.0,0,"BCITS",0.0,0);
  cpgmtxt("B",2.5,1.0,1.0,"Min[dB]");
  cpgmtxt("T",0.5,1.0,1.0,"LCP");
  cpgline(n,f_bl,f_fq);

  plot_wedge(fmin,fmax);

  return;
}
