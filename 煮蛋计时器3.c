#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

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

void CreateVbs(char *tmpStr, int minutes, char *model)
{
	char str1[200] = "2.VBS";
	char str2[200];
	char str3[1024];
	int m = 0;
	sprintf(str2, "D:\\Documents\\%s", str1);
	FILE *fp = fopen(str2, "w");
	if (fp == NULL)
	{
		printf("can not find or create a .VBS file\n");
		return;
	}
	else printf("open the VBS file succeed!\n");
	if (*model == 'w')
	{
		sprintf(str3,
			"CreateObject(\"SAPI.SpVoice\").Speak\"现在是北京时间%s，\
			您已经工作了%d分钟，请注意休息，\
			you have already worked for %d minutes, please pay attention to your health,\
			you'd better have a rest\"",
		    tmpStr, minutes, minutes);
	}
	if (*model == 'r')
	{
		sprintf(str3,
			"CreateObject(\"SAPI.SpVoice\").Speak\"现在是北京时间%s，\
			您已经休息了%d分钟，请注意工作状态，\
			you have already rested for %d minutes, please pay attention to your work\"",
			tmpStr, minutes, minutes);
	}
	m = fputs(str3, fp);
	if (m == 0)
	{
		printf("语音写入成功！\n");
	}
	else printf("语音写入失败！\n");
	fclose(fp);
	return;
}

int TransNumbers(char *StrTmp, int *Number)
{
	int ret = 0;
	int count = 0;
	int i = 0, j = 0;
	int num = 0;
	char *tmp = StrTmp;
	while (*tmp)
	{
		count++;
		tmp++;
	}
	tmp = StrTmp;
	for (i = 0; i < count; i++, tmp++)
	{
		switch (*tmp)
		{
		case '0':num = 0; break;
		case '1':num = 1; break;
		case '2':num = 2; break;
		case '3':num = 3; break;
		case '4':num = 4; break;
		case '5':num = 5; break;
		case '6':num = 6; break;
		case '7':num = 7; break;
		case '8':num = 8; break;
		case '9':num = 9; break;
		default:num = 0; printf("fun TransNumbers() err: 没有读取到数字\n"); ret = -1; return ret;
		}
		for (j = i; j < count - 1; j++)
		{
			num = num * 10;
		}
		(*Number) += num;
	}
	return ret;
}

int Initial_TBT_Set(char *filename)
{
	FILE *fp = fopen(filename, "w");
	if (fp == NULL)
	{
		printf("fun Initial_TBT_Set() err:can not create a .ini file\n");
		return -1;
	}
	fputs("count minutes=50;\n", fp);
	fputs("model=work;\n", fp);
	fputs("music=1;\n", fp);
	fclose(fp);
	printf(".ini file created\n");
	return 0;
}

int ReadIni(char *filename, char *part, int *partValue)
{
	int ret = 0;
	char str1[200] = { 0 };
	char str2[220] = { 0 };
	strcpy(str1, filename);
	sprintf(str2, "D:\\Documents\\%s", str1);
	FILE *fp = fopen(str2, "r+");
	if (fp == NULL)
	{
		printf("fun ReadIni() err:can not open the file\n");
		ret = -1;
		return ret;
	}
	char str3[1024] = { 0 };
	int flag = 0;
	char filestr[1024] = { 0 };
	char partValueTmp[128] = { 0 };
	while (!feof(fp))
	{
		char *p = NULL;
		char *ptmp = NULL, *pBegain = NULL, *pEnd = NULL;
		fgets(str3, 128, fp);
		strcat(filestr, str3);
		int n = strlen(str3);
		str3[n + 1] = ';';//手动在字符串'\0'后添加';'结束符，防止字符串'='后什么都没有而中断出错
		p = strstr(str3, part);
		if (p != NULL)
		{
			flag = 1;//找到了配置项
			ptmp = strchr(str3, '=');
			if (ptmp == NULL)
			{
				printf("配置项%s没有'='！\n", part);
				//此处添加错误状态
				ret = -2;
				goto End;
			}
			ptmp = ptmp + 1;
			while (1)//读取配置项
			{
				if (*ptmp == ' ')
				{
					ptmp++;
				}
				else
				{
					if (*ptmp == '\n' || *ptmp == ';' || *ptmp == '\0')
					{
						system("color B");
						printf("配置项%s没有配置初始值！\n", part);
						system("pause");
						/*fclose(fp);
						FILE *fp = fopen(str2, "w");
						fputs("count minutes=50;", fp);
						*minutes = 50;*/
						ret = -3;
						goto End;
					}
					pBegain = ptmp;
					break;
				}
			}
			while (1)
			{
				if (*ptmp == ';' || *ptmp == '\n' || *ptmp == ' ' || *ptmp == '\0')
				{
					break;
				}
				else
					ptmp++;
			}
			pEnd = ptmp;
			memcpy(partValueTmp, pBegain, pEnd - pBegain);
			partValueTmp[pEnd - pBegain] = '\0';
			if (*partValue == 0)//如果传入的partValue地址的内容是0，说明传入的是数据
			{
				ret = TransNumbers(partValueTmp, partValue);
				if (ret != 0)
				{
					printf("fun ReadIni() err: TransNumbers()\n");
				}
				break;
			}
			else//否则传入的是字符串的地址，进行强制类型转换再拷贝
			{
				strcpy((char *)partValue, partValueTmp);
				break;
			}
		}
	}
End:
	if (flag == 0)
	{
		printf("没有找到%s配置项\n", part);
		ret = -4;
	}
	fclose(fp);
	return ret;
}

