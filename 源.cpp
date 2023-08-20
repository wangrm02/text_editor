#include<windows.h>
#include <stdio.h> 
#include<string.h>
#include <graphics.h>
#include <stdlib.h>
#define txt_scale 2000
//本文件所有行从0开始

char txt_temp[txt_scale][txt_scale];
//数据存取思路：将文件中的数据存入二维数组中，在程序中修改完成后再存入文件中
char filename[txt_scale];
int line_num,start_line,end_line;
bool is_open=false,is_modify=false,is_readme=false;
//用一些布尔变量作为开关，记录用户的操作，以实现清晰的运行逻辑

bool is_chinese(char* str)//判断str[0]是不是中文
{
    int i;

    if( str[0]==0||str[1]==0 ) return 0;

    for( i= 0; i < 2; i++)
    {
        if( str[i] < 161u || str[i] == 255u ) return 0;
       // if( !(i%2) && (str[i] < 176 || str[i] >= 248) ) return 0;
    }

    return 1;
}

char* count_lie(char* str,int lie,bool is_cut = false)
	//从str[0]开始，返回第lie列的地址；
	//因为程序支持读取和写入中文，而一个中文字符会占数组两列，但看起来只占一列，此函数是为了让电脑和用户看到的列号一样
{
	int lie_count=0;
	int i =0;
	for (i =0;i < strlen(str);)
	{
		if (lie_count>=lie)
				break;
		if (is_chinese(str+i))
		{
			
			if(is_cut)
				lie_count+=2;
			else
				lie_count++;
			if (lie_count>=lie)
				break;
			i+=2;
		}
		else
		{
			lie_count++;
			if (lie_count>=lie)
				break;
			
			i++;
		}
		
	}
	if (lie_count<lie)
	{
		if(!is_cut)
		MessageBox(0,"输入列数范围不合法","输入错误",0);
		return NULL;
	}
	return str+i;
}

class rec_button//绘制右侧矩形按钮
{
public:
	int x;
	int y;
	int width;
	int height;
	char name[50];

	rec_button()
	{
		x=y=width=height=0;
	}
	rec_button(int a1,int a2,int a3,int a4,char* na):
		x(a1),y(a2),width(a3),height(a4)
	{
		graphdefaults();
		int i = 0;
		while(na[i]!='\0')
		{
			name[i]=na[i];
			i++;
		}
		name[i]=0;
		setfillcolor(GREEN);
		setlinecolor(RED);
		setbkcolor(GREEN);
		fillrectangle(a1,a2,a1+a3,a2+a4);
		settextstyle(20, 15, _T("宋体"));
		outtextxy(a1+20,a2+15,name);
	}

	bool is_trigger(ExMessage message)
	{
		// 判断鼠标是否在矩形范围内
		return (message.x >= x && message.x <= x + width && 
			message.y >= y && message.y <= y + height);
	}
};

class showtext//绘制左侧文本框
{
public:
	int x;
	int y;
	int width;
	int height;
	

	showtext()
	{
		x=y=width=height=0;
	}
	showtext(int a1,int a2,int a3,int a4):
		x(a1),y(a2),width(a3),height(a4)
	{
		graphdefaults();
		setfillcolor(BLACK);
		settextstyle(5, 15, _T("宋体"));
		fillrectangle(a1,a2,a1+a3,a2+a4);
	}
	void output_text(int low,int high)
		//输出从第low行到第high行的内容,在界面上显示指定的某行到某行的内容
	{
		if (low <1)
			low=1;
		if (high > line_num)
			high=line_num;
		if(low>high)
			return;
		if (high > low+24)
			high = low+24;
		graphdefaults();
		setfillcolor(BLACK);
		fillrectangle(x,y,x+width,y+height);
		setbkcolor(BLACK);
		int i;
			for(i = low-1;i<high;i++)
			{
				char temp[60]={0};
				itoa(i+1,temp,10);
				outtextxy(5,5+(i-low+1)*20,temp);//显示行号
				char* t=NULL;
				if((t=count_lie(txt_temp[i],50,true))!=NULL)
				{
					strncpy(temp,txt_temp[i],(t-txt_temp[i]));
					temp[(t-txt_temp[i])]=temp[(t-txt_temp[i])+1]=temp[(t-txt_temp[i])+2]='.';
					//若一行数据过长，则将超过二维数组第50列的部分截断；
					//因为中文字符占两列而英文字符占一列，还用count_lie处理了中文截断可能乱码的情况
				}
				else
					strncpy(temp,txt_temp[i],50);
				
					
				outtextxy(30,5+(i-low+1)*20,temp);
				outtextxy(5,530,"----------------------------");
				outtextxy(5,550,"最多可显示的行数：25");
				//以下操作可实现显示界面可以最多容纳的行数(扩展1)
				char file_line_count[50]="文件总行数：";
				itoa(line_num,temp,10);
				strcat(file_line_count,temp);
				outtextxy(5,570,file_line_count);
			}
		
		
	}
};

