/*
分光器出力　2009年データ解析用
pgplot使用のQL作成プログラム
DSを作製
静穏時からの差分をデシベルであらわす
1枚のファイルを２回読み込む
LCPとRCPを別々のグラフで表示できる

ここから課題
色を自在につけられるようにする
時間を別ファイルから読み込んで自分でつけられるようにする

変更履歴：
2013/10/07　(Tsuchiya) 事後アーカイブ処理のために、岩井作AqDS.cを改修
改修内容
・処理を行うデータ日時をプログラム実行時に引数で与える（改修前：PCの時計から読み取り）
・ファイルパスの更新
2013/10/08　(Tsuchiya) AqDS.cからAqDS_Chk_Sat.cにコピー
改修内容
・8bitデータの飽和有無を確認する(赤：飽和有・青：飽和無・黒：BG以下)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <math.h>
#include "cpgplot.h"

double ConvertTodB(double linearValue)
{
	if(linearValue>0.0)return(-96.28+10.0*log10(linearValue+1.0));
	else return -100.0;
}


int main(int argc, char *argv[]){

	FILE *ptr,*ptr2;
	int yy,mm,dd,st_hr,st_mi,st_sec,ed_hr,ed_mi,ed_sec;
	time_t timer;
	
	int pic,file,spe,m;
	int speMax;	// = 60*440;//35200; //1枚のファイルに入っているスペクトルの本数
	int start, end;	//=23*3600+(5)*60+0;	//開始時間　UT0:00からの経過時間を秒で入力　将来的にはファイルから自動読み込みに
//色関係	
	int dBmax=22.5;		//表示の最大dBを規定　結構大事だけど目だってない
	int cimax=240,z=0,ci;	//色を何分割するかを決める  残念ながら240以下の数字しか受け付けない
	int zerocol=16;
	float c1,c2,c3;
	
	int fileMax = 1; //1枚の図に描くファイルの枚数
	int picMax = 1; //描く図の枚数
	float time,step=1000.0/1024.0;
	char fnm[100],fnm2[100];
	char ope[20];
	double data[16384],data1[16384];  //観測データ
	float ave[1024];
	size_t size;       //データサイズ指定

	if(argc != 2){
		printf("Usage : $AqDS YYYYMMDD\n");
		return 0;
	}

	sscanf(argv[1],"%04d%02d%02d",&yy,&mm,&dd);

	sprintf(ope,"../Debug/%04d%02d%02d/Qlhead_%04d%02d%02d.txt",yy,mm,dd,yy,mm,dd);
	sprintf(fnm,"../Debug/%4d%02d%02d_QLcheck.txt",yy,mm,dd);
	printf("%s\n",ope);
	ptr = fopen(ope, "r");
	if (ptr == NULL){
		puts("File Open Error: no ope_aq");
		return 0;
	}
	fscanf(ptr, "%02d%02d%02d", &st_hr,&st_mi,&st_sec);
	printf("# %d\n%d\n%d\n", st_hr,st_mi,st_sec);
	fscanf(ptr, "%02d%02d%02d", &ed_hr,&ed_mi,&ed_sec);
	printf("# %d\n%d\n%d\n", ed_hr,ed_mi,ed_sec);
	fclose(ptr);

	start = (-9+st_hr)*3600 + st_mi*60 + st_sec;
	end = (-9+ed_hr)*3600 + ed_mi*60 + ed_sec;
	speMax = end-start;
//	speMax=1000;

	for(m=0;m<16384;m++){
		data1[m]=0.0;
	}
//平均ファイル読み込み
	ptr2 = fopen("qsun_bg_low.out", "r");
	if (ptr2 == NULL){
		puts("File Open Error: no avefile");
		return 0;
	}
	for(m=0;m<1024;m++){
		fscanf(ptr2,"%f",&ave[m]);
//		printf("%d %f\n",m,ave[m]);
	}

	for(pic=0;pic<picMax;pic++){
		cpgopen("plot.gif/gif");
///////////////////////////////////////////////////////////////////
		for(z=0;z<cimax;z++){
			c1=360.0f-240.0f*z/(cimax-1.0f);	//120が赤360が青であることは決まっている
			c2=0.5f;
			c3=1.0f;
			cpgshls(z+16,c1,c2,c3);	/*z軸のカラー決定*/
		}
		cpgscir(zerocol,zerocol+cimax-1);	//カラー軸の上下決定。青:zerocol  赤:zerocol+cimax-1
