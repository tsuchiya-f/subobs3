/*
�X�P�W���[���t�@�C����ant/sch�ɓǂ݂ɍs��
�X�P�W���[���t�@�C�����Ȃ���Αҋ@����
�ҋ@��Ăѓǂ݂ɍs��
�ǂݍ��񂾃X�P�W���[���t�@�C���ɉ�����������X�P�W���[���t�@�C�������
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
	char ope[100];	//������X�P�W���[���t�@�C��
	
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
//	���t�Ǝ������擾
		timer = time(NULL);
		now = localtime(&timer);

		year = now->tm_year+1900;
		mon = now->tm_mon+1;
		day = now->tm_mday;
	
//  �X�P�W���[���t�@�C���̃p�X���쐬
//		sprintf(file,"Z:\\sch\\SUN_%d%02d%02d.sch",year,mon,day);
		sprintf(file,"Y:\\sched_latest\\SUN_%d%02d%02d.sch",year,mon,day);
		sprintf(copy,"SUN_%d%02d%02d.sch",year,mon,day);
		printf("%s\n",file);
//  �X�P�W���[���t�@�C����ǂݍ���
		cpf=CopyFile(file,copy,TRUE);//  �X�P�W���[���t�@�C���������Ă���BFALSE���ƃt�@�C���̏㏑��
		printf("%d\n",cpf);
		printf("%d\n",GetLastError());
		ptr = fopen(copy, "r");
		if (ptr == NULL){
			puts("No sch for today. Waiting ...");
			Sleep(60000);//���΂炭�҂�
		}
		else break;
	}
//	������X�P�W���[���t�@�C���̍쐬
	sprintf(ope,"ope_aq_%d%02d%02d.txt",year,mon,day);
	ptr2 = fopen(ope, "w");
	while(fgets(line, 256, ptr) != NULL){	//1�s�Âǂݍ���
		if(strstr(line, reg) != NULL){		//AZ:180�̂���s�����I��
			fprintf(ptr2, "%c%c\n", line[31], line[32]);
			fprintf(ptr2, "%c%c\n", line[34], line[35]);
			fprintf(ptr2, "%c%c\n", line[37], line[38]);
			azno = azno + 1;
		}
	}

	fclose(ptr);
	if(azno < 2){
		fprintf(ptr2, "�X�P�W���[���t�@�C���ɕs�������܂�");
	}
	fclose(ptr2);

	printf("azno:%d",azno);

}
