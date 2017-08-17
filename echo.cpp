#include"echo.h"

using namespace std;

int echo(char **argv)
{
	for(int i=1; argv[i]!=NULL; i++)
	{	
		if(argv[i][0] == '$') //means is a environ variable
			cout<<getenv(&argv[i][1])<<" ";
		else
			cout<<argv[i]<<" ";
	}
	cout<<endl;
	return 0;
}
