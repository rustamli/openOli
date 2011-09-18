#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#include <unistd.h>
#include <errno.h>
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

#include "judge.h"
#include "misc.h"

using namespace std;

string executive, tmpdir, infile, outfile, spjexec;
int timelimit, memlimit, outlimit, lang;
int result;

/*
 *  use ./judge -h to see description
 */
int main(int argc, char *argv[]){

    parse_argv(argc, argv); //parsing command line arguments

    init_RF_table(); //initialization of the syscall table in the corresponding RF

    if(chdir(tmpdir.c_str()) < 0){
        perror("chdir");
        exit(EXIT_CHDIR);
    }
    dp("chdir ok\n");


    pid_t child = fork();
    if(child < 0){
        // create a new process, a mistake -__-
        perror("fork");
        exit(EXIT_BAD_FORK);

    }else if(child == 0){
        //child
        dp("before redirect\n");

        io_redirect(); // redirect  input / output / error
        set_limit(); //set the CPU / MEM / STACK / FSIZE restrictions

        set_timer(timelimit); //set the timer
        if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0){
            perror("ptrace(TRACEME)");
            exit(EXIT_PTRACE_TRACEME);
        }

        //executing
        execl(executive.c_str(), NULL, NULL);

        //execution eror
        perror("execl");
        exit(EXIT_EXECL);

    }else{
        //parent
        int status = 0;
        long memuse = 0;
        int orig_eax = 0;
        struct rusage rused;
        struct user_regs_struct regs;

        while(true){
            if(wait4(child, &status, 0, &rused) < 0){
                perror("wait4");
                exit(EXIT_WAIT4);
            }
            //dp("wait ends (%d)\n", status);

            //normal exit
            if(WIFEXITED(status)){
                dp("AC or PE or WA\n");
                result = 1;
                break;
            }

            //signaled exit
            if(WIFSIGNALED(status)){
                int sig = WTERMSIG(status);
                dp("sig = %d\n", sig);
                switch(sig){
                    //time limit
                    case SIGALRM:    
                    case SIGXCPU:
                    case SIGKILL:
                        dp("TLE\n");
                        result = OJ_TLE;
                        break;
                    //output file size limit
                    case SIGXFSZ:
                        dp("OLE\n");
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
                break; //exit loop
            }

            // according to the code Sempr, and do not understand the conditions to determine
            if(WEXITSTATUS(status) != 5){
                dp("EXITCODE = %d\n", WEXITSTATUS(status));
                switch(WEXITSTATUS(status)){
                    //timelimit, TLE
                    case SIGALRM:    
                    case SIGXCPU:
                    case SIGKILL:
                        dp("TLE\n");
                        result = OJ_TLE;
                        break;
                    //output limit, OLE
                    case SIGXFSZ:
                        dp("OLE\n");
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
                kill(child, SIGKILL);
                break; //exit loop
            }

            memuse = max(memuse, rused.ru_minflt * (getpagesize() / 1024));
            //memory usage exceeds the limit, MLE
            if(memuse > memlimit){
                dp("MLE(%dKB)\n", memuse);
                result = OJ_MLE;
                kill(child, SIGKILL);
                break;
            }

            /**/
            //intercept and inspect SYSCALL
            if(ptrace(PTRACE_GETREGS, child, NULL, &regs) < 0){
                perror("ptrace(PTRACE_GETREGS)");
                exit(EXIT_PTRACE_GETREGS);
            }

            //prohibited system call, RF
            if(regs.orig_eax >= 0 && !is_valid_syscall(regs.orig_eax)){
                dp("RF (SYSCALL = %d)\n", regs.orig_eax);
                result = OJ_RF;
                kill(child, SIGKILL);
                break;
            }

            //continue to run
            if(ptrace(PTRACE_SYSCALL, child, NULL, NULL) < 0){
                perror("ptrace(PTRACE_SYSCALL)");
                exit(EXIT_PTRACE_SYSCALL);
            }
        }
    //sub-process is completed, return the use of statistical resources and result
        int timeuse = (rused.ru_utime.tv_sec * 1000 + 
                       rused.ru_utime.tv_usec / 1000);
        dp("[child_ends]\n");
        printf("%d %ld %d\n", 
                result,    memuse,      timeuse);
    }

    return 0;
}