bool str_cmp(char*a,char*b)//自己写了一个针对本程序更好用的str_cmp函数
{
	int i =0;
	if (*a==0||*b==0)
		return false;
	while(*(a+i)!=0&&*(b+i)!=0)
	{
		if (*(a+i)!=*(b+i))
			return false;
		i++;
	}
	return true;
}

int CountPlusSearch(char *a)
//作业要求里“统计某个字符或某个指定字符串的出现次数”以及“查找并显示用户输入的某个字符串的个数”功能要求比较像，因此用一个函数处理
{
	int sum = 0;

	int n,i,j,k;
	n = strlen(a);

	if (n==0)
	{
		return 0;
	}

	for(i = 0;i<line_num;i++)
	{
		j = 0;
		while(j<=strlen(txt_temp[i])-n&&strlen(txt_temp[i])>=n)
		{
			if (str_cmp(&txt_temp[i][j],a))
			{
				sum++;
				j+=n;
			}
			else
			{
				j++;
			}
		}
	}
	return(sum);
}
void open_txt(char* filename) //用户输入文件名，若存在则打开，不存在则报错
{
	if (*filename==0)
	{
		MessageBox(0,"文件名不能为空","warning",0);
		return;
	}
	int i = 0;
	FILE *fp = fopen(filename,"r+");
	if (fp == NULL )
	{
		MessageBox(0,"文件不存在","warning",0);
		return;
	}
	memset(txt_temp,0,sizeof(txt_temp));
	for(i = 0;i<txt_scale&&!feof(fp);i++)
	{
		fgets(txt_temp[i],txt_scale,fp);
		if (txt_temp[i][strlen(txt_temp[i])-1]=='\n'||txt_temp[i][strlen(txt_temp[i])-1]=='\r')
			txt_temp[i][strlen(txt_temp[i])-1]=0;
		if (txt_temp[i][strlen(txt_temp[i])-1]=='\n'||txt_temp[i][strlen(txt_temp[i])-1]=='\r')
			txt_temp[i][strlen(txt_temp[i])-1]=0;
	}
	line_num=i;
	fclose(fp);
}

void new_txt(char* filename) //用户输入文件名，若存在则报错，不存在则新建
{
	FILE *fp = fopen(filename,"r+");
	if (*filename==0)
	{
		MessageBox(0,"文件名不能为空","warning",0);
		return;
	}
	else if(fp != NULL)
	{
		MessageBox(0,"文件名与原有文件相同","warning",0);
		return;
	}
	else
	{
		memset(txt_temp,0,sizeof(txt_temp));
		int i = 0;
		FILE *fp = fopen(filename,"w");
		MessageBox(0,"文件夹创建成功","success",0);
		fclose(fp);
		line_num = 1;
	}
}


void cover(int line,char *cover)//用户输入字符串，然后覆盖指定行
{
	int i;
	if(line_num >1)
	{
		if(line<1||line>line_num)
		{
			MessageBox(0,"行号不能小于1或超过总行数","warning",0);
			return;
		}
		else
		{
			for(i = 0;i<strlen(cover);i++)
			{
				txt_temp[line-1][i] = cover[i];
			}
			txt_temp[line-1][i] = '\0';
		}
	}
	else
	{
		MessageBox(0,"文件内容为空，无法执行该操作","warning",0);
		return;
	}
}

