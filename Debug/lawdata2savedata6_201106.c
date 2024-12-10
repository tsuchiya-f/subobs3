/*
Lv1データアーカイブ用プログラム
最初に8bit処理を全データに対して行う
その後、指定されたファイルについてのみ16bit処理を行う
16bit処理を行うファイル番号はcomfile2.txt内で指定する

連続ファイル処理可能
複数ファイル処理可能

DS部分を削除

分光器出力　2009年データ解析用

*/

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>

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

int main(void){
	FILE *ptr,*ptr2,*ptr3,*fp,*fp2;
	int pic,file,spe,m;
	int savedata;	//保存データのダイナミックレンジを決める 12:最高18=dB微弱モード
	int savedata16=1000;	//保存データのダイナミックレンジを決める 16bit保存モード専用 最高40=db
	int speMax = 12000;//35200; //1枚のファイルに入っているスペクトルの本数

	int fileMax; //1枚の図に描くファイルの枚数
	int picMax = 0; //描く図の枚数
	float time;
	char fnm[100],fnm2[100],fnm3[100];
	int bin=16384;
	double data[16384],data1[16384];  //観測データ
	float ave[1024];
	size_t size;       //データサイズ指定 
	int bit;
	unsigned char out,outr[6553],outl[6553]; //unsigned char = 0-255=8git , unsigned short int = 0-65535 = 16bit
	unsigned short int out16,outr16[6553],outl16[6553]; //unsigned char = 0-255=8git , unsigned short int = 0-65535 = 16bit

	int yy, mm, dd;
	int i;

	int ovflows, kazu,fino,fino2;
	float obstm;

	char dir[200],path[200],send[200],copy[200],cmd[200];	//ディレクトリ操作する時に使用

	for(m=0;m<bin;m++){
		data1[m]=0.0;
	}
//平均ファイル読み込み
	ptr2 = fopen("qsun_bg_high.out", "r");
	if (ptr2 == NULL){
		puts("File Open Error No minfile");
		return 0;
	}
	for(m=0;m<bin;m++){
		fscanf(ptr2,"%f",&ave[m]);
//		ave[m] = dBcTodBm(ave[m]);
//		printf("%f\n",ave[m]);
	}

//解析する日、解析bitを決めるファイルを読み込む
	fp2 = fopen("lawdata2arc.16bit.txt", "r");
	if (fp2 == NULL){
		puts("File Open Error No com file");
		return 0;
	}

	while(1){			//This is daily loop
		fscanf(fp2,"%04d",&yy);
		if(yy==0){
			printf("end of all data");
			break;
		}
		fscanf(fp2,"%02d",&mm);
		fscanf(fp2,"%02d",&dd);
//		fscanf(fp2,"%d",&bit);
//		printf("%d %d\n",dd,bit);

//以下はディレクトリ操作の部分。できたディレクトリを、その中にあるファイルごとサーバーにコピー
		GetCurrentDirectory(200,dir);	//カレントディレクトリを知る
		printf("%s\n",dir);
		sprintf(path,"%s\\%d%02d%02d",dir,yy,mm,dd);	//直下に日付に対応したディレクトリ
		_mkdir(path);			//新しいディレクトリを作る	

		sprintf(fnm3,"Qlhead_%d%02d%02d.txt",yy, mm, dd);//start stop dataをコピーする
		sprintf(send,"%s\\%s",dir,fnm3);	//元ディレクトリにあるコピーしたいファイル名
		sprintf(copy,"%s\\%s",path,fnm3);	//コピーした後のフルパス
		CopyFile(send,copy,TRUE);//FALSEだとファイルの上書き

		sprintf(fnm3,"%d%02d%02d_head.txt",yy, mm, dd);//time data を開く
		printf("%s\n",fnm3);
		ptr3 = fopen(fnm3, "rb");
		if (ptr3 == NULL){
			puts("File Open Error: time data");
			break;
		}
		sprintf(send,"%s\\%s",dir,fnm3);	//元ディレクトリにあるコピーしたいファイル名
		sprintf(copy,"%s\\%s",path,fnm3);	//コピーした後のフルパス
		CopyFile(send,copy,TRUE);//FALSEだとファイルの上書き
		while(fscanf(ptr3,"%f %d %d %d",&obstm,&ovflows,&kazu,&fino) != EOF){ //観測日のファイル数finoを数える
		}
		printf("No. of file is 0 to %d\n",fino);
		fclose(ptr3);

/*
/////////////////////////////////////////////////////////////////////////////////////
//8bitのルーティン開始 5ファイルを1つに保存　スケーリングファクター10で統一
		fileMax = 5;
		savedata = 10;
		picMax = (fino+1)/fileMax + 1;
		printf("8 bit file will be %d\n",picMax);
		for(pic=0;pic<picMax;pic++){
//			sprintf(fnm2,"20101002_Lv1_%d.txt",pic);
			sprintf(fnm2,"%s\\%d%02d%02d_Lv1_%d-%d.txt",path,yy,mm,dd,pic*fileMax,pic*fileMax+4);
			fp=fopen(fnm2,"wb+");
			printf("OK_8_2( ");
//ここからR固有のルーティン
			for(file=0;file<fileMax;file++){	
			//処理データファイル
//				sprintf(fnm,"20101002_%d.txt",pic*fileMax + file);
				sprintf(fnm,"%d%02d%02d_%d.txt",yy,mm,dd,pic*fileMax + file);
				printf("%d ",pic*fileMax + file);
				ptr = fopen(fnm, "rb");
				if (ptr == NULL){
					puts("File Open Error");
					break;
				}
				for(spe=0;spe<speMax;spe++){
					size = fread( &data, sizeof(double), bin, ptr );
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
						if(m>=1639 && m<=8192){	
							outr[m-1639] = data[m];
						}
					}
					fwrite(&outr,sizeof(out),6554,fp);
				}
				fclose(ptr);
			} //end of for (file) loop
//ここからR固有のルーティン
		//処理データファイル
			for(file=0;file<fileMax;file++){	
//			sprintf(fnm,"20101002_%d.txt",pic*fileMax + file);
				sprintf(fnm,"%d%02d%02d_%d.txt",yy,mm,dd,pic*fileMax + file);
				ptr = fopen(fnm, "rb");
				if (ptr == NULL){
					puts("File Open Error");
					break;
				}
				for(spe=0;spe<speMax;spe++){
					size = fread( &data, sizeof(double), bin, ptr );
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
						if(m>=9010 && m<=15563){
							outl[15563-m]=data[m];
						}
					}
					fwrite(&outl,sizeof(out),6554,fp);
				}
				fclose(ptr);
			} //end of for (file) loop
			fclose(fp);
			printf(")8 bit data of %d%02d%02d\n",yy,mm,dd);
		}//end of pic loop
//////////////////////////////////////////////////////////////////////////////////////////////
*/

//ここから16bit保存のルーティン。基本的にやっていることは同じで確保するメモリが大きくなっている。
//16bitは強いバーストが打た時だけ作成、16bit にするファイルは事前に決定しておく必要がある
		fileMax = 1;
		savedata16 =1000;
		printf("OK_16\n");
		while(1){
			fscanf(fp2,"%d",&fino);
			if(fino==-1){break;}	//奇数番目に-1が来るはず
			fino = fino/120;
			fscanf(fp2,"%d",&fino2);
			if(fino2==-1){
				printf("no enough info for lawdata2arc.16bit.txt");
				break;
			}
			fino2 = fino2/120;		//ここまでで高分解データ保存のスタートとストップを決定
			
			for(fino=fino;fino<fino2;fino++){

//				sprintf(fnm2,"20101002_Lv1_%d.txt",pic);
				sprintf(fnm2,"%s\\%d%02d%02d_Lv1_16bit_%d.dat",path,yy,mm,dd,fino);
				fp=fopen(fnm2,"wb+");
				printf("OK_16_2 ");
//ここからR固有のルーティン
				for(file=0;file<fileMax;file++){	
					//処理データファイル
//					sprintf(fnm,"20101002_%d.txt",pic*fileMax + file);
					sprintf(fnm,"%d%02d%02d_%d.txt",yy,mm,dd,fino);
					printf("( %d ) ",fino);
					ptr = fopen(fnm, "rb");
					if (ptr == NULL){
						puts("File Open Error");
						break;
					}
					for(spe=0;spe<speMax;spe++){
						size = fread( &data, sizeof(double), bin, ptr );
						time=(float)spe+(float)file*(float)speMax;	//startからの経過時間を計算している　ファイルが1枚しかなければ結局speMax
						for(m=0;m<size;m++){			  
							data[m] = ConvertTodB(data[m]);
							data[m] = data[m] - ave[m];
							data[m] = savedata16 * (data[m] + 3) ; //QSが3dB落ちてたら何か異常事態。データにしないことにする
							if(data[m]<0){
								data[m]=0.0;
							}
							if(data[m]>65535){
								data[m]=65535;
							}
							if(m>=1639 && m<=8192){	
								outr16[m-1639] = data[m];
							}
						}
						fwrite(&outr16,sizeof(out16),6554,fp);
					}
					fclose(ptr);
				}
//ここからR固有のルーティン
		//処理データファイル
				for(file=0;file<fileMax;file++){	
//					sprintf(fnm,"20101002_%d.txt",pic*fileMax + file);
					sprintf(fnm,"%d%02d%02d_%d.txt",yy,mm,dd,fino);
					ptr = fopen(fnm, "rb");
					if (ptr == NULL){
						puts("File Open Error");
						break;
					}
					for(spe=0;spe<speMax;spe++){
						size = fread( &data, sizeof(double), bin, ptr );
						time=(float)spe+(float)file*(float)speMax;
						for(m=0;m<size;m++){			  
							data[m] = ConvertTodB(data[m]);
							data[m] = data[m] - ave[m];
							data[m] = savedata16 * (data[m] + 3) ; //QSが3dB落ちてたら何か異常事態。データにしないことにする
							if(data[m]<0){
								data[m]=0.0;
							}
							if(data[m]>65535){
								data[m]=65535;
							}
							if(m>=9010 && m<=15563){
								outl16[15563-m]=data[m];
							}
						}
						fwrite(&outl16,sizeof(out16),6554,fp);
					}
					fclose(ptr);
				} //end of for (file) loop
				fclose(fp);
				printf("16 bit data of %d%02d%02d\n",yy,mm,dd);
			}	//end of for loop from start to stop

			// make PDF file
			sprintf(cmd,"./plot_lv1_16bit %04d%02d%02d", yy,mm,dd);
			printf("%s\n" ,cmd);
			system ( cmd );

		} //end of 16 bit while loop set of start and stop
	} //end of daily while loop
	fclose(fp2);
	
	
	return 0;
}

