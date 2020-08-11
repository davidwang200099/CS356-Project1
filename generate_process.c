#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
int main(){
    pid_t parent_pid=getpid();
	pid_t child_pid;
	int status;
    if((child_pid=fork())==0){
        printf("Student ID:XXXXXXXX,Parent PID is:%d\n",parent_pid);
        while((child_pid=waitpid(-1,&status,0))>0){
			if(WIFEXITED(status)) printf("Child %d terminated normally with exist status=%d\n",child_pid,WEXITSTATUS(status));
            else printf("Child %d terminated abnormally with exist status=%d\n",child_pid,WEXITSTATUS(status));
        }
    }else{
		printf("Student ID:518021910976,Child PID is:%d\n",child_pid);
        execl("/data/misc/test_ptree","test_ptree",NULL);
        exit(0);
    }
    return 0;
}
