#include <iostream>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/reg.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "openolir.h"

using namespace std;

int runExecutable(char*, char*, char*, char*, char*, char*);
void setLimits(char*, char*, char*);
void setTimer(int);

const int MB = 1048576;
int RFTable[1024];

int main(int argc, char* argv[])
{
    char* executable = argv[1];
    char* input = argv[2];
    char* output = argv[3];
    
    char* timelimit = argv[4];
    char* memorylimit = argv[5];
    char* outputlimit = argv[6];

    runExecutable(executable, input, output, 
            timelimit, memorylimit, outputlimit);

    return 0;
}

int runExecutable(char* executable, char* input, char* output, 
        char* timelimit, char* memorylimit, char* outputlimit) {
    
    pid_t processID = fork();
    
    if (processID == 0) {
        // child

        // redirect input/output stream
        freopen(input, "r", stdin);
        freopen(output, "w", stdout);

        // redirect error stream
        char* error = strcat(output, ".err"); 
        freopen(error, "w", stderr);

        // set limits
        setLimits(timelimit, memorylimit, outputlimit);
        
        // set timer
        setTimer(atoi(timelimit));

        ptrace(PTRACE_TRACEME, 0, NULL, NULL);

        execl(executable, NULL, NULL);

        cout << "I'm child " << executable << endl;
    }

    else if (processID > 0) {

        int result = -1;
        int status = 0;
        long memuse = 0;
        int orig_eax = 0;
        struct rusage rused;
        struct user_regs_struct regs;

        while(true) {
            wait4(processID, &status, 0, &rused);

            if (WIFEXITED(status)) {
                result = 1;
                break;
            }

            //signaled exit
            if (WIFSIGNALED(status)) {
                
                int sig = WTERMSIG(status);

                switch(sig){
                    //time limit
                    case SIGALRM:    
                    case SIGXCPU:
                    case SIGKILL:
                        result = OJ_TLE;
                        break;

                    //output file size limit
                    case SIGXFSZ:
                        result = OJ_OLE;
                        break;

                    //runtime errors, a variety of situations
                    case SIGSEGV:
                        result = OJ_RE_SEGV;
                        break;
                    case SIGFPE:
                        result = OJ_RE_FPE;
                        break;
                    case SIGBUS:
                        result = OJ_RE_BUS;
                        break;
                    case SIGABRT:
                        result = OJ_RE_ABRT;
                        break;
                    default:
                        result = OJ_RE_UNKNOWN;
                        break;
                }

                break;
            }

            if(WEXITSTATUS(status) != 5){
                switch(WEXITSTATUS(status)){
                    //timelimit, TLE
                    case SIGALRM:    
                    case SIGXCPU:
                    case SIGKILL:
                        result = OJ_TLE;
                        break;

                    //output limit, OLE
                    case SIGXFSZ:
                        result = OJ_OLE;
                        break;

                    //runtime errors, RE
                    case SIGSEGV:
                        result = OJ_RE_SEGV;
                        break;
                    case SIGFPE:
                        result = OJ_RE_FPE;
                        break;
                    case SIGBUS:
                        result = OJ_RE_BUS;
                        break;
                    case SIGABRT:
                        result = OJ_RE_ABRT;
                        break;
                    default:
                        result = OJ_RE_UNKNOWN;
                        break;
                }

                kill(processID, SIGKILL);
                break;
            }

            memuse = max(memuse, rused.ru_minflt * (getpagesize() / 1024));
            // memory usage exceeds the limit, MLE
            int memlim = atoi(memorylimit) * MB;
            if(memuse > memlim){
                result = OJ_MLE;
                kill(processID, SIGKILL);
                break;
            }

            // intercept and inspect SYSCALL
            ptrace(PTRACE_GETREGS, processID, NULL, &regs);

            /*
            //prohibited system call, RF
            if (regs.orig_eax >= 0 && !isValidSyscall(regs.orig_eax)) {
                result = OJ_RF;
                kill(processID, SIGKILL);
                break;
            }
            */

            // continue to run
            ptrace(PTRACE_SYSCALL, processID, NULL, NULL);
        }

        int timeuse = (rused.ru_utime.tv_sec * 1000 + 
                       rused.ru_utime.tv_usec / 1000);

        cout << result << " " << memuse << " " << timeuse << endl;

        return 0;
    }
}


void setLimits(char* timelimit, char* memorylimit, char* outputlimit) {

    rlimit lim;

    // cpu time
    int tl = atoi(timelimit);
    lim.rlim_max = (timelimit + 999) / 1000 + 1;
    lim.rlim_cur = (timelimit + 999) / 1000;
    setrlimit(RLIMIT_CPU, &lim);

    /*
    // mem
    lim.rlim_max = memlimit * 1024;
    lim.rlim_cur = memlimit * 1024;
    setrlimit(RLIMIT_AS, &lim);
    */

    // stack size
    int memlim = atoi(memorylimit);
    lim.rlim_max = memlim * MB;
    lim.rlim_cur = memlim * MB;
    setrlimit(RLIMIT_STACK, &lim);
    
    // output limit输出
    int ol = atoi(outputlimit);
    lim.rlim_max = ol * 1024;
    lim.rlim_cur = ol * 1024;
    setrlimit(RLIMIT_FSIZE, &lim);
}

void setTimer(int timelimit) {
    struct itimerval now;
    now.it_interval.tv_sec = timelimit / 1000;
    now.it_interval.tv_usec = timelimit % 1000000 + 100000; //Relaxation 100ms
    now.it_value.tv_sec = timelimit / 1000;
    now.it_value.tv_usec = timelimit % 1000000 + 100000;

    //VIRTUAL TIMER, the process of the actual execution time
    setitimer(ITIMER_VIRTUAL, &now, NULL);
    now.it_interval.tv_sec *= 2;
    now.it_value.tv_sec *= 2;

    //REAL TIMER, the system real time (so as not to sleep choked to death)
    setitimer(ITIMER_REAL, &now, NULL);
}