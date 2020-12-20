// C program to implement one side of FIFO 
// This side writes first, then reads 
#include <iostream>
#include <sstream>
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;
int main(int argc, char *argv[]) 
{ 
    cout << "PROCESS IS WAITING " << endl;
    sleep(1);
    cout << "PROCESS IS FINISHING" << endl;
} 
