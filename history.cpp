#include"history.h"
using namespace std;

int history(char ** argv)
{	
	int cmd_no=1000;
	if(argv[1]!=NULL)
		cmd_no = atoi(argv[1]);
	int i = total_hist - cmd_no;
	if(i<0)
		i=0;
	for(; (unsigned int)i<history_vect.size(); i++)
		cout<<i+1<<" "<<history_vect[i]<<endl;
	return 0;
}

int if_file_exists(string filename)
{
	int ret;
	struct stat stat_data; // to gather output of stat system call
	ret = stat(filename.c_str(), &stat_data);
	if(ret == -1) // **** happens ;)
	{
		if(errno == ENOENT)
			return 0;
		else
			return -1;
	}
	else
		return 1;
}

int create_file(string filename)
{
	ofstream file_handle;
	file_handle.open(filename.c_str());
	file_handle.close();
	return 1;
}

int add_to_history(string line)
{
	if(total_hist > 0){
		if(line != history_vect[total_hist-1]) {
			history_vect.push_back(line);
			total_hist++;
		}
	}
	else {
		history_vect.push_back(line);
		total_hist++;
	}
	if(total_hist > 1000)
		history_vect.erase(history_vect.begin());
	return 0;
}

int init_history(string filename)
{
	std::fstream file_handle;
	if(!if_file_exists(filename)) //history file doesnt exist
	{
		create_file(filename);
		return 0;
	}
	string line;
	file_handle.open(filename.c_str(), ios::in);
	while(getline(file_handle, line))
		add_to_history(line);
	file_handle.close();
	return 0;
}
