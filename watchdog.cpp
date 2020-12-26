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
struct timespec delta = {0 /*secs*/, 300000000 /*nanosecs*/}; //0.3 sec
void createAllChilds(int numberOfProcess);
void createOneChild(pid_t pidOfChild, int numberOfProcess);
void killAllChildren(bool shouldPOneBeKilled);
void killWatchdog(int sigNumber);
int fd;

using namespace std;
map<long,string> pidsMap;
int main(int argc, char *argv[]) 
{ 
	 

    

	// FIFO file path 
	char * myfifo = (char*) "/tmp/myfifo"; 

    
    // Open FIFO for write only 
    fd = open(myfifo, O_WRONLY); 

    int numberOfProcess = stoi(argv[1]) ;
    char * processOutput = argv[2] ;
    char * watchdogOutput = argv[3] ;

    
    
    long pid;
    string processIdString;
    stringstream processId1;
    pid_t parentPid;


    processId1 << "P" <<  0 << ' ' << (long)getpid();
    parentPid = (long)getpid();
    processIdString = processId1.str();
    write(fd, processIdString.c_str(), 30); 
  
    createAllChilds(numberOfProcess); //Create All Children

    signal(SIGTERM, killWatchdog); 

    while(true){
        pid_t pidOfChild = wait(NULL);
        string pNumber = pidsMap.at(pidOfChild);

        
        if(pNumber == "P1"){
            killAllChildren(false);
            createAllChilds(numberOfProcess); //Create All Children
        } else {
            createOneChild(pidOfChild, numberOfProcess);
        }
    }

	return 0; 
} 



void createOneChild(pid_t pidOfChild, int numberOfProcess){
    stringstream processId;
    string processIdString;
    pid_t childpid;

    string pNumber = pidsMap.at(pidOfChild);
    childpid = fork();
    if(childpid == -1){
        cout << "FAILED TO FORK" << endl;
        return;
    }
    if(childpid == 0) {
        processId << pNumber << ' ' << (long)getpid();
        processIdString = processId.str();
        write(fd, processIdString.c_str(), 30); 
        execl("./process","./process", pNumber.c_str(), NULL);
    }
    pidsMap.erase(pidOfChild);
    pidsMap[childpid] = pNumber;
}

void createAllChilds(int numberOfProcess){
    stringstream processId;
    string processIdString;
    pid_t childpid;

    for (int i=1; i<=numberOfProcess; i++) {
        string pNumber ="P";
        childpid = fork();
        if(childpid == -1){
            cout << "FAILED TO FORK" << endl;
            return;
        }
        if(childpid == 0) {
            pNumber += to_string(i);
            processId << pNumber << ' ' << (long)getpid();
            processIdString = processId.str();
            write(fd, processIdString.c_str(), 30); 
            execl("./process","./process", pNumber.c_str(), NULL);
        } else {
            pNumber += to_string(i);
            pidsMap[childpid] =  pNumber;
            nanosleep(&delta, &delta);  // Deal with writing delays
            continue;
        }
    }
}

void killAllChildren(bool shouldPOneBeKilled){
    map<long,string>::iterator pidsIterator = pidsMap.begin();
    if(!shouldPOneBeKilled) pidsIterator++;
    for(; pidsIterator != pidsMap.end(); pidsIterator++){
        int killValue = kill(pidsIterator->first , 15);
        wait(NULL);
        // cout<<"ATER WAIT : " << killValue <<endl;
    }
    pidsMap.clear();
}

void killWatchdog(int sigNumber) {
    killAllChildren(true);
    cout << "Watchdog is terminating gracefully" << endl;
    exit(0);
}