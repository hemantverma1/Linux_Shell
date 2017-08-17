#include"cd.h"

using namespace std;

int cd(char *argv)
{	
	char * name;
	char abs_path[1024];
	if(argv == NULL)	//no dir given
	{
		name = new char[strlen(getenv("HOME"))+1];
		strcpy(name, getenv("HOME"));
	}
	else if(argv[0] == '~')
	{
		string home = string(getenv("HOME"));
		name = new char[home.length() + strlen(argv) +1];
		home = home + string(&(argv[1]));
		strcpy(name, home.c_str());
	}
	else
		name = argv;
	realpath(name, abs_path);
	if(chdir(name))
	{
		if(errno == EACCES)
			cout<<"cd: "<<name<<": Permission denied"<<endl;
		else if(errno == ENOENT)
			cout<<"cd: "<<name<<": No such file or directory"<<endl;
		else if(errno == ENOTDIR)
			cout<<"cd: "<<name<<": Is not a directory"<<endl;
		else
			perror("");
		return 1;
	}
	setenv("PWD", abs_path, 1);
	return 0;
}
