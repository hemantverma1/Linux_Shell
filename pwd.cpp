#include"pwd.h"

using namespace std;

int pwd(void)
{	
	char buf[1024];
	cout<<getcwd(buf, 1024)<<endl;
	return 0;
}
