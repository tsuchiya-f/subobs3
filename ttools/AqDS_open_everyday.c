/*
FITS作成用プログラムの最初のアプリ
DS部分を削除
毎日index2fitsを作るプログラム
new.exe中にsystemで組み込んで使用

分光器出力　2009年データ解析用
LCPとRCPを別々のグラフで表示できる

変更履歴：
2013/10/07　(Tsuchiya) 事後アーカイブ処理のために、岩井作AqDS_open_everyday.cを改修
改修内容
・処理を行うデータ日時をプログラム実行時に引数で与える（改修前：PCの時計から読み取り）
・ファイルパスの更新
・index2FITS.txt　->　YYYYMMDD_index2FITS.txt
*/

#include<stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

double ConvertTodB(double linearValue)
{
	if(linearValue>0.0)return(-96.28+10.0*log10(linearValue+1.0));
	else return -100.0;
}

double ConvertTodBm(double linearValue)
{
	double dbc;
	if(linearValue>20.3)return(-82.902+10.33*log10(linearValue+1.0));
	else if(linearValue>7.5){
		dbc=-96.28+10.0*log10(linearValue+1.0);
		return(-0.378*dbc*dbc - 62.01*dbc - 2612.2);
	}
	else return -90.0;
}

double dBcTodBm(double dBc)
{
	if(dBc>-83.0)return(16.555 + 1.033*dBc);
	else {
		return(-0.378*dBc*dBc - 62.01*dBc - 2612.2);
	}
}