void Delete(int line)//删除指定行
{
	int i,j;
	if (line_num > 1)
	{
		if(line<1||line>line_num)
		{
			MessageBox(0,"行号不能小于1或超过总行数","warning",0);
			return;
		}
		for(i = line-1;i<line_num-1;i++)
		{
			for(j = 0;j<strlen(txt_temp[i+1]);j++)
			{
				txt_temp[i][j] = txt_temp[i+1][j];
			}
			txt_temp[i][j] = '\0';
		}	
		txt_temp[line_num-1][0]='\0';
		line_num = line_num-1;
	}
	else
	{
		MessageBox(0,"文件内容为空，无法执行该操作","warning",0);
		return;
	}
}

void Add(int line,char *a)//在某一行后添加新行
{
	int i,j;

	if(is_open == true)
	{
		if((line<1||line>line_num)&&(line_num>1||line!=0))
		{
			MessageBox(0,"行号不能小于1或超过总行数","warning",0);
			return;
		}
		
		for(i = line_num;i>=line;i--)
		{
			memset(txt_temp[i],0,txt_scale);
			if (i==0)
				continue;
			for(j = 0;j<strlen(txt_temp[i-1]);j++)
			{
				txt_temp[i][j] = txt_temp[i-1][j];
			}
			txt_temp[i+1][j] = '\0';
		}

		for(j = 0;j<strlen(a);j++)
		{
			txt_temp[line][j] = a[j];
		}
		txt_temp[line][j] = '\0';
		line_num++;
	}
	else
	{
		MessageBox(0,"请先打开或创建一个文件","warning",0);
		return;
	}
}
void Change(char *a,char*b)//替换用户指定的字符串为新的字符串
{
	
	int n,i,j,k,f,flag;
	n = strlen(a);

	if (n==0)
	{
		MessageBox(0,"字符串长度不能为0","warning",0);
		return;
	}
	
		flag = 0;
	for(i = 0;i<line_num;i++)
	{
		j = 0;
		while(j<=strlen(txt_temp[i])-n&&strlen(txt_temp[i])>=n)
		{
			if (str_cmp(&txt_temp[i][j],a))
			{
				if(strlen(b)>n)
				{
					txt_temp[i][strlen(txt_temp[i])+(strlen(b)-n)] = '\0';
					for(f=strlen(txt_temp[i])+(strlen(b)-n-1);f>=j;f--)
					{
					txt_temp[i][f] = txt_temp[i][f+n-strlen(b)];
					}
					for(k = 0;k<strlen(b);k++)
					{
						txt_temp[i][j+k] = b[k];
					}
				}
				else if(strlen(b)<n)
				{
					for(f=j;f<strlen(txt_temp[i])-(n-strlen(b));f++)
					{
						txt_temp[i][f] = txt_temp[i][f+(n-strlen(b))];
					}
					txt_temp[i][strlen(txt_temp[i])-(n-strlen(b))] = '\0';
					for(k = 0;k<strlen(b);k++)
					{
						txt_temp[i][j+k] = b[k];
					}
				}
				else
				{
					for(k = 0;k<n;k++)
					{
						txt_temp[i][j+k] = b[k];
					}
				}
				j+=n;
				flag = 1;
			}
			else
			{
				j++;
			}
		}
	}
	if (flag == 0)
	{
		MessageBox(0,"文件中不存在该字符串，无法进行替换","warning",0);
	}
}

