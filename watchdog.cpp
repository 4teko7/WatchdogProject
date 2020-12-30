/**
 * @file watchdog.cpp
 * @author Bilal Tekin (bilal.tekin@boun.edu.tr)
 * @brief The main idea of the project is to learn how to use namedpipe and communicate between parent and child processes. besides this, Checking the status of the children, and
 * doing necessary actions. There are 3 files which are executor.cpp, process.cpp, and watchdog.cpp.
 * Executor.cpp opens pipe and waits until watchdog writes the name and pids of the processes to the pipe. Watchdog creates processes according to input file. While watchdog writes processes information,
 * executor reads from the pipe and do some actions according to instrucitons in the instruction file that is a parameter given to the executor.
 * There are many signal types in the instruction file. When signal for a specific process is activated by executor, that process receive signal and do some actions. For example, when process takes SIGTERM
 * signal, it exits from the program. Watchdog should wait Its children. If Watchdog does not do this, Its children will become Zombie, which is an unwanted situation.
 * When a child is terminated, Watchdog will get the information about the terminated child thanks to wait function, and will restart the terminated child. For executor to continue its job, Watchdog should
 * write the information about the newly created child to the pipe. Then, executor reads that information from the pipe, and continue its job. If Watchdog receives SIGTERM signal, then before terminating Watchdog,
 * we should terminate all Its children. If we don't terminate children, they will become orphan and this is an unwanted situation. \n
 * Explanation Of The Code Structure: \n
 * Watchdog opens the pipe, and writes its own information to the pipe.
 * It calls 'createAllChildren' method to create Its all children. This method will create all children and write their information to the pipe.
 * We will enter an infinite while loop inside main funciton of the watchdog, and via wait(NULL), we will wait for any child which is terminated, and do necessary actions.
 * When any child is terminated, It is checked whether It is P1 or others. If the terminated child is P1, then all other children will be terminated thanks to "killAllChildren" method, and they will be created again by "createAllChildren" method .
 * But If It is not, then only the terminated child will be created via "createOneChild" method.
 * When Watchdog receives SIGTERM signal, then "killWatchdog" method will be executed. Before terminate watchdog, "killAllChildren" will be called and all Its children will be terminated before Watchdog terminates.
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
#include <sys/wait.h>
#include <sys/types.h>
#include <map>
#include <fstream>

/*! amount of sleep ins second */
unsigned int timeToSleep = 1; // Second
void createAllChildren(int numberOfProcess, char * processOutput,bool isResultOfKill);
void createOneChild(pid_t pidOfChild, int numberOfProcess, char * processOutput);
void killAllChildren(bool shouldPOneBeKilled);
void killWatchdog(int sigNumber);
/*! Holds pipe */
int fd;
using namespace std;
/*! Output file stream for watchdog */
fstream watchdogOutputStream;
/*! Map for Pids and P# for processes */
map<long,string> pidsMap;
/*! index and pids */
map<long,long> indexPids;

/**
 * @brief Main Function
 * 
 * @param argc Argument Count
 * @param argv Argument Values
 * @return int Return value
 */
int main(int argc, char *argv[]) { 
	 
     
  char * myfifo = (char*) "/tmp/myfifo"; //Path of pipe
    

	fd = open(myfifo, O_WRONLY);  

    fd = open(myfifo, O_WRONLY);  

    int numberOfProcess = stoi(argv[1]) ; // Number of processes
    
    char * processOutput = argv[2] ; // Output path of process

    char * watchdogOutput = argv[3] ; //Output path of watchdog

    watchdogOutputStream.open(watchdogOutput, ios::app);
    
    long pid; //pid of objects
    
    string processIdString; //pids String

    stringstream processId1; //Pids of Parent

    pid_t parentPid; //Pids of parent


    processId1 << "P" <<  0 << ' ' << (long)getpid();
    parentPid = (long)getpid();
    processIdString = processId1.str();

    write(fd, processIdString.c_str(), 30); 
  
    createAllChildren(numberOfProcess,processOutput,false);

    signal(SIGTERM, killWatchdog); 

    while(true){
        pid_t pidOfChild = wait(NULL); //Pids of Child
        string pNumber = pidsMap.at(pidOfChild); // P# of child

        
        if(pNumber == "P1"){ 
            killAllChildren(false); 
            createAllChildren(numberOfProcess,processOutput,true); 
        } else {
            createOneChild(pidOfChild, numberOfProcess,processOutput); 
        }
    }

	return 0; 
} 