int main(int argc, char *argv[]){
	
	FILE *ptr,*ptr2,*ptr3,*fp,*fp2,*fp3;
	int pic,file,spe,m,i;
	int savedata=10;	//保存データのダイナミックレンジを決める 12:最高18=dB微弱モード
	int speMax;//35200; //1枚のファイルに入っているスペクトルの本数

	time_t timer;
	
	int fileMax = 1; //1枚の図に描くファイルの枚数
	int picMax = 1; //描く図の枚数
	float time;
	char fnm[100],fnm2[100],fnm3[100],fnm4[100],fnm5[100];
	double data[16384],data1[16384];  //観測データ
	float ave[1024];
	size_t size;       //データサイズ指定 
	unsigned char out,outr[409],outl[409]; //unsigned char = 0-255=8git , unsigned short int = 0-65535 = 16bit

	int yy, mm, dd, dur;//解析を開始する年月日と日数
	int hh, min, ss;
	int ovflows, kazu,fino;
	float obstm;

	struct tm str_time;
	struct tm *now;
	struct tm *now2;
	time_t st_timer,addday;
 	
//日付決定
	if(argc != 2){
		printf("Usage : $AqDS YYYYMMDD\n");
		return 0;
	}

	sscanf(argv[1],"%04d%02d%02d",&yy,&mm,&dd);

	dur=1;	//毎日1枚
////////////////////////////

	for(m=0;m<16384;m++){
		data1[m]=0.0;
	}
//平均ファイル読み込み
	ptr2 = fopen("qsun_bg_low.out", "r");
	if (ptr2 == NULL){
		puts("No min file File Open Error");
		return 0;
	}
	for(m=0;m<1024;m++){
		fscanf(ptr2,"%f",&ave[m]);
	}
	str_time.tm_sec = 0;       // 秒 
	str_time.tm_min = 0;       // 分 
	str_time.tm_hour = 0;      // 時 
	str_time.tm_mday = dd;      //日 
	str_time.tm_mon = mm - 1;       // 月 ( 1月＝0 ) 
	str_time.tm_year = yy - 1900;    // 西暦年 - 1900 
	str_time.tm_isdst = -1;    // サマータイムフラグ 
	st_timer = mktime(&str_time);// 西暦 1970 年1月1日から観測開始時刻までの経過秒 
	now = localtime(&st_timer);//解析開始時刻の年月日を格納 JST

	sprintf(fnm5,"%04d%02d%02d_index2FITS.txt",now->tm_year+1900, now->tm_mon+1, now->tm_mday);
	fp2=fopen(fnm5,"w");
	fp3=fopen("lawdata2arc.txt","w");

	i=0;
	while(1){
		if (i>dur-1){	//解析ループ終了のロジックが必要
			fprintf(fp2,"0\n");
			fprintf(fp3,"0\n");
			printf("0\n");
			break;
		}

		now = localtime(&st_timer);
		sprintf(fnm4,"../Debug/%d%02d%02d_0.txt",now->tm_year+1900, now->tm_mon+1, now->tm_mday);
		printf("%s\n",fnm4);
		if( (ptr3=fopen(fnm4,"r")) != NULL){
			fprintf(fp3,"%d%02d%02d\n",now->tm_year+1900, now->tm_mon+1, now->tm_mday);
			fprintf(fp3,"-1\n");
			fclose(ptr3);
		}

		for(pic=0;pic<picMax;pic++){ //this loop is always "1"
			sprintf(fnm3,"../Debug/%04d%02d%02d/Qlhead_%d%02d%02d.txt",now->tm_year+1900, now->tm_mon+1, now->tm_mday,now->tm_year+1900, now->tm_mon+1, now->tm_mday);//start stop data
			printf("%s\n",fnm3);
			ptr3 = fopen(fnm3, "r");
			if (ptr3 == NULL){
				puts("File Open Error: start stop data");
				break;
			}
			fprintf(fp2,"%d\n%d\n%d\n",now->tm_year+1900, now->tm_mon+1, now->tm_mday);
			while(fscanf(ptr3,"%d %d %d",&hh,&min,&ss) != EOF){
				str_time.tm_sec = ss;       // 秒 
				str_time.tm_min = min;       // 分 
				str_time.tm_hour = hh;      // 時 
				str_time.tm_mday = now->tm_mday;      //日 
				str_time.tm_mon = now->tm_mon;       // 月 ( 1月＝0 ) 
				str_time.tm_year = now->tm_year;    // 西暦年 - 1900 
				str_time.tm_isdst = -1;    // サマータイムフラグ 
				st_timer = mktime(&str_time);// 西暦 1970 年1月1日から観測開始時刻までの経過秒 
				now = localtime(&st_timer);
				printf("%04d %02d %02d - ", now->tm_year+1900, now->tm_mon+1, now->tm_mday);
				printf("%02d %02d %02d JST\n", now->tm_hour, now->tm_min, now->tm_sec);
				now = gmtime(&st_timer);
				printf("%04d %02d %02d - ", now->tm_year+1900, now->tm_mon+1, now->tm_mday);
				printf("%02d %02d %02d UT\n", now->tm_hour, now->tm_min, now->tm_sec);
				fprintf(fp2,"%04d\n%02d\n%02d\n", now->tm_year+1900, now->tm_mon+1, now->tm_mday);
				fprintf(fp2,"%02d\n%02d\n%02d\n", now->tm_hour, now->tm_min, now->tm_sec);
				now = localtime(&st_timer);
			}
			fclose(ptr3);

			sprintf(fnm3,"../Debug/%04d%02d%02d/%d%02d%02d_head.txt",now->tm_year+1900, now->tm_mon+1, now->tm_mday,now->tm_year+1900, now->tm_mon+1, now->tm_mday);//time data
			printf("%s\n",fnm3);
			ptr3 = fopen(fnm3, "rb");
			if (ptr3 == NULL){
				puts("File Open Error: time data");
				break;
			}
			speMax=0;
			while(fscanf(ptr3,"%f %d %d %d",&obstm,&ovflows,&kazu,&fino) != EOF){ //観測したスペクトルの本数を数える
				speMax = speMax + 1;
			}
			printf("%d\n",speMax);
			fprintf(fp2,"%d\n",speMax);
			fclose(ptr3);
	
//ここからR固有のルーティン
			for(file=0;file<fileMax;file++){	
		//処理データファイル
				sprintf(fnm2,"../Debug/%d%02d%02d_QLcheck.txt",now->tm_year+1900, now->tm_mon+1, now->tm_mday);
				ptr = fopen(fnm2, "rb");
				if (ptr == NULL){
					puts("File Open Error");
					break;
				}
				else{
					printf("Now open file %s \n",fnm2);
				}

				sprintf(fnm,"%d%02d%02d_2FITS.txt",now->tm_year+1900, now->tm_mon+1, now->tm_mday);
				fp=fopen(fnm,"wb+");
				for(spe=0;spe<speMax;spe++){
					size = fread( &data, sizeof(double), 1024, ptr );
					time=(float)spe+(float)file*(float)speMax;	//startからの経過時間を計算している　ファイルが1枚しかなければ結局speMax
					for(m=0;m<size;m++){			  
						data[m] = ConvertTodB(data[m]);
						data[m] = data[m] - ave[m];
						data[m] = savedata * (data[m] + 3) ; //QSが3dB落ちてたら何か異常事態。データにしないことにする
						if(data[m]<0){
							data[m]=0.0;
						}
						if(data[m]>255){
							data[m]=255;
						}
						if(m>=103 && m<=512){
							outr[m-103] = data[m];
						}
					}
					fwrite(&outr,sizeof(out),410,fp);
				}
			}
			fclose(ptr);
//ここからR固有のルーティン
		//処理データファイル
			for(file=0;file<fileMax;file++){	
				sprintf(fnm2,"../Debug/%d%02d%02d_QLcheck.txt",now->tm_year+1900, now->tm_mon+1, now->tm_mday);
				ptr = fopen(fnm2, "rb");
				if (ptr == NULL){
					puts("File Open Error");
					break;
				}
				for(spe=0;spe<speMax;spe++){
					size = fread( &data, sizeof(double), 1024, ptr );
					time=(float)spe+(float)file*(float)speMax;
					for(m=0;m<size;m++){			  
						data[m] = ConvertTodB(data[m]);
						data[m] = data[m] - ave[m];
						data[m] = savedata * (data[m] + 3) ; //QSが3dB落ちてたら何か異常事態。データにしないことにする
						if(data[m]<0){
							data[m]=0.0;
						}
						if(data[m]>255){
							data[m]=255;
						}
						if(m>=564 && m<=973){
							outl[973-m]=data[m];
						}
					}
					fwrite(&outl,sizeof(out),410,fp);
				}	  
			}
			fclose(ptr);
			fclose(fp);		
		}//end of pic loop
		st_timer = st_timer + 24*3600;
		i=i+1;
	}//end of while loop
	fclose(fp2);
	fclose(fp3);
	return 0;
}