void Change_special(char *b,int hang,int lie1,int lie2)
	//扩展 2：修改某行指定某列到某列的内容
	//考虑了中文字符存在时，用户和机器认为的列号不一样的情况
{
	int k,f,n;

	if(line_num > 1)
	{
		if(hang<1||lie1<1||lie2<1)
		{
			MessageBox(0,"列号必须大于0","warning",0);
			return;
		}
		else if(hang>line_num||lie1>strlen(txt_temp[hang-1])||lie2>strlen(txt_temp[hang-1]))
		{
			MessageBox(0,"列号过大","warning",0);
			return;
		}
		else if(lie1>lie2)
		{
			MessageBox(0,"列号的开头值不能超过结尾值","warning",0);
			return;
		}
		else
		{
			char *p1=count_lie(txt_temp[hang-1],lie1);
			char *p2=count_lie(txt_temp[hang-1],lie2);
			if (is_chinese(p2))
				p2++;
			n = p2 - p1+1;
				char* f1;
				if(strlen(b)>n)
				{
					txt_temp[hang-1][strlen(txt_temp[hang-1])+(strlen(b)-n)] = '\0';
					for(f1=&txt_temp[hang-1][strlen(txt_temp[hang-1])+(strlen(b)-n-1)];f1 != p1;f1--)
					{
						*f1=*(f1+n-strlen(b));
					}
					
					for(k = 0;k<strlen(b);k++)
					{
						*(p1+k) = b[k];
					}
				}
				else if(strlen(b)<n)
				{
					
					for(f1=p1;f1<&txt_temp[hang-1][strlen(txt_temp[hang-1])+(strlen(b)-n)];f1++)
					{
						*f1=*(f1+n-strlen(b));
					}
					txt_temp[hang-1][strlen(txt_temp[hang-1])-(n-strlen(b))] = '\0';
					for(k = 0;k<strlen(b);k++)
					{
						*(p1+k) = b[k];
					}
				}
				else
				{
					for(k = 0;k<strlen(b);k++)
					{
						*(p1+k) = b[k];
					}
				}
			
		}
	}
	else
	{
		MessageBox(0,"文件内容为空，无法执行该操作","warning",0);
		return;
	}
}

void Move(int start,int end)//移动某行或某几行到其他行
{
	if(start<1||end<1||start>line_num||end>line_num)
	{
		MessageBox(0,"行号不能小于1或超过总行数","warning",0);
		return;
	}
	if(line_num > 1)
	{
		int i;
		char temp[txt_scale];
	
		strcpy(temp,txt_temp[start-1]);
		if(start<end)
		{
			for(i=start-1;i<end;i++)
			{
				strcpy(txt_temp[i],txt_temp[i+1]);
			}
			strcpy(txt_temp[end-1] ,temp);
		}
		else if(start>end)
		{
			for(i=start-2;i>end;i--)
			{
				strcpy(txt_temp[i+1],txt_temp[i]);
			}
			strcpy(txt_temp[end-1] ,temp);
		}
		else
		{
			MessageBox(0,"移动的起点和终点行号不能相同","warning",0);
			return;
		}
	}
	else
	{
		MessageBox(0,"文件内容为空，无法执行该操作","warning",0);
		return;
	}
}

void Save(char* filename)//保存纯文本文件到原路径或指定路径中
{
	int i;
	
		FILE* fp = fopen(filename,"w");
		for(i = 0;i<line_num;i++)
		{
			fputs(txt_temp[i],fp);
			fputs("\n",fp);
		}
		fclose(fp);
	
}