///////////////////////////////////////////////////////////////////
//ここから最初の絵固有のルーティン
		cpgsvp(0,1,0,1);
		cpgswin(0,1,0,0.1);	
		cpgsvp(0.1f,0.85f,0.12f,0.42f);	//グラフの位置を決定
		cpgswin(start,start+speMax,100.f,500.f);	//グラフのX・Y座標の最初と最後を決定
		cpgtbox("bctsizhn",0.0f,0.5,"",0.0f,0);
		cpgbox("", 0.0, 0.0, "bctsizhn", 100.0, 2.0);
		cpglab("TIME UT","Frequency MHz","RCP");
		cpgsch(1.5);
//		cpgwedg("RI", 1.0f, 3.0f, 0, dBmax, "dB from BG");
		cpgsch(1.0);

		for(file=0;file<fileMax;file++){	
		//処理データファイル
			ptr = fopen(fnm, "rb");
			if (ptr == NULL){
				puts("File Open Error");
				return 0;
			}
			for(spe=0;spe<speMax;spe++){
				size = fread( &data, sizeof(double), 1024, ptr );
				time=(float)spe+(float)file*(float)speMax;	//startからの経過時間を計算している　ファイルが1枚しかなければ結局speMax
				for(m=0;m<size;m++){			  
					data[m] = ConvertTodB(data[m]);
					data[m] = data[m] - ave[m];
					if(data[m]>dBmax){
						ci=2;
					}else{
						ci = 11;
					}
/*
					ci=zerocol+data[m]*(cimax/dBmax);	//0～dBmax dBまでをプロットする
					if(ci>zerocol+cimax-1){
						ci=1;
					}else if(ci<zerocol){
						ci = 0;
					}
*/					cpgsci(ci);	//カラーとz軸の対応
					cpgrect(start+time,start+(time+1),step*(float)m,step*(float)(m+1) );	//小さなグリッドの座標x軸とy軸との対応
					if(time==1000){
//						printf("%f\n",data[m]);
//						printf("%d\n",ci);
					}
				}	  
			}
		}
		fclose(ptr);
//		cpgpage;
//ここから2枚目の絵固有のルーティン

		cpgswin(0,1,0,1);	//2枚目のグラフから背景のデフォルトが変わるから自分で指定
		cpgsci(0);	/*白*/
		cpgrect(0,1,1,2);	//
		cpgsci(1);	/*黒*/
		cpgswin(0,1,0.5,1);	
		cpgsvp(0.1f,0.85f,0.6f,0.9f);	//グラフの位置を決定
		cpgswin(start,start+speMax,100.f,500.f);	//グラフのX・Y座標の最初と最後を決定
		cpgtbox("bctsizhn",0.0f,0.5,"",0.0f,0);
		cpgbox("", 0.0, 0.0, "bctsizhn", 100.0, 2.0);
		cpglab("TIME UT","Frequency MHz","LCP");
		cpgsch(1.5);
//		cpgwedg("RI", 1.0f, 3.0f, 0, dBmax, "dB from BG");
		cpgsch(1.0);
		//処理データファイル
		for(file=0;file<fileMax;file++){	
			ptr = fopen(fnm, "rb");
			if (ptr == NULL){
				puts("File Open Error");
				return 0;
			}
			for(spe=0;spe<speMax;spe++){
				size = fread( &data, sizeof(double), 1024, ptr );
				time=(float)spe+(float)file*(float)speMax;
				for(m=0;m<size;m++){			  
					data[m] = ConvertTodB(data[m]);
					data[m] = data[m] - ave[m];
					if(data[m]>dBmax){
						ci=2;
					}else{
						ci = 11;
					}
/*
					ci=zerocol+data[m]*(cimax/dBmax);	//0～dBmax dBまでをプロットする
					if(ci>zerocol+cimax-1){
						ci=1;
					}else if(ci<zerocol){
						ci = 0;
					}
*/
					cpgsci(ci);	//カラーとz軸の対応
					cpgrect(start+time,start+(time+1),1050.0-step*(float)m,1050.0-step*(float)(m+1) );	//小さなグリッドの座標x軸とy軸との対応
				}	  
			}	  
		}

		cpgsvp(0,1,0,1);
		cpgswin(0,100,0,100);
		cpgsci(1);
		cpgtext(43, 1, "Kazumasa Iwai and Tohoku University. All rights reserved.");
		cpgsch(1.5);
		cpgtext(2, 96, "AMATERAS");
		cpgsch(1.35);
		sprintf(fnm2,"Solar Radio Spectra %d/%02d/%02d",yy,mm,dd);
		cpgtext(53, 96, fnm2);		
		fclose(ptr);
		cpgclos();
		sprintf(fnm2,"Chk2_IPRT_SUN_%02d%02d%02d.gif",yy,mm,dd);
		rename("plot.gif",fnm2);

	}
	return 0;
}

