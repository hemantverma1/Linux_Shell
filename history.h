#include<iostream>                                              
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>
#include<vector>
#include<string>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>

extern int total_hist;                                                                                                           
extern std::vector<std::string> history_vect;

#ifndef INC_HIST_H
#define INC_HIST_H
int history(char ** argv);
int if_file_exists(std::string filename);
int create_file(std::string filename);
int init_history(std::string filename);
int add_to_history(std::string line);
#endif	/*INC_HIST_H*/
