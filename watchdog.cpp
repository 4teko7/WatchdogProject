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
using namespace std;
int main(int argc, char *argv[]) 
{ 
	int fd; 

    

	// FIFO file path 
	char * myfifo = (char*) "/tmp/myfifo"; 

    
    // Open FIFO for write only 
    fd = open(myfifo, O_WRONLY); 

    int numberOfProcess = stoi(argv[1]) ;
    char * processOutput = argv[2] ;
    char * watchdogOutput = argv[3] ;

    pid_t parentPid;
    pid_t childpid;
    stringstream processId;
    stringstream processId1;
    string processIdString;
    long pids[numberOfProcess];
    map<long,string> pidsMap;
    long pid;

    processId1 << "P" <<  0 << ' ' << (long)getpid();
    parentPid = (long)getpid();
    processIdString = processId1.str();
    cout << processIdString << endl;
    // Write the input arr2ing on FIFO 
    // write(fd, processIdString.c_str(), strlen(processIdString.c_str())); 
    write(fd, processIdString.c_str(), 30); 
    pids[0] = parentPid;
    for (int i=1; i<=numberOfProcess; i++) {
        childpid = fork();
        if(childpid == -1){
            cout << "FAILED TO FORK" << endl;
            return 1;
        }
        if(childpid == 0) {
            pids[i] = (long)getpid();
            string pNumber ="P";
            pNumber += to_string(i);
            processId << "P" << i << ' ' << (long)getpid();
            processIdString = processId.str();
            cout << processIdString << endl;
            // Write the input arr2ing on FIFO 
		    // write(fd, processIdString.c_str(), strlen(processIdString.c_str())); 
		    write(fd, processIdString.c_str(), 30); 
            execl("./process","./process", pNumber.c_str(), NULL);
        } else {
            pids[i] = childpid;
            string pNum ="P";
            pNum += to_string(i);
            pidsMap[childpid] =  pNum;
            cout << "pNum : " << pNum << " childpid : " << childpid << endl;
            nanosleep(&delta, &delta);  // Deal with writing delays
            continue;
            // cout << " I AM PARENT : " << (long)getpid()  << " : "  << childpid << endl;
        }
    }

    // sleep(5000);

    long counter = 0;
    while(true){
        if(counter >= numberOfProcess){
            counter = 0;
        }
        int status;
        // cout << "PIDS " << counter << " : " <<  pids[counter] << endl;
        pid_t result = waitpid(pids[counter], &status, WNOHANG);
        if (result == 0) {
            // Child still alive
            // cout << "CHILD IS ALIVE" << endl;
        } else if (result == -1) {
            // Error 
            // cout << "Error " << endl;
        } else {
            // Child exited
            cout << "Child WITH PID " << pidsMap[pids[counter]] << " " << pids[counter] << "WAS TERMINATED" << endl;
            string pNumber = pidsMap.at(pids[counter]);

            childpid = fork();
            if(childpid == -1){
                // cout << "FAILED TO FORK" << endl;
                return 1;
            }
            if(childpid == 0) {
                pid = (long)getpid();
                processId << pNumber << ' ' << (long)getpid();
                processIdString = processId.str();
                cout << processIdString << endl;
                // Write the input arr2ing on FIFO 
                // write(fd, processIdString.c_str(), strlen(processIdString.c_str())); 
                write(fd, processIdString.c_str(), 30); 
                cout << "CHILD WITH PID : " << pNumber << " " << pid << " WAS CREATED" << endl;
                execl("./process","./process", pNumber.c_str(), NULL);
        }

        pids[counter] = childpid;
        pidsMap[childpid] = pNumber;
    }
    counter++;
    }

    // close(fd); 


    // char str1[80], str2[80]; 
    // for (int i=0; i<=numberOfProcess; i++) {
    //     cout << "in for loop : " << i << endl;
    //     // First open in read only and read 
    //     fd = open(myfifo,O_RDONLY); 
    //     read(fd, str1, 80); 

    //     // Print the read string and close 
    //     printf("User1: %s\n", str1); 
    // } 
    //     close(fd); 
    

	// // Creating the named file(FIFO) 
	// // mkfifo(<pathname>, <permission>) 
	// mkfifo(myfifo, 0666); 

	// while (1) 
	// { 
	// 	// Open FIFO for write only 
	// 	fd = open(myfifo, O_WRONLY); 

	// 	// Take an input arr2ing from user. 
	// 	// 80 is maximum length 
	// 	fgets(arr2, 80, stdin); 

	// 	// Write the input arr2ing on FIFO 
	// 	// and close it 
	// 	write(fd, arr2, strlen(arr2)+1); 
	// 	close(fd); 
	// } 
	return 0; 
} 
