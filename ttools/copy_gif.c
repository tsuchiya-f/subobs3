/*
QL(gif)と低分解能データ(1sec/1MHz fits源泉データ)
をAMATERAS-PC(SUNOBS3)からデータサーバにcopy
	
更新履歴
	2013/10/07　(Tsuchiya) 事後アーカイブ処理のために、岩井作copy_gif.cを改修
	改修内容
	・WINMain -> main
	・処理を行うデータ日時をプログラム実行時に引数で与える（改修前：PCの時計から読み取り）
	・ファイルパスの更新
*/

#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	time_t timer;
    struct tm *now;

	int yy;
	int mm;
	int dd;
	int tryno;
	char send[100],copy[100];
	int cpresult;

//	日付と時刻を取得
	if(argc != 2){
		printf("Usage : $copy_gif YYYYMMDD\n");
		return 0;
	}
	sscanf(argv[1],"%04d%02d%02d",&yy,&mm,&dd);

	sprintf(send,"IPRT_SUN_%02d%02d%02d.gif",yy,mm,dd);
	sprintf(copy,"Y:\\sun\\QL\\IPRT_SUN_%02d%02d%02d.gif",yy,mm,dd);
	CopyFile(send,copy,TRUE);//FALSEだとファイルの上書き

	sprintf(send,"%04d%02d%02d_2FITS.txt",yy,mm,dd);	//FITS用ファイルadd 20110303
	sprintf(copy,"Y:\\sun\\LOW\\%04d%02d%02d_2FITS.txt",yy,mm,dd);
	tryno=0;
	while(tryno <= 3){
		cpresult=CopyFile(send,copy,TRUE);//FALSEだとファイルの上書き
		tryno = tryno + 1;
		if(cpresult == 0) {
			puts("NG");
			Sleep(60000);//コピーに失敗したら1分待つ
		}
		else break;
	}

	sprintf(send,"%04d%02d%02d_index2FITS.txt",yy,mm,dd);
	sprintf(copy,"Y:\\sun\\LOW\\%s",send);
	CopyFile(send,copy,FALSE);//FALSEだとファイルの上書き

	return 0;


}
