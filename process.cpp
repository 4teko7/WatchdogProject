/**
 * @file process.cpp
 * @author Bilal Tekin (bilal.tekin@boun.edu.tr)
 * @brief The main idea of the project is to learn how to use namedpipe and communicate between parent and child processes. besides this, Checking the status of the children, and
 * doing necessary actions. There are 3 files which are executor.cpp, process.cpp, and watchdog.cpp.
 * Executor.cpp opens pipe and waits until watchdog writes the name and pids of the processes to the pipe. While watchdog writes this information,
 * executor reads from the pipe and do some actions according to instrucitons in the instruction file that is a parameter given to the executor.
 * There are many signal types in the instruction file. When signal for a specific process is activated by executor, that process receive signal and do some actions.
 * At the same time, watchdog watches the processes and when any process is terminated, it restart that process, and write the information of the newly created process to the pipe.
 * Then, executor reads that information from the pipe, and continue its job. 
 * @version 0.1
 * @date 2020-12-30
 * 
 * @copyright Copyright (c) 2020
 * 
 */
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

void mySignal(int sigNumber); 

/*! process Output Path */
char * processOutput;
/*! process number: P# */
char * PNumber; 
using namespace std;
/*! Process output stream */
fstream processOutputStream;
/**
 * @brief Main Function
 * 
 * @param argc Argument Count
 * @param argv Argument Values
 * @return int Return value
 */
int main(int argc, char *argv[]) { 

    processOutput = argv[1] ;
    PNumber = argv[2] ;

    processOutputStream.open(processOutput, ios::app);


    processOutputStream << PNumber << " is waiting for a signal\n";
    processOutputStream.flush();

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

/**
 * @brief Receives signal and do appropriate action.
 * 
 * @param sigNumber Signal number
 */
void mySignal(int sigNumber) { 
    if(sigNumber == 15) {
        processOutputStream << PNumber << " received signal " << sigNumber << ", terminating gracefully\n";
        processOutputStream.flush();
        processOutputStream.close();
        exit(0);
    } else {
        signal(sigNumber, mySignal);
        processOutputStream << PNumber << " received signal " << sigNumber << "\n";
        processOutputStream.flush();
    }

} 