
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>

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
#include <fcntl.h>

#include "misc.h"
#include "judge.h"

using namespace std;

extern string executive, tmpdir, infile, outfile, spjexec;
extern int timelimit, memlimit, outlimit, lang;


void check_add_java_security(string &t){
    string sec = "security.manager";
    int len = (int)t.size(), len1 = (int) sec.size();
    int i, j;
    for (i = 0; i < len - len1 + 1; ++i){
        if(t[i] == 's'){
            for(j = 0; j < len1; ++j){
                if(t[i+j] != sec[j]) break;
            }
            if(j == len1) return;
        }
    }
    t += " -Djava.security.manager";
}

const char usage_str[] = 
"\n" 
"----- FelixOJ Judge interfaces ------ \n" 
"\n" 
"Enter: command-line arguments (if a parameter contains spaces, remember to use quotation marks ...) \n" 
"-E need to implement the program (if it is java, you need to include the full java ooox command-line) \n" 
"-L language type (0 = C, 1 = C + +, 2 = pascal, 3 = Java) \n" 
"-D the temporary folder (for storing program output) \n" 
"-I input file (such as data/1001/test.in) \n" 
"-O output file (such as data/1001/test.out) \n" 
"-T time limit, in milliseconds (default 1000ms) \n" 
"-M memory limit, KB as the unit (default is 65536KB) \n" 
"-O output size limit, KB as the unit (default is 8192KB) \n" 
"-S SPJ program command line, if you do not provide that it is not SPJ \n" 
"-J -? Display this prompt \n" 
"Output: \n" 
"1. If the judge properly concluded the process of return to 0, the standard output \n" 
"% D% d% d, respectively OJ_RESULT, MEM_USED, TIME_USED \n" 
"One of OJ_RESULT see judge.h defined \n" 
"2. If the judge concluded the process of non-normal return of non-0, the standard output of the empty \n" 
"One of the specific meaning of the return value see judge.h defined \n" 
"3. Program standard output is redirected to a temporary folder stdout.txt \n" 
"Program error output is redirected to a temporary folder stderr.txt \n" 
"The caller needs to ensure that no conflict and the input and output test file \n" 
"These two documents to use and remove the need to call those who deal with \n" 
"\n" 
"\n" 
"SPJ interface \n" 
"Input: Command line contains three file names, according to the order for the standard input standard output program output \n" 
"Output: 1, 2, 4, respectively, AC, PE, WA \n" 
"5s return within 0 indicated normal, otherwise the judge would impose an end spj, and return System Error \n" 
"Spj not sub-RF \n" 
"\n" 
"Example: \n" 
"Non-SPJ \n" 
"./Judge-e \"/oj/tmp/9527/a.out \"-d \"/oj/tmp/9527 \"\n" 
"-I \"/oj/data/1001/test.out \"-O \"/oj/data/1001/test.in \"\n" 
"-T 1000-m 65536-o 512-l 1 \n" 
"SPJ \n" 
"./Judge-e \"/oj/tmp/9527/a.out \"-d \"/oj/tmp/9527 \"\n" 
"-I \"/oj/data/1001/test.out \"-O \"/oj/data/1001/test.in \"\n" 
"-T 1000-m 65536-o 512-l 1-s \"/oj/data/1001/spj \"\n" ;
//打印使用信息
void usage(){
    puts(usage_str);
}