/**
 * @brief Create One Child Process
 * 
 * @param pidOfChild Pid of child that will be created
 * @param numberOfProcess number of processes
 * @param processOutput output path of process file
 */
void createOneChild(pid_t pidOfChild, int numberOfProcess, char * processOutput){
    
    stringstream processId; //Process id
    
    string processIdString; //Process id string
    
    pid_t childpid; //Pids of Child

    string pNumber = pidsMap.at(pidOfChild); // P# Number
    watchdogOutputStream << pNumber << " is killed\n";
    watchdogOutputStream.flush();
    childpid = fork();

    if(childpid == -1){
        watchdogOutputStream << "FAILED TO FORK\n";
        watchdogOutputStream.flush();
        return;
    }
    if(childpid == 0) {
        processId << pNumber << ' ' << (long)getpid();
        processIdString = processId.str();
        write(fd, processIdString.c_str(), 30); 
        execl("./process","./process", processOutput, pNumber.c_str(), NULL);
    }
    pidsMap.erase(pidOfChild);
    pidsMap[childpid] = pNumber;
    indexPids[stoi(pNumber.substr(1))]=childpid;
    watchdogOutputStream << "Restarting " << pNumber << "\n";
    watchdogOutputStream << pNumber << " is started and it has a pid of " << childpid << "\n";
    watchdogOutputStream.flush();

}

/**
 * @brief Create All Childs object in Two case: 1. In the beginning of the program, and 2. When P1 is terminated
 * 
 * @param numberOfProcess Number of processes
 * @param processOutput Output path of process file
 * @param isResultOfKill This variable is for writing output to file when a variable is terminated. At the beginning of the program, this is false, and we do not write anything to output file. 
 */
void createAllChildren(int numberOfProcess, char * processOutput,bool isResultOfKill){

    stringstream processId; //Pids of Child

    string processIdString; //Pids of Child
    
    pid_t childpid; //Pids of Child

    if(isResultOfKill){
        watchdogOutputStream << "P1 is killed, all processes must be killed\n";
        watchdogOutputStream << "Restarting all processes\n";
        watchdogOutputStream.flush();
    }
    for (int i=1; i<=numberOfProcess; i++) {

        string pNumber ="P"; // P# Number
        childpid = fork();
        if(childpid == -1){
            watchdogOutputStream << "FAILED TO FORK\n";
            watchdogOutputStream.flush();
            return;
        }
        if(childpid == 0) {
            pNumber += to_string(i);
            processId << pNumber << ' ' << (long)getpid();
            processIdString = processId.str();
            write(fd, processIdString.c_str(), 30); 
            execl("./process","./process", processOutput, pNumber.c_str(), NULL);
        } else {
            sleep(timeToSleep);  // Deal with writing delays
            pNumber += to_string(i);
            pidsMap[childpid] =  pNumber;
            indexPids[stoi(pNumber.substr(1))]=childpid;
            watchdogOutputStream << pNumber << " is started and it has a pid of " << childpid << "\n";
            watchdogOutputStream.flush();
            continue;
        }
    }
}

/**
 * @brief Kills all processes
 * 
 * @param shouldPOneBeKilled When watchdog is terminated, this is true because P1 must be killed. But when P1 is terminated, that means P1 is terminated before this method, then This is false and we should pass P1 and start termination from P2.
 */
void killAllChildren(bool shouldPOneBeKilled){
    map<long,long>::iterator pidsIterator = indexPids.begin(); //Iterator for iterating on pids
    if(!shouldPOneBeKilled) pidsIterator++;
    for(; pidsIterator != indexPids.end(); pidsIterator++){
        kill(pidsIterator->second , 15);
        wait(NULL);
        sleep(timeToSleep);  // Deal with writing delays
    }
    pidsMap.clear();
}

/**
 * @brief Kills watchdog when it receives SIGTERM signal.
 * 
 * @param sigNumber Signal number coming from signal.
 */
void killWatchdog(int sigNumber) {
    sleep(timeToSleep);  // Deal with writing delays
    killAllChildren(true);
    watchdogOutputStream << "Watchdog is terminating gracefully\n";
    watchdogOutputStream.flush();
    watchdogOutputStream.close();
    exit(0);
}