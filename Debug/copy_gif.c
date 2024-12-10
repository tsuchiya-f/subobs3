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
	char send[100],copy[100];
	char cmd[200];

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

//	sprintf(send,"IPRT_SUN_%02d%02d%02d.gif",yy,mm,dd);
//	sprintf(copy,"Y:\\sun\\QL\\IPRT_SUN_%02d%02d%02d.gif",yy,mm,dd);
//	CopyFile(send,copy,TRUE);//FALSEだとファイルの上書き
	sprintf(cmd,"cp IPRT_SUN_%04d%02d%02d.gif /cygdrive/y/sun/QL/",yy,mm,dd);
	printf("%s\n", cmd);
	system(cmd);

//	sprintf(send,"Chk_IPRT_SUN_%02d%02d%02d.gif",yy,mm,dd);
//	sprintf(copy,"Y:\\sun\\QL\\Chk_IPRT_SUN_%02d%02d%02d.gif",yy,mm,dd);
//	CopyFile(send,copy,TRUE);//FALSEだとファイルの上書き
	sprintf(cmd,"cp Chk_IPRT_SUN_%04d%02d%02d.gif /cygdrive/y/sun/QL/",yy,mm,dd);
	printf("%s\n", cmd);
	system(cmd);

//	sprintf(send,"Chk2_IPRT_SUN_%02d%02d%02d.gif",yy,mm,dd);
//	sprintf(copy,"Y:\\sun\\QL\\Chk2_IPRT_SUN_%02d%02d%02d.gif",yy,mm,dd);
//	CopyFile(send,copy,TRUE);//FALSEだとファイルの上書き
	sprintf(cmd,"cp Chk2_IPRT_SUN_%04d%02d%02d.gif /cygdrive/y/sun/QL/",yy,mm,dd);
	printf("%s\n", cmd);
	system(cmd);

//	sprintf(send,"%04d%02d%02d_2FITS.txt",yy,mm,dd);	//FITS用ファイルadd 20110303
//	sprintf(copy,"Y:\\sun\\LOW\\%04d%02d%02d_2FITS.txt",yy,mm,dd);
//	CopyFile(send,copy,TRUE);//FALSEだとファイルの上書き
	sprintf(cmd,"cp %04d%02d%02d_2FITS.txt /cygdrive/y/sun/LOW/",yy,mm,dd);
	printf("%s\n", cmd);
	system(cmd);

//	sprintf(send,"index2FITS.txt",yy,mm,dd);
//	sprintf(copy,"Y:\\sun\\LOW\\index2FITS.txt",yy,mm,dd);
//	CopyFile(send,copy,FALSE);//FALSEだとファイルの上書き
	sprintf(cmd,"cp index2FITS.txt /cygdrive/y/sun/LOW/");
	printf("%s\n", cmd);
	system(cmd);

//	sprintf(send,"index2FITS.txt");
//	sprintf(copy,"index2FITS_%04d%02d%02d.txt",yy,mm,dd);
//	CopyFile(send,copy,FALSE);//FALSEだとファイルの上書き
	sprintf(cmd,"cp index2FITS.txt index2FITS_%04d%02d%02d.txt",yy,mm,dd);
	printf("%s\n", cmd);
	system(cmd);

//	sprintf(send,"index2FITS_%04d%02d%02d.txt",yy,mm,dd);
//	sprintf(copy,"Y:\\sun\\LOW\\index2FITS_%04d%02d%02d.txt",yy,mm,dd);
//	CopyFile(send,copy,FALSE);//FALSEだとファイルの上書き
	sprintf(cmd,"cp index2FITS.txt /cygdrive/y/sun/LOW/index2FITS_%04d%02d%02d.txt",yy,mm,dd);
	printf("%s\n", cmd);
	system(cmd);

	return 0;


}
