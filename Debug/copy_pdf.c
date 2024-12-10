#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>

//int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int showCmd)
int main(int argc, char *argv[])
{
	time_t timer;
    struct tm *now;

	int yy;
	int mm;
	int dd;
	char send[100],copy[100],cmd[100];

	if (argc == 1){
	//	日付と時刻を取得
    	timer = time(NULL);
    	now = localtime(&timer);

		yy = now->tm_year+1900;
		mm = now->tm_mon+1;
		dd = now->tm_mday;
	}else{
	  	sscanf(argv[1],"%4d%2d%2d",&yy,&mm,&dd);
	  	printf("date:%04d/%02d/%02d\n",yy,mm,dd);
	}


//	sprintf(send,"IPRT-SUN-Lv1-%04d%02d%02d-v2.pdf",yy,mm,dd);
//	sprintf(copy,"Y:\\sun\\pdf\\IPRT-SUN-Lv1-%04d%02d%02d-v2.pdf",yy,mm,dd);
//	CopyFile(send,copy,TRUE);//FALSEだとファイルの上書き
	sprintf(cmd,"cp IPRT-SUN-Lv1-%04d%02d%02d-v2.pdf /cygdrive/y/sun/pdf/",yy,mm,dd);
	printf("%s\n", cmd);
	system (cmd);

	sprintf(cmd,"scp IPRT-SUN-Lv1-%04d%02d%02d-v2.pdf iprt@130.34.116.246:/db/IPRT-SUN/BROWSE/pdf/",yy,mm,dd);
//	sprintf(cmd,"scp IPRT-SUN-Lv1-%04d%02d%02d-v2.pdf iprt@octave.gp.tohoku.ac.jp:/db/IPRT-SUN/BROWSE/pdf/",yy,mm,dd);
//	sprintf(cmd,"scp IPRT-SUN-Lv1-%04d%02d%02d-v2.pdf iprt@radio.gp.tohoku.ac.jp:/db/IPRT-SUN/BROWSE/pdf/",yy,mm,dd);
	printf("%s\n", cmd);
	system (cmd);

	return 0;


}
