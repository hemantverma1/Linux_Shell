#include"parser.h"
using namespace std;

vector<struct command_type> parse(string line, int& ret)
{
	unsigned int i;
	int pipe_flag=0;
	std::vector<struct command_type > command_vect; //final vector to be returned by the function
	std::vector<std::vector<string> > commands;
	std::vector<string> words; //all words of a command
	string word;			   //represent a single word/token of a command
	ret = 0;
	for(i=0; i<=line.length(); i++)
	{
		if(i==line.length()) //parsed till the end - now store whatever is left in words vector
		{
			if(pipe_flag)	//means last character was a pipe and the command ends - syntax error
			{
				ret = -5;
				return command_vect;
			}
			if(word.length() > 0)
				words.push_back(word);
			word.clear();
			if(words.size() > 0)
				commands.push_back(words);
			words.erase(words.begin(), words.end());
			continue;
		}
		if(line[i] == ' ' || line[i] == '\t')
		{
			if(word.length() > 0)
				words.push_back(word);
			word.clear();
			continue;
			//while(i<line.length() && (line[i] == ' ' || line[i] == '\t'))
			//	i++;
		}
		else if(line[i] == '\'' || line[i] == '"' )
		{
			pipe_flag=0;
			string inside_qoute;
			int offset;
			offset = parse_qouted_string(line.substr(i), inside_qoute);
			if(offset==-1) //means it couldnt find ending qoute
			{
				ret=-1;	//No ending qoute found - error
				return command_vect;
			}
			else
			{
				word = inside_qoute;
				i+=offset;
				words.push_back(word);
				word.clear();
			}
		}
		else if(line[i] == '|' || line[i] == '&' || line[i] == '<' || line[i] == '>')
		{
			if(word.length() > 0)
				words.push_back(word);
			word.clear();
			if(line[i] == '|')
			{
				if(pipe_flag) //if pipe comes directly after a pipe - error
				{
					ret = -6;
					return command_vect;
				}
				if(words.size() <= 0) //no command and a pipe comes - error
				{
					ret = -7;
					return command_vect;
				}
				commands.push_back(words);
				words.erase(words.begin(), words.end());
				pipe_flag=1;
			}
			else
			{
				if(pipe_flag && line[i] == '&')
				{
					ret = -2; //ampersand cant come right after a pipe - error
					return command_vect;
				}
				if(line[i] == '&' && words.size() <= 0) //no command and an ampersand comes - error
				{
					ret = -8;
					return command_vect;
				}
				word+=line[i]; //add & or < or >
				words.push_back(word);
				word.clear();
			}
		}
		else
		{
			word+=line[i];
			pipe_flag=0;
		}
	}
	/*for(std::vector<vector <string> >::iterator it = commands.begin(); it!=commands.end(); it++)
	{
		for(std::vector<string>::iterator it2 = (*it).begin(); it2!=(*it).end(); it2++)
			cout<<*it2<<"\t";
		cout<<"\n";
	}*/
	for(std::vector<vector <string> >::iterator it = commands.begin(); it!=commands.end(); it++) //iterate over every command
	{
		words.erase(words.begin(), words.end()); //here variable "words" will be used to store all the args 
												 //of a command except redirection and backgrounding

		struct command_type tmp;				 //temporary structure as need to populate vector 
		tmp.background = false;
		for(std::vector<string>::iterator it2 = (*it).begin(); it2!=(*it).end(); it2++)
		{
			if(*it2 == ">")
			{
				it2++;
				if(it2!=(*it).end())
					tmp.output_redirect = *it2;
				else
				{
					ret = -3; //syntax error near output redirection operator
					return command_vect;
				}

			}
			else if(*it2 == "<")
			{
				it2++;
				if(it2!=(*it).end())
					tmp.input_redirect = *it2;
				else
				{
					ret = -4; //syntax error near input redirection operator
					return command_vect;
				}
			}
			else if(*it2 == "&")
				tmp.background = true;
			else
				words.push_back(*it2);
		}
		
		/*tmp.args = new char*[words.size()+1];
		for(i=0; i<words.size(); i++)
		{
			tmp.args[i] = new char[words[i].length()+1];
			strcpy(tmp.args[i], words[i].c_str());
		}
		tmp.args[i] = NULL;*/

		tmp.args = words;
		command_vect.push_back(tmp);

		/*for(i=0; i<tmp.args.size(); i++)
			cout<<"args["<<i<<"]: "<<tmp.args[i]<<endl;
		cout<<"input_redirect: "<<tmp.input_redirect<<endl;
		cout<<"output_redirect: "<<tmp.output_redirect<<endl;
		cout<<"backgrounding: "<<tmp.background<<endl;*/
	}
	return command_vect;
}

/* Following function takes source string as input which starts from a qoute;
parses the token inside qoutes and returns in inside_qoutes token;
returns -1 if qoutes are invalid(no ending qoute found  else returns the length of the token(to be used as offset)
*/
int parse_qouted_string(string source, string& inside_qoutes)
{
	unsigned int i;
	char initial_qoute = source[0]; //initial qoute should be the ending qoute
	for(i=1; i<source.length() && source[i]!=initial_qoute; i++)
		inside_qoutes += source[i];
	if(i>=source.length())	//means ending qoute never came
		return -1;
	else
		return i;
}



/*<scan and break on | OR & > -- check  pipe shouldnt come after a '&' 
--> & should be included inside a command only - strip the pipe
	<scan internal strings and make these flags on - input redirection, output redirection and backgrounding


int execvp(const char *file, char *const argv[]);
just give the program name and 
argv =	[0] --> [program name\0]
		[1] --> [-l\0]
		[2] --> [-w\0]
		.
		.
		.
		.
		[NULL]

fork()

vector of argv's i.e array of pointers to chars 

std::vector<char **>

*/
