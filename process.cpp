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

// function declaration 
void mySignal(int sigNumber); 

char * PNumber;
using namespace std;
int main(int argc, char *argv[]) 
{ 

    PNumber = argv[1] ;

    cout << PNumber << " is waiting for a signal" << endl;


    signal(SIGHUP, mySignal); 
    signal(SIGINT, mySignal); 
    signal(SIGQUIT, mySignal); 
    signal(SIGHUP, mySignal); 
    signal(SIGINT, mySignal); 
    signal(SIGILL, mySignal); 
    signal(SIGTRAP, mySignal); 
    signal(SIGFPE, mySignal); 
    signal(SIGSEGV, mySignal); 
    signal(SIGTERM, mySignal); 
    signal(SIGXCPU, mySignal); 
    for (;;) {}
} 

// sighup() function definition 
void mySignal(int sigNumber) 

{ 
    signal(SIGHUP, mySignal); /* reset signal */
    cout << PNumber << " received signal " << sigNumber << endl;

} 