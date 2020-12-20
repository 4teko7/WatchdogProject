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
void sighup(int sigNumber); 
void sigint(int sigNumber); 
void sigquit(int sigNumber); 
// void sigint(int sigNumber); 
// void sigill(int sigNumber); 
// void sigstrap(int sigstrap); 
// void sigfpe(int sigfpe); 
// void sigsegv(int sigsegv); 
// void sigterm(int sigterm); 
// void sigxcpu(int sigxcpu); 

char * PNumber;
using namespace std;
int main(int argc, char *argv[]) 
{ 

    PNumber = argv[1] ;

    cout << PNumber << " is waiting for a signal" << endl;


    signal(SIGHUP, sighup); 
    signal(SIGINT, sigint); 
    signal(SIGQUIT, sigquit); 


    // signal(SIGHUP, mySignal); 
    // signal(SIGINT, mySignal); 
    // signal(SIGILL, mySignal); 
    // signal(SIGTRAP, mySignal); 
    // signal(SIGFPE, mySignal); 
    // signal(SIGSEGV, mySignal); 
    // signal(SIGTERM, mySignal); 
    // signal(SIGXCPU, mySignal); 
    for (;;) {}
} 

// sighup() function definition 
void sighup(int sigNumber) 

{ 
    cout << PNumber << " received signal " << sigNumber << endl;
    signal(SIGHUP, sighup); /* reset signal */
    cout << PNumber << " received signal " << sigNumber << endl;

} 

void sigint(int sigNumber) 

{ 
    signal(SIGINT, sigint); /* reset signal */
    printf(PNumber, " received signal " , sigNumber); 
} 

void sigquit(int sigNumber) 

{ 
    signal(SIGQUIT, sigquit); /* reset signal */
    printf(PNumber, " received signal " , sigNumber); 
} 

// // sigint() function definition 
// void sigint(int sigNumber) 

// { 
//     signal(SIGINT, sigint); /* reset signal */
//     printf("CHILD: I have received a SIGINT\n"); 
// } 

// void sigill(int sigNumber) {

// }
// void sigstrap(int sigstrap){

// }
// void sigfpe(int sigfpe){

// }
// void sigsegv(int sigsegv){

// }
// void sigterm(int sigterm){

// }
// void sigxcpu(int sigxcpu){

// }