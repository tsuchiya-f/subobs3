/*
Lv1�f�[�^�A�[�J�C�u�p�v���O����
�ŏ���8bit������S�f�[�^�ɑ΂��čs��
���̌�A�w�肳�ꂽ�t�@�C���ɂ��Ă̂�16bit�������s��
16bit�������s���t�@�C���ԍ���comfile2.txt���Ŏw�肷��

�A���t�@�C�������\
�����t�@�C�������\

DS�������폜

������o�́@2009�N�f�[�^��͗p

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
	int savedata;	//�ۑ��f�[�^�̃_�C�i�~�b�N�����W�����߂� 12:�ō�18=dB���ヂ�[�h
	int savedata16=1000;	//�ۑ��f�[�^�̃_�C�i�~�b�N�����W�����߂� 16bit�ۑ����[�h��p �ō�40=db
	int speMax = 12000;//35200; //1���̃t�@�C���ɓ����Ă���X�y�N�g���̖{��

	int fileMax; //1���̐}�ɕ`���t�@�C���̖���
	int picMax = 0; //�`���}�̖���
	float time;
	char fnm[100],fnm2[100],fnm3[100];
	int bin=16384;
	double data[16384],data1[16384];  //�ϑ��f�[�^
	float ave[1024];
	size_t size;       //�f�[�^�T�C�Y�w�� 
	int bit;
	unsigned char out,outr[6553],outl[6553]; //unsigned char = 0-255=8git , unsigned short int = 0-65535 = 16bit
	unsigned short int out16,outr16[6553],outl16[6553]; //unsigned char = 0-255=8git , unsigned short int = 0-65535 = 16bit

	int yy, mm, dd;
	int i;

	int ovflows, kazu,fino,fino2;
	float obstm;

	char dir[200],path[200],send[200],copy[200],cmd[200];	//�f�B���N�g�����삷�鎞�Ɏg�p

	for(m=0;m<bin;m++){
		data1[m]=0.0;
	}
//���σt�@�C���ǂݍ���
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

//��͂�����A���bit�����߂�t�@�C����ǂݍ���
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

//�ȉ��̓f�B���N�g������̕����B�ł����f�B���N�g�����A���̒��ɂ���t�@�C�����ƃT�[�o�[�ɃR�s�[
		GetCurrentDirectory(200,dir);	//�J�����g�f�B���N�g����m��
		printf("%s\n",dir);
		sprintf(path,"%s\\%d%02d%02d",dir,yy,mm,dd);	//�����ɓ��t�ɑΉ������f�B���N�g��
		_mkdir(path);			//�V�����f�B���N�g�������	

		sprintf(fnm3,"Qlhead_%d%02d%02d.txt",yy, mm, dd);//start stop data���R�s�[����
		sprintf(send,"%s\\%s",dir,fnm3);	//���f�B���N�g���ɂ���R�s�[�������t�@�C����
		sprintf(copy,"%s\\%s",path,fnm3);	//�R�s�[������̃t���p�X
		CopyFile(send,copy,TRUE);//FALSE���ƃt�@�C���̏㏑��

		sprintf(fnm3,"%d%02d%02d_head.txt",yy, mm, dd);//time data ���J��
		printf("%s\n",fnm3);
		ptr3 = fopen(fnm3, "rb");
		if (ptr3 == NULL){
			puts("File Open Error: time data");
			break;
		}
		sprintf(send,"%s\\%s",dir,fnm3);	//���f�B���N�g���ɂ���R�s�[�������t�@�C����
		sprintf(copy,"%s\\%s",path,fnm3);	//�R�s�[������̃t���p�X
		CopyFile(send,copy,TRUE);//FALSE���ƃt�@�C���̏㏑��
		while(fscanf(ptr3,"%f %d %d %d",&obstm,&ovflows,&kazu,&fino) != EOF){ //�ϑ����̃t�@�C����fino�𐔂���
		}
		printf("No. of file is 0 to %d\n",fino);
		fclose(ptr3);

/*
/////////////////////////////////////////////////////////////////////////////////////
//8bit�̃��[�e�B���J�n 5�t�@�C����1�ɕۑ��@�X�P�[�����O�t�@�N�^�[10�œ���
		fileMax = 5;
		savedata = 10;
		picMax = (fino+1)/fileMax + 1;
		printf("8 bit file will be %d\n",picMax);
		for(pic=0;pic<picMax;pic++){
//			sprintf(fnm2,"20101002_Lv1_%d.txt",pic);
			sprintf(fnm2,"%s\\%d%02d%02d_Lv1_%d-%d.txt",path,yy,mm,dd,pic*fileMax,pic*fileMax+4);
			fp=fopen(fnm2,"wb+");
			printf("OK_8_2( ");
//��������R�ŗL�̃��[�e�B��
			for(file=0;file<fileMax;file++){	
			//�����f�[�^�t�@�C��
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
						if(m>=1639 && m<=8192){	
							outr[m-1639] = data[m];
						}
					}
					fwrite(&outr,sizeof(out),6554,fp);
				}
				fclose(ptr);
			} //end of for (file) loop
//��������R�ŗL�̃��[�e�B��
		//�����f�[�^�t�@�C��
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
						data[m] = savedata * (data[m] + 3) ; //QS��3dB�����Ă��牽���ُ펖�ԁB�f�[�^�ɂ��Ȃ����Ƃɂ���
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

//��������16bit�ۑ��̃��[�e�B���B��{�I�ɂ���Ă��邱�Ƃ͓����Ŋm�ۂ��郁�������傫���Ȃ��Ă���B
//16bit�͋����o�[�X�g���ł��������쐬�A16bit �ɂ���t�@�C���͎��O�Ɍ��肵�Ă����K�v������
		fileMax = 1;
		savedata16 =1000;
		printf("OK_16\n");
		while(1){
			fscanf(fp2,"%d",&fino);
			if(fino==-1){break;}	//��Ԗڂ�-1������͂�
			fino = fino/120;
			fscanf(fp2,"%d",&fino2);
			if(fino2==-1){
				printf("no enough info for lawdata2arc.16bit.txt");
				break;
			}
			fino2 = fino2/120;		//�����܂łō������f�[�^�ۑ��̃X�^�[�g�ƃX�g�b�v������
			
			for(fino=fino;fino<fino2;fino++){

//				sprintf(fnm2,"20101002_Lv1_%d.txt",pic);
				sprintf(fnm2,"%s\\%d%02d%02d_Lv1_16bit_%d.dat",path,yy,mm,dd,fino);
				fp=fopen(fnm2,"wb+");
				printf("OK_16_2 ");
//��������R�ŗL�̃��[�e�B��
				for(file=0;file<fileMax;file++){	
					//�����f�[�^�t�@�C��
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
						time=(float)spe+(float)file*(float)speMax;	//start����̌o�ߎ��Ԃ��v�Z���Ă���@�t�@�C����1�������Ȃ���Ό���speMax
						for(m=0;m<size;m++){			  
							data[m] = ConvertTodB(data[m]);
							data[m] = data[m] - ave[m];
							data[m] = savedata16 * (data[m] + 3) ; //QS��3dB�����Ă��牽���ُ펖�ԁB�f�[�^�ɂ��Ȃ����Ƃɂ���
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
//��������R�ŗL�̃��[�e�B��
		//�����f�[�^�t�@�C��
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
							data[m] = savedata16 * (data[m] + 3) ; //QS��3dB�����Ă��牽���ُ펖�ԁB�f�[�^�ɂ��Ȃ����Ƃɂ���
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

