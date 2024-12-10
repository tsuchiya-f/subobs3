/*
スケジュールファイルをant/schに読みに行く
スケジュールファイルがなければ待機する
待機後再び読みに行く
読み込んだスケジュールファイルに応じた分光器スケジュールファイルを作る
*/

#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
//#include <stdafx.h>

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int showCmd)
{
	FILE *ptr, *ptr2;
	char file[100];
	char copy[100];
	char ope[100];	//分光器スケジュールファイル
	
	time_t timer;
    struct tm *now;
	
	int year;
	int mon;
	int day;

	char line[256];
	char reg[10] = "AZ:180";
	int azno=0;
	BOOL cpf;

	while(1){
//	日付と時刻を取得
		timer = time(NULL);
		now = localtime(&timer);

		year = now->tm_year+1900;
		mon = now->tm_mon+1;
		day = now->tm_mday;
	
//  スケジュールファイルのパスを作成
//		sprintf(file,"Z:\\sch\\SUN_%d%02d%02d.sch",year,mon,day);
		sprintf(file,"Y:\\sched_latest\\SUN_%d%02d%02d.sch",year,mon,day);
		sprintf(copy,"SUN_%d%02d%02d.sch",year,mon,day);
		printf("%s\n",file);
//  スケジュールファイルを読み込む
		cpf=CopyFile(file,copy,TRUE);//  スケジュールファイルを持ってくる。FALSEだとファイルの上書き
		printf("%d\n",cpf);
		printf("%d\n",GetLastError());
		ptr = fopen(copy, "r");
		if (ptr == NULL){
			puts("No sch for today. Waiting ...");
			Sleep(60000);//しばらく待つ
		}
		else break;
	}
//	分光器スケジュールファイルの作成
	sprintf(ope,"ope_aq_%d%02d%02d.txt",year,mon,day);
	ptr2 = fopen(ope, "w");
	while(fgets(line, 256, ptr) != NULL){	//1行づつ読み込む
		if(strstr(line, reg) != NULL){		//AZ:180のある行だけ選ぶ
			fprintf(ptr2, "%c%c\n", line[31], line[32]);
			fprintf(ptr2, "%c%c\n", line[34], line[35]);
			fprintf(ptr2, "%c%c\n", line[37], line[38]);
			azno = azno + 1;
		}
	}

	fclose(ptr);
	if(azno < 2){
		fprintf(ptr2, "スケジュールファイルに不具合があります");
	}
	fclose(ptr2);

	printf("azno:%d",azno);

}
