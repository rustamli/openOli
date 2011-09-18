#ifndef __JUDGE_H__
#define __JUDGE_H__

//result codes
const int OJ_WAIT       = 0; //OK
const int OJ_AC         = 1; //OK
const int OJ_PE         = 2; //OK
const int OJ_TLE        = 3; //OK
const int OJ_MLE        = 4; //OK
const int OJ_WA         = 5; //OK
const int OJ_OLE        = 6; //OK
const int OJ_CE         = 7; //OK
const int OJ_RE_SEGV    = 8; //OK
const int OJ_RE_FPE     = 9; //OK
const int OJ_RE_BUS     = 10;//OK
const int OJ_RE_ABRT    = 11;//OK
const int OJ_RE_UNKNOWN = 12;//OK
const int OJ_RF         = 13;//OK
const int OJ_SE         = 14;//System Error

//exit codes
const int EXIT_OK               =   0;
const int EXIT_BAD_USAGE        =   2;
const int EXIT_BAD_ARG          =   2;
const int EXIT_BAD_TIME_LIMIT   =   4;
const int EXIT_BAD_LANG         =   5;
const int EXIT_BAD_MEM_LIMIT    =   6;
const int EXIT_BAD_OUT_LIMIT    =   8;
const int EXIT_BAD_FORK         =   10;
const int EXIT_SETRLIMIT_TIME   =   12;
const int EXIT_SETRLIMIT_MEM    =   14;
const int EXIT_SETRLIMIT_STACK  =   16;
const int EXIT_SETRLIMIT_FSIZE  =   17;
const int EXIT_CHDIR            =   18;
const int EXIT_FREOPEN_IN       =   20;
const int EXIT_FREOPEN_OUT      =   22;
const int EXIT_FREOPEN_ERR      =   24;
const int EXIT_SETITIMER        =   26;
const int EXIT_PTRACE_TRACEME   =   28;
const int EXIT_EXECL            =   30;
const int EXIT_PTRACE_GETREGS   =   32;
const int EXIT_PTRACE_SYSCALL   =   34;
const int EXIT_FILEREADER_OPEN  =   36;
const int EXIT_FILEREADER_READ  =   38;
const int EXIT_FILEREADER_CLOSE =   40;
const int EXIT_WAIT4            =   42;
const int EXIT_SPJ_POPEN        =   44;
const int EXIT_SPJ_PCLOSE       =   46;
const int EXIT_SPJ_TIMEOUT      =   48;
const int EXIT_SPJ_FSCANF       =   50;
const int EXIT_SPJ_UNKNOWN      =   52;
const int EXIT_UNKNOWN          =   64;

const int MEGA= 1048576; // 1M

#endif