//解析参数
void parse_argv(int argc, char *argv[]){
    if(argc < 2){
        usage();
        exit(EXIT_BAD_ARG);
    }
    int opt;

    lang = 1;
    timelimit = 1000;
    memlimit = 65536;
    outlimit = 8192;

    while(opt = getopt(argc, argv, "?he:l:d:I:O:t:m:o:s:"), opt != -1){
        switch(opt){
            case 'e': //可执行文件路径
                executive = optarg;
                break;
            case 'l': //语言类型 0=c, 1=c++, 2=pascal, 3=java
                lang = atoi(optarg);
                break;
            case 'd': //临时文件夹
                tmpdir = optarg;
                break;
            case 'I': //输入文件
                infile = optarg;
                break;
            case 'O': //输出文件
                outfile = optarg;
                break;
            case 't': //时间限制
                timelimit = atoi(optarg);
                break;
            case 'm': //内存限制
                memlimit = atoi(optarg);
                break;
            case 'o': //输出限制
                outlimit = atoi(optarg);
                break;
            case 's': //SPJexec
                spjexec = optarg;
                break;
            case '?':
            case 'h':
                usage();
                exit(EXIT_OK);
                break;
            default:
                usage();
                exit(EXIT_BAD_USAGE);
                break;
        }
    }
    if(executive.empty() || 
       tmpdir.empty() ||
       infile.empty() ||
       outfile.empty()){
        fprintf(stderr, "NULL ARG\n");
        exit(EXIT_BAD_ARG);
    }
    if(lang < 0 || lang > 3){
        fprintf(stderr, "BAD_LANG\n");
        exit(EXIT_BAD_LANG);
    }
    if(lang == 3) { //java
        check_add_java_security(executive);
    }
    if(timelimit <= 0){
        fprintf(stderr, "BAD_TIME_LIMIT\n");
        exit(EXIT_BAD_TIME_LIMIT);
    }
    if(memlimit <= 0){
        fprintf(stderr, "BAD_MEM_LIMIT\n");
        exit(EXIT_BAD_MEM_LIMIT);
    }
    if(outlimit <= 0){
        fprintf(stderr, "BAD_OUT_LIMIT\n");
        exit(EXIT_BAD_OUT_LIMIT);
    }

    dp("executive = %s\n", executive.c_str());
    dp("lang = %d\n", lang);
    dp("tmpdir = %s\n", tmpdir.c_str());
    dp("infile = %s\n", infile.c_str());
    dp("outfile = %s\n", outfile.c_str());
    dp("timelimit = %d ms\n", timelimit);
    dp("memlimit = %d KB\n", memlimit);
    dp("outlimit = %d KB\n", outlimit);
    dp("spjexec = %s\n", spjexec.c_str());
}

void dp(const char *fmt, ...){
#ifdef DEBUG
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
#endif
}

void set_limit(){

    rlimit lim;

    //时间限制
    lim.rlim_max = (timelimit + 999) / 1000 + 1; //秒，硬限制向上取整+1
    lim.rlim_cur = (timelimit + 999) / 1000; //软限制向上取整，不加1
    if(setrlimit(RLIMIT_CPU, &lim) < 0){
        perror("setrlimit");
        exit(EXIT_SETRLIMIT_TIME);
    }

    /*

    //内存限制
    //在这里进行内存限制可能导致MLE被判成RE
    //所以改成在每次wait以后判断
    lim.rlim_max = memlimit * 1024;
    lim.rlim_cur = memlimit * 1024;
    if(setrlimit(RLIMIT_AS, &lim) < 0){
        perror("setrlimit");
        exit(EXIT_SETRLIMIT_MEM);
    }

    */

    //堆栈空间限制
    lim.rlim_max = 4 * MEGA; // 4MB
    lim.rlim_cur = 4 * MEGA;
    if(setrlimit(RLIMIT_STACK, &lim) < 0){
        perror("setrlimit");
        exit(EXIT_SETRLIMIT_STACK);
    }

    //输出文件大小限制
    lim.rlim_max = outlimit * 1024;
    lim.rlim_cur = outlimit * 1024;
    if(setrlimit(RLIMIT_FSIZE, &lim) < 0){
        perror("setrlimit");
        exit(EXIT_SETRLIMIT_FSIZE);
    }

    dp("cpu/mem/stack/fsize limit set ok.\n");
}

void io_redirect(){
    //重定向输入
    if(freopen(infile.c_str(), "r", stdin) == NULL){
        perror("freopen(stdin)");
        exit(EXIT_FREOPEN_IN);
    }
    dp("in ok\n");
    //重定向输出
    if(freopen("stdout.txt", "w", stdout) == NULL){
        perror("freopen(stdout)");
        exit(EXIT_FREOPEN_OUT);
    }
    /*
    */

    //重定向错误输出
    if(freopen("stderr.txt", "w", stderr) == NULL){
        perror("freopen(stderr)");
        exit(EXIT_FREOPEN_ERR);
    }
}

