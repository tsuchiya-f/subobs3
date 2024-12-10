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

#define NA (10)
#define NB (6)
#define M (60000)   /* number of time step (100/sec * 120sec * 5file) */
#define N (6554)    /* frequency channel of each pol */

/* proto-type decl. */
static void get_data (char *, unsigned char *, unsigned char *,
		      unsigned char *, unsigned char * );
static void init_pgplot(char * );
static void plot_wedge(float , float );
static void plot_data(unsigned char *, unsigned char *,
		      unsigned char *, unsigned char * );

/* public variable */
char ver[] = "v2";
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

  unsigned char *dr;
  unsigned char *dl;
  unsigned char *br, *bl;
  char          cmd[128];
  FILE          *fp;
  int n = N/NB;
  int m = M/NA;

  int np=0;

  /* input data file */
  if (argc != 2){
    printf ("usage $ main yyyymmdd\n");
    return;
  }

  sprintf(cmd,"ls -tr %s%s/*_Lv1_*.*|grep -v 16bit>filelist.dat",dir,argv[1]);
  system(cmd);
  fp = fopen("filelist.dat","r");

  dr = (unsigned char *)malloc (m*n);
  dl = (unsigned char *)malloc (m*n);
  br = (unsigned char *)malloc (n);
  bl = (unsigned char *)malloc (n);

  /* plot data */
  init_pgplot(argv[1]);

  while(1){

    fscanf(fp,"%s",infile);
    if (feof(fp)) break;
    printf("input %s\n",infile);

    np ++;

    get_data(infile,dr,dl,br,bl);

    /*
    {
      static int i=0,j;
      char fn0[256];
      FILE *fp0;
      
      sprintf(fn0,"bp_%d.dat",i++);
      fp0 = fopen(fn0,"w");
      for (j=0;j<n;j++){
	fprintf(fp0,"%d %d %d\n",j,br[j],bl[j]);
      }
      fclose(fp0);
    }
    */

    plot_data(dr,dl,br,bl);
    cpgpage();
  }

  cpgend();

  free (dr);
  free (dl);
  free (br);
  free (bl);

  fclose (fp);

  /* make pdf */
  if ( np ){
    sprintf(cmd,"ps2pdf IPRT-SUN-Lv1-%s-%s.ps",argv[1],ver);
    system (cmd);
  }
  sprintf(cmd,"rm -f IPRT-SUN-Lv1-%s-%s.ps",argv[1],ver);
  system (cmd);

  return;

}

/*----------------------------------------------------------*
 *  Get Data
 *  (read every NA spectra)
 *----------------------------------------------------------*/
