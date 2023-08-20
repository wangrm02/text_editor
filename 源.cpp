#include<windows.h>
#include <stdio.h> 
#include<string.h>
#include <graphics.h>
#include <stdlib.h>
#define txt_scale 2000
//���ļ������д�0��ʼ

char txt_temp[txt_scale][txt_scale];
//���ݴ�ȡ˼·�����ļ��е����ݴ����ά�����У��ڳ������޸���ɺ��ٴ����ļ���
char filename[txt_scale];
int line_num,start_line,end_line;
bool is_open=false,is_modify=false,is_readme=false;
//��һЩ����������Ϊ���أ���¼�û��Ĳ�������ʵ�������������߼�

bool is_chinese(char* str)//�ж�str[0]�ǲ�������
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
	//��str[0]��ʼ�����ص�lie�еĵ�ַ��
	//��Ϊ����֧�ֶ�ȡ��д�����ģ���һ�������ַ���ռ�������У���������ֻռһ�У��˺�����Ϊ���õ��Ժ��û��������к�һ��
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
		MessageBox(0,"����������Χ���Ϸ�","�������",0);
		return NULL;
	}
	return str+i;
}

class rec_button//�����Ҳ���ΰ�ť
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
		settextstyle(20, 15, _T("����"));
		outtextxy(a1+20,a2+15,name);
	}

	bool is_trigger(ExMessage message)
	{
		// �ж�����Ƿ��ھ��η�Χ��
		return (message.x >= x && message.x <= x + width && 
			message.y >= y && message.y <= y + height);
	}
};

class showtext//��������ı���
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
		settextstyle(5, 15, _T("����"));
		fillrectangle(a1,a2,a1+a3,a2+a4);
	}
	void output_text(int low,int high)
		//����ӵ�low�е���high�е�����,�ڽ�������ʾָ����ĳ�е�ĳ�е�����
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
				outtextxy(5,5+(i-low+1)*20,temp);//��ʾ�к�
				char* t=NULL;
				if((t=count_lie(txt_temp[i],50,true))!=NULL)
				{
					strncpy(temp,txt_temp[i],(t-txt_temp[i]));
					temp[(t-txt_temp[i])]=temp[(t-txt_temp[i])+1]=temp[(t-txt_temp[i])+2]='.';
					//��һ�����ݹ������򽫳�����ά�����50�еĲ��ֽضϣ�
					//��Ϊ�����ַ�ռ���ж�Ӣ���ַ�ռһ�У�����count_lie���������ĽضϿ�����������
				}
				else
					strncpy(temp,txt_temp[i],50);
				
					
				outtextxy(30,5+(i-low+1)*20,temp);
				outtextxy(5,530,"----------------------------");
				outtextxy(5,550,"������ʾ��������25");
				//���²�����ʵ����ʾ�������������ɵ�����(��չ1)
				char file_line_count[50]="�ļ���������";
				itoa(line_num,temp,10);
				strcat(file_line_count,temp);
				outtextxy(5,570,file_line_count);
			}
		
		
	}
};