void set_timer(int timelimit){
    struct itimerval now;
    now.it_interval.tv_sec = timelimit / 1000;
    now.it_interval.tv_usec = timelimit % 1000000 + 100000; //放宽100ms
    now.it_value.tv_sec = timelimit / 1000;
    now.it_value.tv_usec = timelimit % 1000000 + 100000;
    //VIRTUAL TIMER, 进程实际执行时间
    if(setitimer(ITIMER_VIRTUAL, &now, NULL) < 0){
        perror("setitimer");
        exit(EXIT_SETITIMER);
    }
    now.it_interval.tv_sec *= 2;
    now.it_value.tv_sec *= 2;
    //REAL TIMER, 系统真实时间(以免sleep卡死)
    if(setitimer(ITIMER_REAL, &now, NULL) < 0){
        perror("setitimer");
        exit(EXIT_SETITIMER);
    }
    dp("setitimer ok.\n");
}

int RF_table[1024];
//C or C++
int LANG_CV[256]={SYS_execve, SYS_read, SYS_uname, SYS_write, SYS_open, SYS_close, SYS_access, SYS_brk, SYS_munmap, SYS_mprotect, SYS_mmap2, SYS_fstat64, SYS_set_thread_area, SYS_exit_group, SYS_exit, 0};
int LANG_CC[256]={1,          -1,       -1,        -1,        -1,       -1,        -1,         -1,      -1,         -1,           -1,        -1,          -1,                  -1,             -1,       0};
//Pascal
int LANG_PV[256]={SYS_execve, SYS_open, SYS_set_thread_area, SYS_brk, SYS_read, SYS_uname, SYS_write, SYS_ioctl, SYS_readlink, SYS_mmap, SYS_rt_sigaction, SYS_getrlimit, SYS_exit_group, SYS_exit, SYS_ugetrlimit, 0};
int LANG_PC[256]={1,          -1,       -1,                  -1,      -1,       -1,        -1,        -1,        -1,           -1,       -1,               -1,            -1,             -1,       -1,             0};
//Java
int LANG_JV[256]={SYS_execve, SYS_ugetrlimit, SYS_rt_sigprocmask, SYS_futex, SYS_read, SYS_mmap2, SYS_stat64, SYS_open, SYS_close, SYS_access, SYS_brk, SYS_readlink, SYS_munmap, SYS_close, SYS_uname, SYS_clone, SYS_uname, SYS_mprotect, SYS_rt_sigaction, SYS_sigprocmask, SYS_getrlimit, SYS_fstat64, SYS_getuid32, SYS_getgid32, SYS_geteuid32, SYS_getegid32, SYS_set_thread_area, SYS_set_tid_address, SYS_set_robust_list, SYS_exit_group, 0};
int LANG_JC[256]={2,          -1,            -1,                 -1,        -1,        -1,        -1,         -1,       -1,        -1,         -1,      -1,           -1,         -1,        -1,        1,         -1,        -1,           -1,               -1,              -1,            -1,          -1,           -1,           -1,            -1,            -1,                  -1,                  -1,                  -1,              0};

void init_RF_table(){
    int i;
    memset(RF_table, 0, sizeof(RF_table));
    if (lang == 0 || lang == 1){ // C & C++
        for (i = 0; LANG_CV[i]; i++) {
            RF_table[LANG_CV[i]]=LANG_CC[i];
        }
    }else if (lang == 2){ // Pascal
        for (i = 0; LANG_PV[i]; i++) {
            RF_table[LANG_PV[i]]=LANG_PC[i];
        }
    }else if (lang == 3){ // Java
        for (i = 0; LANG_JV[i]; i++) {
            RF_table[LANG_JV[i]]=LANG_JC[i];
        }
    }else{
        dp("BAD lang");
    }
}

