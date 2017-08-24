#include<windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int GetCurTime(char* strTime)
{
	struct tm*		tmTime = NULL;
	size_t			timeLen = 0;
	time_t			tTime = 0;	
	
	tTime = time(NULL);
	tmTime = localtime(&tTime);
	//timeLen = strftime(strTime, 33, "%Y(Y)%m(M)%d(D)%H(H)%M(M)%S(S)", tmTime);
	timeLen = strftime(strTime, 33, "%Y.%m.%d %H:%M", tmTime);
	
	return timeLen;
}

void CreateVbs(char *tmpStr,int minutes)
{
	char str1[200]="2.VBS";
	char str2[200];
	char str3[1024]; 
	int m=0;
	sprintf(str2,"D:\\Documents\\%s",str1);
	FILE *fp=fopen(str2,"w");
	if(fp==NULL)
	{
		printf("can not find or create a .VBS file\n");	
		return;
	}
	else printf("open the VBS file succeed!\n");
	sprintf(str3,
	"CreateObject(\"SAPI.SpVoice\").Speak\"现在是北京时间%s，\
	您已经工作了%d小时%d分钟，请注意休息，\
	you have already worked for %d minutes, please pay attention to your health\"",
	tmpStr,minutes/60,minutes%60,minutes);
	m=fputs(str3,fp);
	if(m==0)
	{
		printf("语音写入成功！\n");
	}
	else printf("语音写入失败！\n");
	fclose(fp);
	return;
}

void TimeBroadcast(int i)//传入参数为秒数 
{
	int minutes=0;
	minutes=i/60;
	char tmpStr[64];
	char str[128];
	GetCurTime(tmpStr);//获取时间 
	sprintf(str, "%s", tmpStr);
	CreateVbs(str,minutes);//写入临时语音文件
	ShellExecuteA(0, "open", "D:\\Documents\\2.VBS", 0, 0, 1);//执行语音播报 
	Sleep(18000);//语音播报预留时间 
	return; 	
}

void main()
{
	int i=0;
	int j=0;
	int k=0;
	for(i=0;i<3600;i++)
	{
		//system("cls");
		printf("现在是第%d分钟又%d秒\n",i/60,i%60);
		Sleep(1000);
		if(i==3000)
		{
			for(j=0;j<10;j++)
			{
				for(k=0;k<3;k++)
				{
					Beep(1480,150);
					Sleep(30);	
				}
				Beep(1480,150);
				if(j==7) break;
				Sleep(400);
			}	
			TimeBroadcast(i);		
		}
	}
	system("pause");
}
