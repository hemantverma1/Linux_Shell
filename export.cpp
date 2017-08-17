#include"export.h"
using namespace std;

int export_s(char *argv)
{
	int i;
	string var, value;
	if(argv == NULL)
	{
		cout<<"export: syntax error"<<endl;
		return 1;
	}
	for(i=0; argv[i]!='\0' && argv[i]!='='; i++)
		var+=argv[i];
	if(argv[i] == '\0')	//couldnt find a =
	{
		cout<<"export: syntax error"<<endl;
		return 1;
	}
	i++;
	for(; argv[i]!='\0'; i++)
		value+=argv[i];
	setenv(var.c_str(), value.c_str(), 1);
	return 0;
}
