#ifndef __MAIN__H_

#define __MAIN__H_

#include<iostream>
#include<string>
#include<stdio.h>
#include <algorithm>
#include <sys/stat.h>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h> 
#include <fstream>
#include <time.h>
#include <cstring>
//Header file for main program

//Functions
void ExecuteCommand(const std::string cmd,std::string & ans);
bool IsFileExists(const char* fname);
void CreateFile(const char* file);
std::string SHA256Enc(std::string tokenToEncrypt);
void CreateDirectory(const char* path, mode_t mode);
void recalculateHash(std::string block);

//global variables
static int nonce = 0;
static int currBlock = 1;
#endif