int CreateIni(char *filename)
{
	int ret = 0;
	if (filename == NULL)
	{
		printf("fun CreateIni() 传入地址不能为NULL\n");
		ret = -1;
		return ret;
	}
	if (*filename == (char)NULL)
	{
		printf("fun CreateIni() 传入的文件名不能为空\n");
		ret = -2;
		return ret;
	}
	char str1[200] = { 0 };
	char str2[220] = { 0 };
	strcpy(str1, filename);
	sprintf(str2, "D:\\Documents\\%s", str1);
	FILE *fp = fopen(str2, "r+");
	if (fp == NULL)
	{
		printf("can not find the necessary .ini file\n");
		system("color B");
		printf("you will create a TBTset.ini file\n");
		system("pause");
		ret = Initial_TBT_Set(str2);
		return ret;
	}
	fclose(fp);
	return ret;
}

void TimeBroadcast(int i, char *model)//传入参数为秒数 
{
	int minutes = 0;
	minutes = i / 60;
	char tmpStr[64];
	char str[128];
	GetCurTime(tmpStr);//获取当前时间 
	sprintf(str, "%s", tmpStr);
	CreateVbs(str, minutes, model);//写入临时语音文件
	ShellExecuteA(0, "open", "D:\\Documents\\2.VBS", 0, 0, 1);//执行语音播报 
	Sleep(18000);//语音播报预留时间 
	return;
}

void Music(int music)
{
	int j, k;
	if (music == 1)
	{
		for (j = 0; j < 4; j++)
		{
			for (k = 0; k < 3; k++)
			{
				Beep(1480, 150);
				Sleep(30);
			}
			Beep(1480, 150);
			if (j == 3) break;
			Sleep(1500);
		}
		return;
	}
	if (music == 2)
	{
		Beep(494, 200);
		Beep(523, 300);
		Beep(578, 400);
		Beep(659, 500);
		Beep(698, 600);
		Beep(784, 700);
		Beep(880, 800);
		Beep(932, 900);
		Beep(1046, 1000);
		Beep(1175, 1100);
		Beep(1381, 1200);
		Beep(1480, 1300);
		Beep(1661, 1400);
		return;
	}
}

void main()
{
	int ret = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	int minutes = 0;
	char model[20] = { "work" };//由于ReadIni函数限制，字符串请注意不要初始化为0
	int music = 0;
	ret = CreateIni("TBTset.ini");
	if (ret != 0)
	{
		printf("fun main() err: CreateIni()\n");
		system("pause");
		return;
	}
	ret = ReadIni("TBTset.ini", "count minutes", &minutes);
	if (ret != 0)
	{
		printf("fun main() err: ReadIni(count minutes)\n");
		system("pause");
		return;
	}
	ret = ReadIni("TBTset.ini", "music", &music);
	if (ret != 0)
	{
		printf("fun main() err: ReadIni(music)\n");
		system("pause");
		return;
	}
	ret = ReadIni("TBTset.ini", "model", model);
	if (ret != 0)
	{
		printf("fun main() err: ReadIni(model)\n");
		system("pause");
		return;
	}
	for (i = 0; i<3600; i++)
	{
		//system("cls");
		printf("现在是第%d秒\n", i);
		Sleep(1000);
		if (i == minutes * 60)
		{
			Music(music);
			TimeBroadcast(i, model);
		}
	}
	system("pause");
}
