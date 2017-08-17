#include<iostream>
#include"parser.h"
#include"cd.h"
#include"pwd.h"
#include"echo.h"
#include"exit_s.h"
#include"export.h"
#include"history.h"
#include<string>
#include<unistd.h>
#include<string.h>
#include <ctype.h> 
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <list>
#include <signal.h>

//int pipeline(int * p_pipe_fd, std::vector<struct command_type>& commands);

void inbuild_command_handler(char ** args);
int check_if_builtin(char * cmd);
void bang_op_handler(std::string& line);
void SIGhandler(int sig);
void fg(void);

int total_hist=0;
std::vector<std::string> history_vect;
std::string history_file;
std::list<int> children;
std::list<int> stopped_proc;

using namespace std;

int main()
{
	unsigned int i;
	int ret, j, status;
	string line;
	std::vector<struct command_type> commands;
	pid_t pid;
	history_file = string(getenv("HOME"));
	history_file += "/.myshell_history";
	init_history(history_file);
	signal(SIGINT, SIGhandler);
	signal(SIGTSTP, SIGhandler);
	while(1)
	{
		cout<<"["<<getenv("USER")<<":"<<getenv("PWD")<<"]$ ";
		if(std::getline(std::cin,line) == NULL)
			exit_s(history_file);
		if(line.length() > 0 && line[0] == '!')
			bang_op_handler(line);
		commands = parse(line, ret);
		if(ret<0)
		{
			cout<<"Syntax Error"<<endl; 
			add_to_history(line);
			cout.flush();
			continue;
		}
		if(commands.size()<1)
			continue;
		add_to_history(line);	
		int ** pipe_fd = new int * [commands.size()-1];
		for(i=0; i<commands.size()-1; i++)
			pipe_fd[i] = new int[2];
		j=0;
		for(std::vector<struct command_type>::iterator it = commands.begin(); it!=commands.end(); it++)
		{
			char ** args;
			string input_redirect;
			string output_redirect;
			bool background;
			int input_fd, output_fd;
			//copy command arguments
			args = new char*[(*it).args.size()+1];
			for(i=0; i<(*it).args.size(); i++)
			{
				args[i] = new char[(*it).args[i].length()+1];
				strcpy(args[i], (*it).args[i].c_str());
			}
			args[i] = NULL;
			//copy I/O redirection and backgrounding
			input_redirect = (*it).input_redirect;
			output_redirect = (*it).output_redirect;
			background = (*it).background;

			if(commands.size() == 1 && (!strcmp(args[0], "cd")|| !strcmp(args[0], "exit") || !strcmp(args[0], "export") || !strcmp(args[0], "fg"))) {
				inbuild_command_handler(args);		//dont fork and call in parent only
				break;								//break the current for-loop since there is only one command
			}	
			
			
			if((it+1) != commands.end())
				pipe(pipe_fd[j]); //create a new pipe	
			pid = fork();
			if(pid<0)
			{ 
				cout<<"Couldnt fork chiild process"<<endl; 
				exit(0);
			}
			else if(pid == 0) //child
			{
				if(it != commands.begin()){		//is not the very first command
					close(0);
					dup2(pipe_fd[j-1][0], 0);
					close(pipe_fd[j-1][0]);			//in any case, close the STDIN pipe_fd since has been duplicated
				}								//STDIN is changed to pipe[j]
				if((it+1) != commands.end()){	//is not the last command
					close(1);
					dup2(pipe_fd[j][1], 1);
					close(pipe_fd[j][1]);
				}
				if(input_redirect.length() > 0) //input file given
				{
					input_fd = open(input_redirect.c_str(), O_RDONLY);
					if(input_fd < 1){
						perror(input_redirect.c_str());
						exit(0);
					}
					dup2(input_fd, 0);
					close(input_fd);
				}
				if(output_redirect.length() >0)
				{
					output_fd = open(output_redirect.c_str(), O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH); 
					if(output_fd<0){
						perror(output_redirect.c_str());
						exit(0);
					}
					dup2(output_fd, 1);
					close(output_fd);
				}
				if(check_if_builtin(args[0])){
					inbuild_command_handler(args);
					exit(0);
				}
				if(background == true) {
					setsid();
				}
				execvp(args[0], args);
				perror(args[0]);
				exit(0);
			}
			else		//parent
			{
				if(!background){
					children.push_back(pid);
					while(1) {
						waitpid(pid, &status, WUNTRACED);
						if(WIFEXITED(status) || WIFSTOPPED(status) || WIFSIGNALED(status))
							break;
					}
				}
				else
					cout<<"["<<pid<<"]"<<endl;
				if((it+1) != commands.end())
					close(pipe_fd[j][1]);
				if(j-1 >= 0)
					close(pipe_fd[j-1][0]);
				for(std::list<int>::iterator it=children.begin(); it!=children.end(); it++)
					if(kill(*it, 0) != 0) //process doesnt exist in the system
						it = children.erase(it);
			}
			j++;
		}
		for(i=0; i<commands.size()-1; i++)
			free(pipe_fd[i]);
		free(pipe_fd);
	}
	return 0;
}


void inbuild_command_handler(char ** args)
{
	if(!strcmp(args[0], "cd"))
		cd(args[1]);
	else if(!strcmp(args[0], "pwd"))
		pwd();
	else if(!strcmp(args[0], "echo"))
		echo(args);
	else if(!strcmp(args[0], "exit"))
		exit_s(history_file);
	else if(!strcmp(args[0], "export"))
		export_s(args[1]);
	else if(!strcmp(args[0], "history"))
		history(args);
	else if(!strcmp(args[0], "fg"))
		fg();
}

