#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <time.h>

void main(void){

	char fn[128],gif[128],cmd[256];
	FILE *fp, *fp2;

    struct tm *now, *yes;
	time_t present, yesterday;
	int yy,mm,dd;
	int	st_hr,st_mi,st_sc,ed_hr,ed_mi,ed_sc;

    int flg = 0;

    present = time(NULL);
    now = localtime(&present);

	yy = now->tm_year+1900;
	mm = now->tm_mon+1;
	dd = now->tm_mday;

	sprintf(fn,"ope_aq_%04d%02d%02d.txt",yy,mm,dd);
	fp = fopen(fn, "r");
	if (fp == NULL){
		printf("File Open Error. Can not find %s\n",fn);
		return;
	}
	fscanf(fp, "%02d%02d%02d", &st_hr,&st_mi,&st_sc);
	fscanf(fp, "%02d%02d%02d", &ed_hr,&ed_mi,&ed_sc);
	fclose(fp);
	
	fp2 = fopen("mail.txt", "w");
	fprintf(fp2,"---------------------------------\n");
	fprintf(fp2,"Starting AMATERAS observation\n");
	fprintf(fp2,"---------------------------------\n");
	fprintf(fp2,"   Date       : %04d/%02d/%02d\n",yy,mm,dd);
	fprintf(fp2,"   Start time : %02d:%02d:%02d\n",st_hr,st_mi,st_sc);
	fprintf(fp2,"   End   time : %02d:%02d:%02d\n",ed_hr,ed_mi,ed_sc);

    yesterday = present - 24*60*60;
    yes = localtime(&yesterday);

	yy = yes->tm_year+1900;
	mm = yes->tm_mon+1;
	dd = yes->tm_mday;
	sprintf(gif,"Chk_IPRT_SUN_%04d%02d%02d.gif",yy,mm,dd);

        if( (fp=fopen(gif,"r")) != NULL){
           fclose (fp);
           flg = 1;	
           fprintf(fp2,"\n");
	   fprintf(fp2,"---------------------------------\n");
	   fprintf(fp2,"Attached file : QL on %04d/%02d/%02d\n",yy,mm,dd);
        }
	
        fprintf(fp2,"---------------------------------\n");
        fprintf(fp2,"Disk info\n");
        fprintf(fp2,"---------------------------------\n");

	fclose(fp2);

	system ("df -m | grep F >> mail.txt");

        if(flg){
	   sprintf(cmd,"mail/smail.exe -hbzmail.plala.or.jp -fiprt@bz04.plala.or.jp -s\"AMATERAS observation start\" -Fmail.txt -a%s amateras@pparc.gp.tohoku.ac.jp",gif);
        }else{
	   sprintf(cmd,"mail/smail.exe -hbzmail.plala.or.jp -fiprt@bz04.plala.or.jp -s\"AMATERAS observation start\" -Fmail.txt amateras@pparc.gp.tohoku.ac.jp",gif);
        }

	printf("%s\n",cmd);
	system (cmd);

	return;

}