bool is_valid_syscall(int num){
    static int in_syscall = 0;
    in_syscall = 1 - in_syscall;
    //dp("%d (%s)\n", num, in_syscall? "in" : "out");
    if(RF_table[num] == 0) {
        return false;
    } else{
        if(in_syscall == 0){
            RF_table[num]--;
        }
    }
    return true;
}

class filereader{
private:
    int fd;
    char buf[1024];
    int cnt;
    int pt;
    char get(){
        if(pt + 1 == cnt) cnt = 0;
        if(cnt == 0){
            pt = -1;
            cnt = read(fd, buf, 1024);
            if(cnt < 0){
                perror("read(filereader)");
                exit(EXIT_FILEREADER_READ);
            }
            if(cnt == 0){
                return -1;
            }
        }
        pt++;
        return buf[pt];
    }
public:
    filereader(const char *filename){
        fd = open(filename, O_RDONLY);
        if(fd < 0){
            perror("open(filereader)");
            exit(EXIT_FILEREADER_OPEN);
        }
        pt = -1;
        cnt = 0;
    }

    char next(){
        char t = '\r';
        while(t != -1 && t == '\r') t = get();
        return t;
    }
    void ret(){
        pt--;
    }
    ~filereader(){
        if(close(fd) < 0){
            perror("close(filereader)");
            exit(EXIT_FILEREADER_CLOSE);
        }
    }
};

bool isblank(char &t1){
    return 
      (t1 == ' '  ||
       t1 == '\t' ||
       t1 == '\r' ||
       t1 == '\n');
}

// RETURN: OJ_AC, OJ_PE, OJ_WA
int compare(const char * f1, const char * f2){
    filereader a(f1), b(f2);
    int ac = 1;
    char t1, t2;
    while(true){
        t1 = a.next();
        t2 = b.next();
        if(t1 == -1 || t2 == -1) break;
        if(ac == 1){
            if(t1 == t2){
                continue;
            }else /* t1 != t2 */ {
                if(isblank(t1) || isblank(t2)){
                    ac = 0;
                    a.ret();
                    b.ret();
                    continue;
                }else{
                    return OJ_WA;
                }
            }
        }else{
            while(t1 != -1 && isblank(t1)) t1 = a.next();
            while(t2 != -1 && isblank(t2)) t2 = b.next();
            if(t1 == -1 || t2 == -1) break;
            if(t1 != t2){
                return OJ_WA;
            }
        }
    }
    if(t1 != -1 || t2 != -1) ac = 0;
    while(t1 != -1 && isblank(t1)) t1 = a.next();
    while(t2 != -1 && isblank(t2)) t2 = b.next();

    if(t1 == -1 && t2 == -1){
        if(ac == 1){
            return OJ_AC;
        }else{
            return OJ_PE;
        }
    }else{
        return OJ_WA;
    }
}

void popen_timeout(int signo){
    dp("SPJ timed out.\n");
    exit(EXIT_SPJ_TIMEOUT);
}

int special_judge(){
    string cmdline = spjexec + " '" + infile  + "' '" + outfile + "' stdout.txt";
    dp("SPJ: %s\n", cmdline.c_str());
    FILE *fp = popen(cmdline.c_str(), "r");
    if(fp == NULL){
        perror("popen");
        exit(EXIT_SPJ_POPEN);
    }
    signal(SIGALRM, popen_timeout);
    signal(SIGVTALRM, popen_timeout);
    set_timer(5000);

    int res;
    if(fscanf(fp, "%d", &res) != 1){
        fprintf(stderr, "fscanf: no output produced from spj\n");
        exit(EXIT_SPJ_FSCANF);
    }
    dp("SPJ res: %d\n", res);

    if(pclose(fp) < 0){
        perror("pclose");
        exit(EXIT_SPJ_PCLOSE);
    }
    switch(res){
        case 1:
            return OJ_AC;
        case 2:
            return OJ_PE;
        case 4:
            return OJ_WA;
        default:
            dp("SPJ BAD RETURN VALUE: %d\n", res);
            exit(EXIT_SPJ_UNKNOWN);
    }
}
