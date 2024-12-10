/*
������o�́@2008�N�f�[�^��͗p
pgplot�g�p��QL�쐬�v���O����
DS���쐻
�É�������̍������f�V�x���ł���킷
�t�@�C�����J��Ԃ��ǂݍ��ނ悤�ɂ��Ă���
*/

#include<stdio.h>
#include <math.h>
#include "cpgplot.h"

double ConvertTodB(double linearValue)
{
	if(linearValue>0.0)return(-96.28+10.0*log10(linearValue+1.0));
	else return -100.0;
}


int main(void){
	FILE *ptr,*ptr2;
	int aa,pic,file,spe,m,n;
	int speMax = 5000;//35200; //1���̃t�@�C���ɓ����Ă���X�y�N�g���̖{��
	int fileMax = 1; //1���̐}�ɕ`���t�@�C���̖���
	int picMax = 1; //�`���}�̖���
	float time;
//	int data;
	int year,hour,sec;
	char fnm[100],fnm2[100];
	double data[16384],data1[16384];  /* �ǂݍ��݃f�[�^�i�[�p   */
	float ave[1024];
	size_t size;       /* �ǂݍ��߂��f�[�^�T�C�Y */
	size_t i; 

//���σt�@�C���ǂݍ���
	ptr2 = fopen("0810.txt", "r");
	if (ptr2 == NULL){
		puts("File Open Error");
		return 0;
	}
	for(m=0;m<16384;m++){
		data1[m]=0.0;
//		fscanf(ptr2,"%f",&ave[m]);
//		printf("%f\n",ave[m]);
	}

	
	for(pic=0;pic<picMax;pic++){
		cpgopen("plot.gif/gif");
///////////////////////////////////////////////////////////////////
		int ci=120,z=0;	/*�F���x�����U�O�ɕ���*/
		float c1,c2,c3;
		for(z=0;z<ci;z++){
			c1=360.0f-240.0f*z/(ci-1.0f);	/*120����360����*/
			c2=0.5f;
			c3=1.0f;
			cpgshls(z+16,c1,c2,c3);	/*z���̃J���[����*/
		}
		cpgscir(16,135);	/*�J���[���̏㉺����B16����75���Ԃ���*/
///////////////////////////////////////////////////////////////////
		cpgpap(10.0, 0.5);
		cpgenv(0, 22, 0,1024, 0, 1);
//		cpgenv(0, fileMax*speMax/60, 0,1024, 0, 1);//���ŋK�i��
	  
		cpglab("TIME(sec)","Frequency","AqDS Ver1.0 ");
		cpgwedg("RI",1.0,3.0,16,135,"dB");
//		cpgsci(3);	  
//		cpgmove(0,powfunc(0));
		for(file=0;file<fileMax;file++){	
//�����f�[�^�t�@�C��
			aa = file + pic*fileMax;
//			sprintf(fnm,"%s_%dSUN4.txt",day,aa);
			sprintf(fnm,"test0415_%d.txt",aa);
//			printf("%c\n",fnm);
//			ptr = fopen(fnm, "rb");
			ptr = fopen("20130715_231.txt", "rb");
			if (ptr == NULL){
				puts("File Open Error");
				return 0;
			}

			for(spe=0;spe<speMax;spe++){
				size = fread( &data, sizeof(double), 16384, ptr );//1024 or 16384
				for(m=0;m<size;m++){			  
					data[m] = ConvertTodB(data[m]);
					time=(float)spe+(float)file*(float)speMax;
					if(time > 3000 && time < 4000){
						if(data1[m]>data[m]){
							data1[m]=data[m];
						}
					}
//					data[m] = data[m] - ave[m];
//					ci=(data[m]+88)*4+16;
//					ci=20+data[m]*4;	/*(f-f_min)/(f_max-f_min)�������قǐA�傫���قǐ�*/
//					if(ci<16){
//						ci=0;
//					}
//					else if(ci>135){
//						ci=135;
//					}
//					cpgsci(ci);	/*�J���[��z���̑Ή�*/
//					time=(float)spe+(float)file*(float)speMax;
//					if(time==15){
//						printf("%d\n",ci);
//					}
//					if(m==435){
//						printf("%d %f\n",spe,data[m]);
//					}
//
//					if(time >= 2880 && time < 3600){
//						cpgrect(time*5/60-60*0,(time+1)*5/60-60*0,(float)m/16,(float)(m+1)/16 );	/*�����ȃO���b�h�̍��Wx����y���Ƃ̑Ή�*/
//					}
//					cpgrect(time,(time+1),(float)m/16,(float)(m+1)/16 );	/*�����ȃO���b�h�̍��Wx����y���Ƃ̑Ή�*/
				}	  
			}	  
		}
		for(m=0;m<16384;m++){
			printf("%f %f\n",0.061*m, data1[m]);
//			printf("%d %f\n",m,data1[m]);
		}
		cpgclos();
//		sprintf(fnm2,"plot091023_l_2_m_%d.gif",pic);
		rename("plot.gif",fnm2);
	}

	return 0;
}