int check_if_builtin(char * cmd)
{
	if(!strcmp(cmd, "cd")||!strcmp(cmd, "pwd")||!strcmp(cmd, "export")||!strcmp(cmd, "history")||!strcmp(cmd, "echo")||!strcmp(cmd, "exit"))
		return 1;
	else
		return 0;
}

void bang_op_handler(string& line)
{
	if(line[1] == '!' && total_hist>0) //means command is !!
		line = history_vect[total_hist-1];
	else if(line[1] == '-' || (line[1] >= '0' && line[1] <= 9+'0')) //negative number
	{
		string tmp = line.substr(1);
		int num = atoi(tmp.c_str());
		if(num > 0 && num <total_hist)
			line = history_vect[num-1];
		else if(num<0 && total_hist+num>=0)
			line = history_vect[total_hist+num];
	}
	else	//is a string after !
	{
		string tmp = line.substr(1);
		for(std::vector<std::string>::reverse_iterator it=history_vect.rbegin(); it!=history_vect.rend(); it++)
			if(tmp == (*it).substr(0,tmp.length())){
				line = *it;
				break;
			}
	}
}

void SIGhandler(int sig)
{
	signal(SIGINT, SIGhandler); //Set the kernel to call SIGhandler only -- kernel sometimes resets this to default behaviour
	signal(SIGTSTP, SIGhandler); //Set the kernel to call SIGhandler only -- kernel sometimes resets this to default behaviour
	if(sig == SIGINT)
	{
		if(children.size() > 0){
			for(std::list<int>::iterator it=children.begin(); it!=children.end(); it++)
				if(kill(*it, SIGINT) == 0) //kill it!
					it = children.erase(it);
		}
		else {
			cout<<endl;
			cout<<"["<<getenv("USER")<<":"<<getenv("PWD")<<"]$ ";
			cout.flush();
		}
	}
	else //SIGTSTP
	{
		if(children.size() >0){
			kill(*(children.rbegin()), SIGSTOP);
			stopped_proc.push_back(*(children.rbegin()));
			cout<<endl<<"["<<*(children.rbegin())<<"] Stopped"<<endl;
			cout.flush();
			children.pop_back();
		}
		else {
			cout<<endl;
			cout<<"["<<getenv("USER")<<":"<<getenv("PWD")<<"]$ ";
			cout.flush();
		}
	}
}

void fg(void)
{
	if(stopped_proc.size() > 0)
	{
		int tmp_pid = *(stopped_proc.rbegin());
		int status;
		stopped_proc.pop_back();		//so that shell starts waiting for it now
		children.push_back(tmp_pid);
		kill(tmp_pid, SIGCONT);
		while(1) {
			waitpid(tmp_pid, &status, WUNTRACED);
			if(WIFEXITED(status) || WIFSTOPPED(status) || WIFSIGNALED(status))
				break;
		}
	}
	else
		cout<<"There are no stopped processes"<<endl;
}

/*int pipeline(int * p_pipe_fd, std::vector<struct command_type>& commands)
{
	unsigned i;
	pid_t pid;
	char ** args;
	std::vector<struct command_type>::iterator it = commands.begin();
	string input_redirect;
	string output_redirect;
	bool background;
	int input_fd, output_fd;
	
	cout<<"in pipeline\n";
	cout.flush();
	//copy command arguments
	args = new char*[(*it).args.size()+1];
	for(i=0; i<(*it).args.size(); i++)
	{
		args[i] = new char[(*it).args[i].length()+1];
		strcpy(args[i], (*it).args[i].c_str());
	}
	args[i] = NULL;

	//copy I/O redirection and backgrounding
	input_redirect = (*it).input_redirect;
	output_redirect = (*it).output_redirect;
	background = (*it).background;

	//remove the command from vector 
	commands.erase(it);

	pid = fork();
	if(pid<0)
	{ 
		//do something 
	}
	else if(pid == 0) //child
	{
		cout<<args[0]<<endl;
		cout.flush();*/
		/*if(commands.size() > 0)	//still more are left
		{
			int pipe_fd[2];
			pipe(pipe_fd);
			dup2(pipe_fd[STDOUT_FILENO], STDOUT_FILENO);
			close(pipe_fd[STDOUT_FILENO]);
			pipeline(pipe_fd, commands);
			close(pipe_fd[STDIN_FILENO]);
		}*/
		/*if(input_redirect.length() > 0) //input file given
		{
			input_fd = open(input_redirect.c_str(), O_RDONLY);
			dup2(input_fd, STDIN_FILENO);
			close(input_fd);
		}
		if(output_redirect.length() >0)
		{
			output_fd = open(output_redirect.c_str(), O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH); 
			dup2(output_fd, STDOUT_FILENO);
			close(output_fd);
		}*/
		/*dup2(p_pipe_fd[0], 0);
		close(p_pipe_fd[0]);
		close(p_pipe_fd[1]);
		execvp(args[0], args);
		exit(0);
	}
	else		//parent
	{
		wait(NULL);
		return 0;
	}

}*/


/*
int main()
{
	<DS to store history and open/initialize a history file too ~/.bash_history>
	<Shell prompt>
		<parser> -- should return what to run and all input options in proper format >
		Create required number of pipes
		[0, Wrt_fd, Rd_fd, Wrt_fd, ...., Rd_fd, 1]
		while(number of elements in parsed vector>
		{
			pid = fork();
			if(in child)
				-- change their STDIN/STDOUT (based on pipe);
				-- changes STDIN/OUT based on redirection
				execvp;
				-- close pipe/file
			if(inparent)
				set STDOUT/IN to be used in next iteration
				make last process background if needed
		}
		//Either wait for last process or background it
	return 0;
}


<fork>
	<change STDOUT /STDIN as required
	<close STOUT and STDIN>
		<run command>
	<close files if any>
	<close pipes>
<exit>
*/
