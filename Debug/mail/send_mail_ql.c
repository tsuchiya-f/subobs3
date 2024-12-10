#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <time.h>


int file_exist(char *path){
	
    int flg = 0;
	FILE *fp;
    if( (fp=fopen(path,"r")) != NULL){
       fclose (fp);
       flg = 1;
    }
    
    return flg;
}


void main(int argc, char *argv[]){

    int i;
	char fn[128],gif[4][128],cmd[256];
	FILE *fp, *fp2;

    struct tm *now, *yes;
	time_t present, yesterday;
	int yy,mm,dd;
	int	st_hr,st_mi,st_sc,ed_hr,ed_mi,ed_sc;

    int flg = 0;
    int flg_img = 0;

    /* input data file */
    if (argc != 2){
      printf ("usage $ send_mail_ql yyyymmdd\n");
      present = time(NULL);
      now = localtime(&present);
   	  yy = now->tm_year+1900;
	  mm =   now->tm_mon+1;
 	  dd = now->tm_mday;
    }else{
		sscanf(argv[1],"%04d%02d%02d",&yy,&mm,&dd);
	}

	sprintf(fn,"ope_aq_%04d%02d%02d.txt",yy,mm,dd);
	fp = fopen(fn, "r");
	if (fp == NULL){
		printf("File Open Error. Can not find %s\n",fn);
		return;
	}
	fscanf(fp, "%02d%02d%02d", &st_hr,&st_mi,&st_sc);
	fscanf(fp, "%02d%02d%02d", &ed_hr,&ed_mi,&ed_sc);
	fclose(fp);
	
	fp2 = fopen("mail_ql.txt", "w");
	fprintf(fp2,"---------------------------------\n");
	fprintf(fp2,"Result of AMATERAS observation\n");
	fprintf(fp2,"---------------------------------\n");
	fprintf(fp2,"   Date       : %04d/%02d/%02d\n",yy,mm,dd);
	fprintf(fp2,"   Start time : %02d:%02d:%02d\n",st_hr,st_mi,st_sc);
	fprintf(fp2,"   End   time : %02d:%02d:%02d\n",ed_hr,ed_mi,ed_sc);

	sprintf(&gif[0][0],"Chk_IPRT_SUN_%04d%02d%02d.gif",yy,mm,dd);
	
	system ("wget http://swnews.jp/rt/fig/goes_xr_3d.png");
	sprintf(&gif[1][0],"%04d%02d%02d_xray_flux.png",yy,mm,dd);
	sprintf(cmd,"mv goes_xr_3d.png %s",gif[1]);
    system( cmd );

	system ("wget https://sdo.gsfc.nasa.gov/assets/img/latest/latest_1024_HMIB.jpg");
	sprintf(&gif[2][0],"%04d%02d%02d_512_HMIB.jpg",yy,mm,dd);
	sprintf(cmd,"mv latest_1024_HMIB.jpg %s",gif[2]);
    system( cmd );

	system ("wget https://sdo.gsfc.nasa.gov/assets/img/latest/latest_1024_0094.jpg");
	sprintf(&gif[3][0],"%04d%02d%02d_512_0094.jpg",yy,mm,dd);
	sprintf(cmd,"mv latest_1024_0094.jpg %s",gif[3]);
    system( cmd );

    flg = file_exist(gif[0]);
//    printf("%d %d %d\n",file_exist(gif[1]), file_exist(gif[2]), file_exist(gif[3]));

//    flg_img = file_exist(gif[1]);
    if (file_exist(gif[1]) && file_exist(gif[2]) && file_exist(gif[3])) flg_img=1;

    if( flg ){
       fprintf(fp2,"\n");
       fprintf(fp2,"---------------------------------\n");
       fprintf(fp2,"Attached file : QL on %04d/%02d/%02d\n",yy,mm,dd);
    }

    fprintf(fp2,"---------------------------------\n");
    fprintf(fp2,"Disk info\n");
    fprintf(fp2,"---------------------------------\n");

	fclose(fp2);

	system ("df -m | grep F >> mail_ql.txt");

    if(flg){
       if (flg_img){
//          sprintf(cmd,"mail/smail.exe -hbzmail.plala.or.jp -fiprt@bz04.plala.or.jp -s\"AMATERAS QL on %04d-%02d-%02d\" -Fmail_ql.txt -a%s,%s  amateras@pparc.gp.tohoku.ac.jp",yy,mm,dd,gif[0],gif[1]);
          sprintf(cmd,"mail/smail.exe -hbzmail.plala.or.jp -fiprt@bz04.plala.or.jp -s\"AMATERAS QL on %04d-%02d-%02d\" -Fmail_ql.txt -a%s,%s,%s,%s  amateras@pparc.gp.tohoku.ac.jp",yy,mm,dd,gif[0],gif[1],gif[2],gif[3]);
       }else{
          sprintf(cmd,"mail/smail.exe -hbzmail.plala.or.jp -fiprt@bz04.plala.or.jp -s\"AMATERAS QL on %04d-%02d-%02d\" -Fmail_ql.txt -a%s  amateras@pparc.gp.tohoku.ac.jp",yy,mm,dd,gif[0]);
       }
    }

	printf("%s\n",cmd);
	system (cmd);

   if (flg_img){
      for (i=1;i<4;i++){
//      for (i=1;i<2;i++){
         sprintf(cmd,"mv %s sun_img/",gif[i]);
         system( cmd );
      }
   }

	return;

}
