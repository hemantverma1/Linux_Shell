#include"exit_s.h"
using namespace std;

void exit_s(string filename)
{
	std::fstream file_handle;
	file_handle.open(filename.c_str(), ios::trunc|ios::out);
	for(std::vector<std::string>::iterator it=history_vect.begin(); it!=history_vect.end(); it++)
		file_handle<<*it<<endl;
	file_handle.close();
	_exit(0);
}
