#include<unistd.h>
#include<string>
#include<fstream>
#include<iostream>
#include<vector>

extern std::vector<std::string> history_vect;

#ifndef INC_EXIT_H
#define INC_EXIT_H
void exit_s(std::string filename);
#endif	/*INC_EXIT_H*/