bool str_cmp(char*a,char*b)//�Լ�д��һ����Ա���������õ�str_cmp����
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
//��ҵҪ���ͳ��ĳ���ַ���ĳ��ָ���ַ����ĳ��ִ������Լ������Ҳ���ʾ�û������ĳ���ַ����ĸ���������Ҫ��Ƚ��������һ����������
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
void open_txt(char* filename) //�û������ļ�������������򿪣��������򱨴�
{
	if (*filename==0)
	{
		MessageBox(0,"�ļ�������Ϊ��","warning",0);
		return;
	}
	int i = 0;
	FILE *fp = fopen(filename,"r+");
	if (fp == NULL )
	{
		MessageBox(0,"�ļ�������","warning",0);
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

void new_txt(char* filename) //�û������ļ������������򱨴����������½�
{
	FILE *fp = fopen(filename,"r+");
	if (*filename==0)
	{
		MessageBox(0,"�ļ�������Ϊ��","warning",0);
		return;
	}
	else if(fp != NULL)
	{
		MessageBox(0,"�ļ�����ԭ���ļ���ͬ","warning",0);
		return;
	}
	else
	{
		memset(txt_temp,0,sizeof(txt_temp));
		int i = 0;
		FILE *fp = fopen(filename,"w");
		MessageBox(0,"�ļ��д����ɹ�","success",0);
		fclose(fp);
		line_num = 1;
	}
}


void cover(int line,char *cover)//�û������ַ�����Ȼ�󸲸�ָ����
{
	int i;
	if(line_num >1)
	{
		if(line<1||line>line_num)
		{
			MessageBox(0,"�кŲ���С��1�򳬹�������","warning",0);
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
		MessageBox(0,"�ļ�����Ϊ�գ��޷�ִ�иò���","warning",0);
		return;
	}
}

void Delete(int line)//ɾ��ָ����
{
	int i,j;
	if (line_num > 1)
	{
		if(line<1||line>line_num)
		{
			MessageBox(0,"�кŲ���С��1�򳬹�������","warning",0);
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
		MessageBox(0,"�ļ�����Ϊ�գ��޷�ִ�иò���","warning",0);
		return;
	}
}

void Add(int line,char *a)//��ĳһ�к��������
{
	int i,j;

	if(is_open == true)
	{
		if((line<1||line>line_num)&&(line_num>1||line!=0))
		{
			MessageBox(0,"�кŲ���С��1�򳬹�������","warning",0);
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
		MessageBox(0,"���ȴ򿪻򴴽�һ���ļ�","warning",0);
		return;
	}
}
void Change(char *a,char*b)//�滻�û�ָ�����ַ���Ϊ�µ��ַ���
{
	
	int n,i,j,k,f,flag;
	n = strlen(a);

	if (n==0)
	{
		MessageBox(0,"�ַ������Ȳ���Ϊ0","warning",0);
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
		MessageBox(0,"�ļ��в����ڸ��ַ������޷������滻","warning",0);
	}
}

void Change_special(char *b,int hang,int lie1,int lie2)
	//��չ 2���޸�ĳ��ָ��ĳ�е�ĳ�е�����
	//�����������ַ�����ʱ���û��ͻ�����Ϊ���кŲ�һ�������
{
	int k,f,n;

	if(line_num > 1)
	{
		if(hang<1||lie1<1||lie2<1)
		{
			MessageBox(0,"�кű������0","warning",0);
			return;
		}
		else if(hang>line_num||lie1>strlen(txt_temp[hang-1])||lie2>strlen(txt_temp[hang-1]))
		{
			MessageBox(0,"�кŹ���","warning",0);
			return;
		}
		else if(lie1>lie2)
		{
			MessageBox(0,"�кŵĿ�ͷֵ���ܳ�����βֵ","warning",0);
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
		MessageBox(0,"�ļ�����Ϊ�գ��޷�ִ�иò���","warning",0);
		return;
	}
}

void Move(int start,int end)//�ƶ�ĳ�л�ĳ���е�������
{
	if(start<1||end<1||start>line_num||end>line_num)
	{
		MessageBox(0,"�кŲ���С��1�򳬹�������","warning",0);
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
			MessageBox(0,"�ƶ��������յ��кŲ�����ͬ","warning",0);
			return;
		}
	}
	else
	{
		MessageBox(0,"�ļ�����Ϊ�գ��޷�ִ�иò���","warning",0);
		return;
	}
}

void Save(char* filename)//���洿�ı��ļ���ԭ·����ָ��·����
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
	rec_button _new(520,10,100,50,"�½�"),open(520,70,100,50,"��"),save(520,130,100,50,"����"),search(520,190,100,50,"����"),
		replace(520,250,100,50,"�滻"),edit(520,310,100,50,"�༭"),up(520,370,100,50,"�Ϸ�"),down(520,430,100,50,"�·�"),
		_print(520,490,100,50,"��ʾ"),ReadMeFirst(520,560,100,50,"˵��");
	
	ExMessage message;
	while(true)
	{
		getmessage(&message);
		if (message.message == WM_LBUTTONDOWN)
		{
			if(open.is_trigger(message) == true)
			{
				if(is_modify == true)//is_modify����û��Ƿ���ļ��������޸ģ���Ӧ�������û����б���
				{
					MessageBox(0,"���ȼ���Ƿ񱣴���ļ������е��޸�","warning",0);
				}
				InputBox(filename, txt_scale, "�������ļ���","�������ļ���","1.txt");
				open_txt(filename);
				start_line = 1;
				end_line = line_num;
				show_text_area.output_text(start_line,end_line);
				is_open=true;//is_open����û��Ƿ����һ���Լ����ļ���������readme˵���ĵ�������Ϊtrueʱ���ܽ��б༭����ҳ�Ȳ���
				is_readme=false;//is_readme����û��Ƿ����˵���ĵ�����Ϊtrue��ֻ�ܷ�ҳ��ָ����ʾ�У������ܽ��б༭
			}
			if (_new.is_trigger(message) == true)
			{
				if(is_modify == true)
				{
					MessageBox(0,"���ȼ���Ƿ񱣴���ļ������е��޸�","warning",0);
				}
				InputBox(filename, txt_scale, "�������ļ���","�������ļ���");
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
					InputBox(filename,txt_scale, "�������ļ���","�������ļ���",filename);
					Save(filename);
					is_modify = false;
				}
				else
				{
					MessageBox(0,"���ȴ򿪻򴴽�һ���ļ�","warning",0);
				}
			}
			if (search.is_trigger(message) == true)
			{
				int sum_i;
				char search_char[txt_scale]={0},sum_c[100]={0};
				if(is_open==false)
				{
					MessageBox(0,"���ȴ򿪻򴴽�һ���ļ�","warning",0);
				}
				else
				{
					InputBox(search_char, txt_scale, "������Ҫ���ҵ��ַ���","������Ҫ���ҵ��ַ���");
					if(*search_char == 0)
					{
						MessageBox(0,"�ַ������Ȳ���Ϊ0","warning",0);
					}
					else
					{
						sum_i = CountPlusSearch(search_char);
						itoa(sum_i,sum_c,10);
						MessageBox(0,sum_c,"���ַ������ֵĴ���",0);
					}
				}
			}
			if (replace.is_trigger(message) == true)
			{
				
				if(is_open == true&&is_readme==false)
				{
					char target[txt_scale],newchar[txt_scale];
					InputBox(target,txt_scale,"������Ҫ���滻���ַ���","������Ҫ���滻���ַ���");
					if (*target == 0)
					{
						MessageBox(0,"�ַ�������Ϊ��","warning",0);
					}
					else
					{
						InputBox(newchar,txt_scale,"�������µ��ַ���","�������µ��ַ���");

				         Change(target,newchar);

						 is_modify = true;
					}
				}
				else
				{
					MessageBox(0,"���ȴ򿪻򴴽�һ���ļ�","warning",0);
				}
			}
			if (edit.is_trigger(message) == true)
			{
				if(is_open == true&&is_readme==false)
				{
					char option[txt_scale];
					InputBox(option,txt_scale,"a:�����ַ�����Ȼ�󸲸�ָ����\nb:ɾ��ָ����\nc:��ĳһ�к��������\nd:�ƶ�ĳ�л�ĳ���е�������\ne:�޸�ĳ��ָ��ĳ�е�ĳ�е�����","��ѡ����Ҫ���еĲ���");
					if(strlen(option) == 1)
					{
						if(option[0] == 'a')
						{
							char cover_ea[txt_scale],line_ea_c[txt_scale];
							int line_ea;
							InputBox(cover_ea,txt_scale,"�������ַ���","�������ַ���");
							InputBox(line_ea_c,txt_scale,"������Ҫ�����ǵ��к�","������Ҫ�����ǵ��к�");
							line_ea = atoi(line_ea_c);
							cover(line_ea,cover_ea);
							is_modify = true;
						}
						else if(option[0] == 'b')
						{
							int line_eb;
							char line_eb_c[txt_scale];
							InputBox(line_eb_c,txt_scale,"������Ҫɾ�����к�","������Ҫɾ�����к�");
							line_eb = atoi(line_eb_c);
							Delete(line_eb);
							is_modify = true;
						}
						else if(option[0] == 'c')
						{
							int line_ec;
							char line_ec_c[txt_scale],add_ec[txt_scale];
							InputBox(add_ec,txt_scale,"������Ҫ�ӵ��ַ���","������Ҫ�ӵ��ַ���");
							InputBox(line_ec_c,txt_scale,"������Ҫ�����к������","������Ҫ�����к������");
							
							line_ec = atoi(line_ec_c);
							Add(line_ec,add_ec);
							is_modify = true;
						}
						else if(option[0] == 'd')
						{
							int start,end;
							char start_c[txt_scale],end_c[txt_scale];
							InputBox(start_c,txt_scale,"������Ҫ�ƶ����к�","������Ҫ�ƶ����к�");
							InputBox(end_c,txt_scale,"������Ҫ�ƶ�������","������Ҫ�ƶ�������");
							start = atoi(start_c);
							end = atoi(end_c);
							Move(start,end);
							is_modify = true;
						}
						else if(option[0] == 'e')
						{
							char replace[txt_scale],hang_r[txt_scale],lie1_r[txt_scale],lie2_r[txt_scale];
							int hang,lie1,lie2;
							InputBox(replace,txt_scale,"���������ַ���������","���������ַ���������");
							if(*replace == 0)
							{
								MessageBox(0,"�ַ�������Ϊ��","warning",0);
							}
							else 
							{
								InputBox(hang_r,txt_scale,"������Ŀ���ַ������ڵ��к�","������Ŀ���ַ������ڵ��к�");
								hang = atoi(hang_r);
								if(hang<1||hang>line_num)
								{
									MessageBox(0,"�кŲ��Ϸ�","warning",0);
								}
								else
								{
									InputBox(lie1_r,txt_scale,"������Ŀ���ַ����Ŀ�ͷ�к�","������Ŀ���ַ����Ŀ�ͷ�к�");
									lie1 = atoi(lie1_r);
									InputBox(lie2_r,txt_scale,"������Ŀ���ַ����Ľ�β�к�","������Ŀ���ַ����Ľ�β�к�");
									lie2 = atoi(lie2_r);
									Change_special(replace,hang,lie1,lie2);
								}
							}
							is_modify = true;
						}
						else
							MessageBox(0,"�����ֵ���Ϸ�","warning",0);
					}
					else
					{
						MessageBox(0,"�����ֵ���Ϸ�","warning",0);
					}
				}
				else
				{
					MessageBox(0,"���ȴ򿪻򴴽�һ���ļ�","warning",0);
				}
			}
			//�·���up��down��ʵ�����ϻ����·�ҳ�Ĺ��ܣ���չ1��
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
					MessageBox(0,"���ȴ򿪻򴴽�һ���ļ�","warning",0);
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
					MessageBox(0,"���ȴ򿪻򴴽�һ���ļ�","warning",0);
				}
			}
			if(_print.is_trigger(message) == true)//��ʵ�ֽ���ʾ�û��뿴�����кŷ�Χ������
			{
				if(is_open == true)
				{
					char start_p_c[txt_scale],end_p_c[txt_scale];
					int start_p,end_p;
					InputBox(start_p_c,txt_scale,"���������뿴���ĵ�һ���к�","���������뿴���ĵ�һ���к�");
					start_p = atoi(start_p_c);
					InputBox(end_p_c,txt_scale,"���������뿴�������һ���к�","���������뿴�������һ���к�");
					end_p = atoi(end_p_c);
					if(start_p<1||start_p>line_num||end_p<1||end_p>line_num||start_p>end_p)
					{
						MessageBox(0,"�кŲ��Ϸ�","warning",0);
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
					MessageBox(0,"���ȴ򿪻򴴽�һ���ļ�","warning",0);
				}
			}
			if(ReadMeFirst.is_trigger(message) == true)//˵���ĵ�
			{
				if(is_modify == true)
				{
					MessageBox(0,"���ȼ���Ƿ񱣴���ļ������е��޸�","warning",0);
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

