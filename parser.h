#include<vector>
#include<string>

#ifndef INC_PARSER_H
#define INC_PARSER_H
struct command_type
{
	std::vector<std::string> args;	//command arguments as needed in execvp system call
	std::string  input_redirect;
	std::string output_redirect;
	bool background;
};

int parse_qouted_string(std::string source, std::string& inside_qoutes);
std::vector<struct command_type> parse(std::string line, int& ret);
#endif	/*INC_PARSER_H*/