int main()
{
	initgraph(640, 630);
	setbkcolor(DARKGRAY);
	cleardevice();
	showtext show_text_area(0,0,500,630);
	rec_button _new(520,10,100,50,"新建"),open(520,70,100,50,"打开"),save(520,130,100,50,"保存"),search(520,190,100,50,"查找"),
		replace(520,250,100,50,"替换"),edit(520,310,100,50,"编辑"),up(520,370,100,50,"上翻"),down(520,430,100,50,"下翻"),
		_print(520,490,100,50,"显示"),ReadMeFirst(520,560,100,50,"说明");
	
	ExMessage message;
	while(true)
	{
		getmessage(&message);
		if (message.message == WM_LBUTTONDOWN)
		{
			if(open.is_trigger(message) == true)
			{
				if(is_modify == true)//is_modify检查用户是否对文件进行了修改，相应地提醒用户进行保存
				{
					MessageBox(0,"请先检查是否保存对文件所进行的修改","warning",0);
				}
				InputBox(filename, txt_scale, "请输入文件名","请输入文件名","1.txt");
				open_txt(filename);
				start_line = 1;
				end_line = line_num;
				show_text_area.output_text(start_line,end_line);
				is_open=true;//is_open检查用户是否打开了一个自己的文件（不包括readme说明文档），置为true时才能进行编辑、翻页等操作
				is_readme=false;//is_readme检查用户是否打开了说明文档，若为true则只能翻页、指定显示行，而不能进行编辑
			}
			if (_new.is_trigger(message) == true)
			{
				if(is_modify == true)
				{
					MessageBox(0,"请先检查是否保存对文件所进行的修改","warning",0);
				}
				InputBox(filename, txt_scale, "请输入文件名","请输入文件名");
				new_txt(filename);
				start_line = 1;
				end_line = line_num;
				is_open = true;
				is_readme=false;
			}
			if (save.is_trigger(message) == true)
			{
				if(is_open == true&&is_readme==false)
				{
					InputBox(filename,txt_scale, "请输入文件名","请输入文件名",filename);
					Save(filename);
					is_modify = false;
				}
				else
				{
					MessageBox(0,"请先打开或创建一个文件","warning",0);
				}
			}
			if (search.is_trigger(message) == true)
			{
				int sum_i;
				char search_char[txt_scale]={0},sum_c[100]={0};
				if(is_open==false)
				{
					MessageBox(0,"请先打开或创建一个文件","warning",0);
				}
				else
				{
					InputBox(search_char, txt_scale, "请输入要查找的字符串","请输入要查找的字符串");
					if(*search_char == 0)
					{
						MessageBox(0,"字符串长度不能为0","warning",0);
					}
					else
					{
						sum_i = CountPlusSearch(search_char);
						itoa(sum_i,sum_c,10);
						MessageBox(0,sum_c,"该字符串出现的次数",0);
					}
				}
			}
			if (replace.is_trigger(message) == true)
			{
				
				if(is_open == true&&is_readme==false)
				{
					char target[txt_scale],newchar[txt_scale];
					InputBox(target,txt_scale,"请输入要被替换的字符串","请输入要被替换的字符串");
					if (*target == 0)
					{
						MessageBox(0,"字符串不能为空","warning",0);
					}
					else
					{
						InputBox(newchar,txt_scale,"请输入新的字符串","请输入新的字符串");

				         Change(target,newchar);

						 is_modify = true;
					}
				}
				else
				{
					MessageBox(0,"请先打开或创建一个文件","warning",0);
				}
			}
			if (edit.is_trigger(message) == true)
			{
				if(is_open == true&&is_readme==false)
				{
					char option[txt_scale];
					InputBox(option,txt_scale,"a:输入字符串，然后覆盖指定行\nb:删除指定行\nc:在某一行后添加新行\nd:移动某行或某几行到其他行\ne:修改某行指定某列到某列的内容","请选择您要进行的操作");
					if(strlen(option) == 1)
					{
						if(option[0] == 'a')
						{
							char cover_ea[txt_scale],line_ea_c[txt_scale];
							int line_ea;
							InputBox(cover_ea,txt_scale,"请输入字符串","请输入字符串");
							InputBox(line_ea_c,txt_scale,"请输入要被覆盖的行号","请输入要被覆盖的行号");
							line_ea = atoi(line_ea_c);
							cover(line_ea,cover_ea);
							is_modify = true;
						}
						else if(option[0] == 'b')
						{
							int line_eb;
							char line_eb_c[txt_scale];
							InputBox(line_eb_c,txt_scale,"请输入要删除的行号","请输入要删除的行号");
							line_eb = atoi(line_eb_c);
							Delete(line_eb);
							is_modify = true;
						}
						else if(option[0] == 'c')
						{
							int line_ec;
							char line_ec_c[txt_scale],add_ec[txt_scale];
							InputBox(add_ec,txt_scale,"请输入要加的字符串","请输入要加的字符串");
							InputBox(line_ec_c,txt_scale,"请输入要在哪行后加新行","请输入要在哪行后加新行");
							
							line_ec = atoi(line_ec_c);
							Add(line_ec,add_ec);
							is_modify = true;
						}
						else if(option[0] == 'd')
						{
							int start,end;
							char start_c[txt_scale],end_c[txt_scale];
							InputBox(start_c,txt_scale,"请输入要移动的行号","请输入要移动的行号");
							InputBox(end_c,txt_scale,"请输入要移动到哪行","请输入要移动到哪行");
							start = atoi(start_c);
							end = atoi(end_c);
							Move(start,end);
							is_modify = true;
						}
						else if(option[0] == 'e')
						{
							char replace[txt_scale],hang_r[txt_scale],lie1_r[txt_scale],lie2_r[txt_scale];
							int hang,lie1,lie2;
							InputBox(replace,txt_scale,"请输入新字符串的内容","请输入新字符串的内容");
							if(*replace == 0)
							{
								MessageBox(0,"字符串不能为空","warning",0);
							}
							else 
							{
								InputBox(hang_r,txt_scale,"请输入目标字符串所在的行号","请输入目标字符串所在的行号");
								hang = atoi(hang_r);
								if(hang<1||hang>line_num)
								{
									MessageBox(0,"行号不合法","warning",0);
								}
								else
								{
									InputBox(lie1_r,txt_scale,"请输入目标字符串的开头列号","请输入目标字符串的开头列号");
									lie1 = atoi(lie1_r);
									InputBox(lie2_r,txt_scale,"请输入目标字符串的结尾列号","请输入目标字符串的结尾列号");
									lie2 = atoi(lie2_r);
									Change_special(replace,hang,lie1,lie2);
								}
							}
							is_modify = true;
						}
						else
							MessageBox(0,"输入的值不合法","warning",0);
					}
					else
					{
						MessageBox(0,"输入的值不合法","warning",0);
					}
				}
				else
				{
					MessageBox(0,"请先打开或创建一个文件","warning",0);
				}
			}
			//下方的up和down能实现向上或向下翻页的功能（扩展1）
			if(up.is_trigger(message) == true)
			{
				if(is_open == true)
				{
					if(start_line<1)
						start_line=1;
					start_line-=25;
					if (start_line<1)
						start_line=1;
					end_line=start_line+24;
					if(end_line>line_num)
						end_line=line_num;
				}
				else
				{
					MessageBox(0,"请先打开或创建一个文件","warning",0);
				}
			}
			if(down.is_trigger(message) == true)
			{
				if(is_open == true)
				{
					if(start_line<1)
						start_line=1;
					if (start_line+49<end_line+25)
					{
						end_line=start_line+49;
					}
					else
					{
						end_line+=25;
					}
					if(end_line>line_num)
						end_line=line_num;
					start_line=end_line-24;
					if (start_line<1)
						start_line=1;
					//end_line=start_line+24;
					
					
				}
				else
				{
					MessageBox(0,"请先打开或创建一个文件","warning",0);
				}
			}
			if(_print.is_trigger(message) == true)//可实现仅显示用户想看见的行号范围的内容
			{
				if(is_open == true)
				{
					char start_p_c[txt_scale],end_p_c[txt_scale];
					int start_p,end_p;
					InputBox(start_p_c,txt_scale,"请输入您想看见的第一行行号","请输入您想看见的第一行行号");
					start_p = atoi(start_p_c);
					InputBox(end_p_c,txt_scale,"请输入您想看见的最后一行行号","请输入您想看见的最后一行行号");
					end_p = atoi(end_p_c);
					if(start_p<1||start_p>line_num||end_p<1||end_p>line_num||start_p>end_p)
					{
						MessageBox(0,"行号不合法","warning",0);
					}
					else
					{
						if(start_line<1)
							start_line=1;
						start_line=start_p;
						end_line=end_p;
					}

				}
				else
				{
					MessageBox(0,"请先打开或创建一个文件","warning",0);
				}
			}
			if(ReadMeFirst.is_trigger(message) == true)//说明文档
			{
				if(is_modify == true)
				{
					MessageBox(0,"请先检查是否保存对文件所进行的修改","warning",0);
				}
				is_open=true;
				is_readme=true;
				int i;
				FILE *fp = fopen("readmefirst.txt","r");
				memset(txt_temp,0,sizeof(txt_temp));
				for(i = 0;i<txt_scale&&!feof(fp);i++)
				{
					fgets(txt_temp[i],txt_scale,fp);
					if (txt_temp[i][strlen(txt_temp[i])-1]=='\n'||txt_temp[i][strlen(txt_temp[i])-1]=='\r')
						txt_temp[i][strlen(txt_temp[i])-1]=0;
					if (txt_temp[i][strlen(txt_temp[i])-1]=='\n'||txt_temp[i][strlen(txt_temp[i])-1]=='\r')
						txt_temp[i][strlen(txt_temp[i])-1]=0;
				}
				line_num=i;
				fclose(fp);
				start_line=1;
				end_line = line_num;
			}
			show_text_area.output_text(start_line,end_line);
		}
	}
}

