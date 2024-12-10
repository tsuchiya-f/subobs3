/*
QL(gif)�ƒᕪ��\�f�[�^(1sec/1MHz fits����f�[�^)
��AMATERAS-PC(SUNOBS3)����f�[�^�T�[�o��copy
	
�X�V����
	2013/10/07�@(Tsuchiya) ����A�[�J�C�u�����̂��߂ɁA����copy_gif.c�����C
	���C���e
	�EWINMain -> main
	�E�������s���f�[�^�������v���O�������s���Ɉ����ŗ^����i���C�O�FPC�̎��v����ǂݎ��j
	�E�t�@�C���p�X�̍X�V
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

//	���t�Ǝ������擾
	if(argc != 2){
		printf("Usage : $copy_gif YYYYMMDD\n");
		return 0;
	}
	sscanf(argv[1],"%04d%02d%02d",&yy,&mm,&dd);

	sprintf(send,"IPRT_SUN_%02d%02d%02d.gif",yy,mm,dd);
	sprintf(copy,"Y:\\sun\\QL\\IPRT_SUN_%02d%02d%02d.gif",yy,mm,dd);
	CopyFile(send,copy,TRUE);//FALSE���ƃt�@�C���̏㏑��

	sprintf(send,"%04d%02d%02d_2FITS.txt",yy,mm,dd);	//FITS�p�t�@�C��add 20110303
	sprintf(copy,"Y:\\sun\\LOW\\%04d%02d%02d_2FITS.txt",yy,mm,dd);
	tryno=0;
	while(tryno <= 3){
		cpresult=CopyFile(send,copy,TRUE);//FALSE���ƃt�@�C���̏㏑��
		tryno = tryno + 1;
		if(cpresult == 0) {
			puts("NG");
			Sleep(60000);//�R�s�[�Ɏ��s������1���҂�
		}
		else break;
	}

	sprintf(send,"%04d%02d%02d_index2FITS.txt",yy,mm,dd);
	sprintf(copy,"Y:\\sun\\LOW\\%s",send);
	CopyFile(send,copy,FALSE);//FALSE���ƃt�@�C���̏㏑��

	return 0;


}
