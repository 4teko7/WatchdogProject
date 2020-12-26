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
#include <fstream>
// function declaration 
void mySignal(int sigNumber); 

char * processOutput; //Process Output path
char * PNumber; //P#
using namespace std;
fstream processOutputStream; //Process output stream
int main(int argc, char *argv[]) { 

    //Arguments taken from watchdog
    processOutput = argv[1] ;
    PNumber = argv[2] ;

    processOutputStream.open(processOutput, ios::app);


    processOutputStream << PNumber << " is waiting for a signal\n";
    processOutputStream.flush();

    //SIGNALS
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

    return 0;
} 

// sighup() function definition 
void mySignal(int sigNumber) { 
    if(sigNumber == 15) {
        processOutputStream << PNumber << " is received signal " << sigNumber << ",terminating gracefully\n";
        processOutputStream.flush();
        processOutputStream.close();
        exit(0);
    } else {
        signal(sigNumber, mySignal);
        processOutputStream << PNumber << " received signal " << sigNumber << "\n";
        processOutputStream.flush();
    }

} 