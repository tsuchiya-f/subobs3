#include <stdio.h>

int main()
{
	int yy, mm, dd,i;
	char fnm[100],fnm2[100],fnm3[100];

	sprintf(fnm3,"yes");
	printf("year: ");
	scanf("%d",&yy);
	printf("month: ");
	scanf("%d",&mm);
	printf("day: ");
	scanf("%d",&dd);

	printf("rm %04d%02d%02d_*.txt ? OK=0, NO=1::",yy,mm,dd);
	scanf("%d",&i);
//	printf("%s",fnm2);
	if(i==0){
           sprintf(fnm,"rm %04d%02d%02d_?.txt",yy,mm,dd);
           system (fnm);
           sprintf(fnm,"rm %04d%02d%02d_??.txt",yy,mm,dd);
           system (fnm);
           sprintf(fnm,"rm %04d%02d%02d_???.txt",yy,mm,dd);
           system (fnm);
/*
		for(i=0;i<=500;i++){
			sprintf(fnm,"rm %d%02d%02d_%d.txt",yy,mm,dd,i);
			system(fnm);	//ŽÀs
		}
*/
	}
	return 0;
}
