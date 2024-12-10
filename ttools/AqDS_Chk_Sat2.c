/*
������o�́@2009�N�f�[�^��͗p
pgplot�g�p��QL�쐬�v���O����
DS���쐻
�É�������̍������f�V�x���ł���킷
1���̃t�@�C�����Q��ǂݍ���
LCP��RCP��ʁX�̃O���t�ŕ\���ł���

��������ۑ�
�F�����݂ɂ�����悤�ɂ���
���Ԃ�ʃt�@�C������ǂݍ���Ŏ����ł�����悤�ɂ���

�ύX�����F
2013/10/07�@(Tsuchiya) ����A�[�J�C�u�����̂��߂ɁA����AqDS.c�����C
���C���e
�E�������s���f�[�^�������v���O�������s���Ɉ����ŗ^����i���C�O�FPC�̎��v����ǂݎ��j
�E�t�@�C���p�X�̍X�V
2013/10/08�@(Tsuchiya) AqDS.c����AqDS_Chk_Sat.c�ɃR�s�[
���C���e
�E8bit�f�[�^�̖O�a�L�����m�F����(�ԁF�O�a�L�E�F�O�a���E���FBG�ȉ�)

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
	int speMax;	// = 60*440;//35200; //1���̃t�@�C���ɓ����Ă���X�y�N�g���̖{��
	int start, end;	//=23*3600+(5)*60+0;	//�J�n���ԁ@UT0:00����̌o�ߎ��Ԃ�b�œ��́@�����I�ɂ̓t�@�C�����玩���ǂݍ��݂�
//�F�֌W	
	int dBmax=22.5;		//�\���̍ő�dB���K��@���\�厖�����ǖڂ����ĂȂ�
	int cimax=240,z=0,ci;	//�F�����������邩�����߂�  �c�O�Ȃ���240�ȉ��̐��������󂯕t���Ȃ�
	int zerocol=16;
	float c1,c2,c3;
	
	int fileMax = 1; //1���̐}�ɕ`���t�@�C���̖���
	int picMax = 1; //�`���}�̖���
	float time,step=1000.0/1024.0;
	char fnm[100],fnm2[100];
	char ope[20];
	double data[16384],data1[16384];  //�ϑ��f�[�^
	float ave[1024];
	size_t size;       //�f�[�^�T�C�Y�w��

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
//���σt�@�C���ǂݍ���
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
			c1=360.0f-240.0f*z/(cimax-1.0f);	//120����360���ł��邱�Ƃ͌��܂��Ă���
			c2=0.5f;
			c3=1.0f;
			cpgshls(z+16,c1,c2,c3);	/*z���̃J���[����*/
		}
		cpgscir(zerocol,zerocol+cimax-1);	//�J���[���̏㉺����B��:zerocol  ��:zerocol+cimax-1
///////////////////////////////////////////////////////////////////
//��������ŏ��̊G�ŗL�̃��[�e�B��
		cpgsvp(0,1,0,1);
		cpgswin(0,1,0,0.1);	
		cpgsvp(0.1f,0.85f,0.12f,0.42f);	//�O���t�̈ʒu������
		cpgswin(start,start+speMax,100.f,500.f);	//�O���t��X�EY���W�̍ŏ��ƍŌ������
		cpgtbox("bctsizhn",0.0f,0.5,"",0.0f,0);
		cpgbox("", 0.0, 0.0, "bctsizhn", 100.0, 2.0);
		cpglab("TIME UT","Frequency MHz","RCP");
		cpgsch(1.5);
//		cpgwedg("RI", 1.0f, 3.0f, 0, dBmax, "dB from BG");
		cpgsch(1.0);

		for(file=0;file<fileMax;file++){	
		//�����f�[�^�t�@�C��
			ptr = fopen(fnm, "rb");
			if (ptr == NULL){
				puts("File Open Error");
				return 0;
			}
			for(spe=0;spe<speMax;spe++){
				size = fread( &data, sizeof(double), 1024, ptr );
				time=(float)spe+(float)file*(float)speMax;	//start����̌o�ߎ��Ԃ��v�Z���Ă���@�t�@�C����1�������Ȃ���Ό���speMax
				for(m=0;m<size;m++){			  
					data[m] = ConvertTodB(data[m]);
					data[m] = data[m] - ave[m];
					if(data[m]>dBmax){
						ci=2;
					}else{
						ci = 11;
					}
/*
					ci=zerocol+data[m]*(cimax/dBmax);	//0�`dBmax dB�܂ł��v���b�g����
					if(ci>zerocol+cimax-1){
						ci=1;
					}else if(ci<zerocol){
						ci = 0;
					}
*/					cpgsci(ci);	//�J���[��z���̑Ή�
					cpgrect(start+time,start+(time+1),step*(float)m,step*(float)(m+1) );	//�����ȃO���b�h�̍��Wx����y���Ƃ̑Ή�
					if(time==1000){
//						printf("%f\n",data[m]);
//						printf("%d\n",ci);
					}
				}	  
			}
		}
		fclose(ptr);
//		cpgpage;
//��������2���ڂ̊G�ŗL�̃��[�e�B��

		cpgswin(0,1,0,1);	//2���ڂ̃O���t����w�i�̃f�t�H���g���ς�邩�玩���Ŏw��
		cpgsci(0);	/*��*/
		cpgrect(0,1,1,2);	//
		cpgsci(1);	/*��*/
		cpgswin(0,1,0.5,1);	
		cpgsvp(0.1f,0.85f,0.6f,0.9f);	//�O���t�̈ʒu������
		cpgswin(start,start+speMax,100.f,500.f);	//�O���t��X�EY���W�̍ŏ��ƍŌ������
		cpgtbox("bctsizhn",0.0f,0.5,"",0.0f,0);
		cpgbox("", 0.0, 0.0, "bctsizhn", 100.0, 2.0);
		cpglab("TIME UT","Frequency MHz","LCP");
		cpgsch(1.5);
//		cpgwedg("RI", 1.0f, 3.0f, 0, dBmax, "dB from BG");
		cpgsch(1.0);
		//�����f�[�^�t�@�C��
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
					ci=zerocol+data[m]*(cimax/dBmax);	//0�`dBmax dB�܂ł��v���b�g����
					if(ci>zerocol+cimax-1){
						ci=1;
					}else if(ci<zerocol){
						ci = 0;
					}
*/
					cpgsci(ci);	//�J���[��z���̑Ή�
					cpgrect(start+time,start+(time+1),1050.0-step*(float)m,1050.0-step*(float)(m+1) );	//�����ȃO���b�h�̍��Wx����y���Ƃ̑Ή�
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