static void get_data(char *fn, unsigned char *dr, unsigned char *dl,
		     unsigned char *br, unsigned char *bl){

  FILE *fp;
  int i,j;
  int ii;
  unsigned char *dat;
  int n = N/NB;
  int m = M/NA;

  dat = malloc(N*NA);

  memset (dr,0x00,sizeof(dr));
  memset (dl,0x00,sizeof(dl)); 
  memset (br,0xff,sizeof(char)*n);
  memset (bl,0xff,sizeof(char)*n);

  fp = fopen(fn,"rb");

  for(i=0;i<m;i++){
    fread (dat, sizeof(unsigned char), N*NA, fp);
    for (j=0;j<n;j++){
      ii = j + i*n;
      dr[ii] = dat[j*NB];
      if (dr[ii] < br[j] && dr[ii] != 0) br[j] = dr[ii];
    }
  }
  for(i=0;i<m;i++){
    fread (dat, sizeof(unsigned char), N*NA, fp);
    for (j=0;j<n;j++){
      ii = j + i*n;
      dl[ii] = dat[j*NB];
      if (dl[ii] < bl[j] && dl[ii] != 0) bl[j] = dl[ii];
    }
  }

  free (dat);
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

  sprintf(ps,"IPRT-SUN-Lv1-%s-%s.ps/cps",str,ver);
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
  c1 = 180.0;
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
static void plot_data(unsigned char *dr, unsigned char *dl,
		      unsigned char *br, unsigned char *bl){

  int i,j,k,k2;
  //  float *img;
  int *img;
  short *s_dr,*s_dl;
  float *f_br,*f_bl,*f_fq;
  //  float tr[]={0.0,1.0,0.0,0.0,0.0,1.0};
  float ave=0.0,sgm=0.0;
  float fmin,fmax,db;
  char  label[128];

  int n = N/NB;
  int m = M/NA;

  img  = (int *)malloc(sizeof(int)*m*n);
  s_dr = (short *)malloc(sizeof(short)*m*n);
  s_dl = (short *)malloc(sizeof(short)*m*n);
  f_br = (float *)malloc(sizeof(float)*n);
  f_bl = (float *)malloc(sizeof(float)*n);
  f_fq = (float *)malloc(sizeof(float)*n);
  /*
  tr[1] = 600.0/(float)m;
  tr[3] = 100.0;
  tr[5] = 400.0/(float)n;
  */

  /* subtract baseline */
  for (i=0;i<m;i++){
    for (j=0;j<n;j++){
      k = i*n + j;
      s_dr[k] = (short)dr[k]-(short)br[j];
      s_dl[k] = (short)dl[k]-(short)bl[j];
    }
  }
  for (j=0;j<n;j++){
    f_br[j] = (float)br[j]/256.0*25.5; // [dB]
    f_bl[j] = (float)bl[j]/256.0*25.5; // [dB]
    f_fq[j] = 100.0 + (float)j/(float)(n-1)*400.0; // [MHz]
  }

  /* search range */
  for (i=0;i<m;i++){
    for (j=0;j<n;j++){
      k = i*n + j;
      ave += (float)s_dr[k]/256.0*25.5; // [dB]
      ave += (float)s_dl[k]/256.0*25.5; // [dB]
    }
  }
  ave /= (float)(m*n*2);

  for (i=0;i<m;i++){
    for (j=0;j<n;j++){
      k = i*n + j;
      sgm += ((float)s_dr[k]/256.0*25.5-ave)*((float)s_dr[k]/256.0*25.5-ave);
      sgm += ((float)s_dl[k]/256.0*25.5-ave)*((float)s_dl[k]/256.0*25.5-ave);
    }
  }
  sgm = sqrt(sgm/(float)(m*n*2));

  fmin = -1.5;
  fmax = ave + 3*sgm;
  //  fmin = ave - 3*sgm;
//  fmax = 25.5;

  /* plot RCP */

  for (i=0;i<m;i++){
    for (j=0;j<n;j++){
      k = i*n + j;
      k2 = i + j*m;
      db = (float)s_dr[k]/256.0*25.5; // [dB]
      img[k2] = 16 + (int)((db-fmin)/(fmax-fmin)*(float)ncol);
      if (img[k2]<16) img[k2] = 16;
      if (img[k2]>15+ncol) img[k2] = 15+ncol;
      if ( (db + f_br[j]) > 25.4 ) img[k2] = 2;
    }
  }

  cpgpanl(1,1);
  cpgsvp(vpx1,vpx2,vpy1,vpy2);
  cpgswin(0.0,600.0,100.0,500.0);
  cpgbox("BCINTS",0.0,0,"BCINTS",0.0,0);

  cpgmtxt("T",0.5,0.0,0.0,infile);
  cpgmtxt("B",2.5,1.0,1.0,"Time [sec]");
  cpgmtxt("L",2.5,1.0,1.0,"Freq. [MHz]");
 
  /*
  cpgscir(16,15+ncol);
  cpgimag(img,m,n,1,m,1,n,fmin,fmax,tr);
  */
  cpgpixl(img,m,n,1,m,1,n,0.0,600.0,100.0,500.0);

  cpgsvp(bpx1,bpx2,bpy1,bpy2);
  cpgswin(0.0,25.5,100.0,500.0);
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
      db = (float)s_dl[k]/256.0*25.5; // [dB]
      img[k2] = 16 + (int)((db-fmin)/(fmax-fmin)*(float)ncol);
      if (img[k2]<16) img[k2] = 16;
      if (img[k2]>15+ncol) img[k2] = 15+ncol;
      if ( (db + f_bl[j]) > 25.4 ) img[k2] = 2;
      /*
      img[k2] = (float)s_dl[k]/256.0*25.5; // [dB]
      if (img[k2]<fmin) img[k2] = fmin;
      if (img[k2]>fmax) img[k2] = fmax;
      */
    }
  }

  cpgpanl(1,2);
  cpgsvp(vpx1,vpx2,vpy1,vpy2);
  cpgswin(0.0,600.0,100.0,500.0);
  cpgbox("BCINTS",0.0,0,"BCINTS",0.0,0);

  cpgmtxt("B",2.5,1.0,1.0,"Time [sec]");
  cpgmtxt("L",2.5,1.0,1.0,"Freq. [MHz]");

  /*
  cpgscir(16,15+ncol);
  cpgimag(img,m,n,1,m,1,n,fmin,fmax,tr);
  */
  cpgpixl(img,m,n,1,m,1,n,0.0,600.0,100.0,500.0);

  cpgsvp(bpx1,bpx2,bpy1,bpy2);
  cpgswin(0.0,25.5,100.0,500.0);
  cpgbox("BCINTS",0.0,0,"BCITS",0.0,0);
  cpgmtxt("B",2.5,1.0,1.0,"Min[dB]");
  cpgmtxt("T",0.5,1.0,1.0,"LCP");
  cpgline(n,f_bl,f_fq);

  plot_wedge(fmin,fmax);

  free (img);
  free (s_dr);
  free (s_dl);
  free (f_br);
  free (f_bl);
  free (f_fq);

  return;
}
