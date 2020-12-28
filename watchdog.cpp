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
#include <sys/wait.h>
#include <sys/types.h>
#include <map>
#include <fstream>
unsigned int timeToSleep = 1; // Second
struct timespec delta = {0 /*secs*/, 1000000000 /*nanosecs*/}; //0.3 sec
void createAllChilds(int numberOfProcess, char * processOutput,bool isResultOfKill);// Create all children
void createOneChild(pid_t pidOfChild, int numberOfProcess, char * processOutput); // Create a child
void killAllChildren(bool shouldPOneBeKilled); //Kill all children
void killWatchdog(int sigNumber); //Kill watchdog
int fd; //for pipe
using namespace std;
fstream watchdogOutputStream; // Output file stream
map<long,string> pidsMap; //Map for Pids and P# for processes
map<long,long> indexPids; //Map for Pids and P# for processes
int main(int argc, char *argv[]) { 
	 
	// FIFO file path 
	char * myfifo = (char*) "/tmp/myfifo"; 

    
    // Open FIFO for write only 
    fd = open(myfifo, O_WRONLY); 

    int numberOfProcess = stoi(argv[1]) ; //number of processes
    char * processOutput = argv[2] ; //process output path
    char * watchdogOutput = argv[3] ; // watchdog output path

    watchdogOutputStream.open(watchdogOutput, ios::app);
    
    long pid;
    string processIdString;
    stringstream processId1;
    pid_t parentPid;


    processId1 << "P" <<  0 << ' ' << (long)getpid();
    parentPid = (long)getpid();
    processIdString = processId1.str();
    write(fd, processIdString.c_str(), 30);  // Writing 'P0 Parent_Pid'
  
    createAllChilds(numberOfProcess,processOutput,false); //Create All Children

    signal(SIGTERM, killWatchdog); //Signal for terminating watchdog

    while(true){ //Loop until watchdog die
        pid_t pidOfChild = wait(NULL);
        string pNumber = pidsMap.at(pidOfChild);

        
        if(pNumber == "P1"){ // If killed child is P1, then kill all children and create them again.
            killAllChildren(false); //Kill all children
            createAllChilds(numberOfProcess,processOutput,true); //Create all children
        } else {
            createOneChild(pidOfChild, numberOfProcess,processOutput); //create killed child again
        }
    }

	return 0; 
} 


//Create a child
void createOneChild(pid_t pidOfChild, int numberOfProcess, char * processOutput){
    stringstream processId;
    string processIdString;
    pid_t childpid;

    string pNumber = pidsMap.at(pidOfChild);
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
    sleep(timeToSleep);  // Deal with writing delays
    pidsMap.erase(pidOfChild);
    pidsMap[childpid] = pNumber;
    indexPids[stoi(pNumber.substr(1))]=childpid;
    watchdogOutputStream << "Restarting " << pNumber << "\n";
    watchdogOutputStream << pNumber << " is started and it has a pid of " << childpid << "\n";
    watchdogOutputStream.flush();
}

//create all children
void createAllChilds(int numberOfProcess, char * processOutput,bool isResultOfKill){
    stringstream processId;
    string processIdString;
    pid_t childpid;
    if(isResultOfKill){
        watchdogOutputStream << "P1 is killed, all processes must be killed\n";
        watchdogOutputStream << "Restarting all processes\n";
        watchdogOutputStream.flush();
    }
    for (int i=1; i<=numberOfProcess; i++) {
        string pNumber ="P";
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

//Kill all children
void killAllChildren(bool shouldPOneBeKilled){
    map<long,long>::iterator pidsIterator = indexPids.begin();
    if(!shouldPOneBeKilled) pidsIterator++;
    for(; pidsIterator != indexPids.end(); pidsIterator++){
        kill(pidsIterator->second , 15);
        sleep(timeToSleep);  // Deal with writing delays
        wait(NULL);
    }
    pidsMap.clear();
}

//Kill watchdog and all Its children
void killWatchdog(int sigNumber) {
    // killAllChildren(true);
    watchdogOutputStream << "Watchdog is terminating gracefully\n";
    watchdogOutputStream.flush();
    watchdogOutputStream.close();
    exit(0);
}