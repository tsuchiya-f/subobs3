/*
FITS�쐬�p�v���O�����̍ŏ��̃A�v��
DS�������폜
����index2fits�����v���O����
new.exe����system�őg�ݍ���Ŏg�p

������o�́@2009�N�f�[�^��͗p
LCP��RCP��ʁX�̃O���t�ŕ\���ł���

�ύX�����F
2013/10/07�@(Tsuchiya) ����A�[�J�C�u�����̂��߂ɁA����AqDS_open_everyday.c�����C
���C���e
�E�������s���f�[�^�������v���O�������s���Ɉ����ŗ^����i���C�O�FPC�̎��v����ǂݎ��j
�E�t�@�C���p�X�̍X�V
�Eindex2FITS.txt�@->�@YYYYMMDD_index2FITS.txt
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
	int savedata=10;	//�ۑ��f�[�^�̃_�C�i�~�b�N�����W�����߂� 12:�ō�18=dB���ヂ�[�h
	int speMax;//35200; //1���̃t�@�C���ɓ����Ă���X�y�N�g���̖{��

	time_t timer;
	
	int fileMax = 1; //1���̐}�ɕ`���t�@�C���̖���
	int picMax = 1; //�`���}�̖���
	float time;
	char fnm[100],fnm2[100],fnm3[100],fnm4[100],fnm5[100];
	double data[16384],data1[16384];  //�ϑ��f�[�^
	float ave[1024];
	size_t size;       //�f�[�^�T�C�Y�w�� 
	unsigned char out,outr[409],outl[409]; //unsigned char = 0-255=8git , unsigned short int = 0-65535 = 16bit

	int yy, mm, dd, dur;//��͂��J�n����N�����Ɠ���
	int hh, min, ss;
	int ovflows, kazu,fino;
	float obstm;

	struct tm str_time;
	struct tm *now;
	struct tm *now2;
	time_t st_timer,addday;
 	
//���t����
	if(argc != 2){
		printf("Usage : $AqDS YYYYMMDD\n");
		return 0;
	}

	sscanf(argv[1],"%04d%02d%02d",&yy,&mm,&dd);

	dur=1;	//����1��
////////////////////////////

	for(m=0;m<16384;m++){
		data1[m]=0.0;
	}
//���σt�@�C���ǂݍ���
	ptr2 = fopen("qsun_bg_low.out", "r");
	if (ptr2 == NULL){
		puts("No min file File Open Error");
		return 0;
	}
	for(m=0;m<1024;m++){
		fscanf(ptr2,"%f",&ave[m]);
	}
	str_time.tm_sec = 0;       // �b 
	str_time.tm_min = 0;       // �� 
	str_time.tm_hour = 0;      // �� 
	str_time.tm_mday = dd;      //�� 
	str_time.tm_mon = mm - 1;       // �� ( 1����0 ) 
	str_time.tm_year = yy - 1900;    // ����N - 1900 
	str_time.tm_isdst = -1;    // �T�}�[�^�C���t���O 
	st_timer = mktime(&str_time);// ���� 1970 �N1��1������ϑ��J�n�����܂ł̌o�ߕb 
	now = localtime(&st_timer);//��͊J�n�����̔N�������i�[ JST

	sprintf(fnm5,"%04d%02d%02d_index2FITS.txt",now->tm_year+1900, now->tm_mon+1, now->tm_mday);
	fp2=fopen(fnm5,"w");
	fp3=fopen("lawdata2arc.txt","w");

	i=0;
	while(1){
		if (i>dur-1){	//��̓��[�v�I���̃��W�b�N���K�v
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
				str_time.tm_sec = ss;       // �b 
				str_time.tm_min = min;       // �� 
				str_time.tm_hour = hh;      // �� 
				str_time.tm_mday = now->tm_mday;      //�� 
				str_time.tm_mon = now->tm_mon;       // �� ( 1����0 ) 
				str_time.tm_year = now->tm_year;    // ����N - 1900 
				str_time.tm_isdst = -1;    // �T�}�[�^�C���t���O 
				st_timer = mktime(&str_time);// ���� 1970 �N1��1������ϑ��J�n�����܂ł̌o�ߕb 
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
			while(fscanf(ptr3,"%f %d %d %d",&obstm,&ovflows,&kazu,&fino) != EOF){ //�ϑ������X�y�N�g���̖{���𐔂���
				speMax = speMax + 1;
			}
			printf("%d\n",speMax);
			fprintf(fp2,"%d\n",speMax);
			fclose(ptr3);
	
//��������R�ŗL�̃��[�e�B��
			for(file=0;file<fileMax;file++){	
		//�����f�[�^�t�@�C��
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
					time=(float)spe+(float)file*(float)speMax;	//start����̌o�ߎ��Ԃ��v�Z���Ă���@�t�@�C����1�������Ȃ���Ό���speMax
					for(m=0;m<size;m++){			  
						data[m] = ConvertTodB(data[m]);
						data[m] = data[m] - ave[m];
						data[m] = savedata * (data[m] + 3) ; //QS��3dB�����Ă��牽���ُ펖�ԁB�f�[�^�ɂ��Ȃ����Ƃɂ���
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
//��������R�ŗL�̃��[�e�B��
		//�����f�[�^�t�@�C��
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
						data[m] = savedata * (data[m] + 3) ; //QS��3dB�����Ă��牽���ُ펖�ԁB�f�[�^�ɂ��Ȃ����Ƃɂ���
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

