#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>

struct prinfo {
pid_t parent_pid;
pid_t pid;
pid_t first_child_pid; 
pid_t next_sibling_pid; 
long state;
long uid;
char comm[64];
unsigned depth;
};

const int MAX_BUFFERSIZE=2048;

int main(){
    struct prinfo *buffer=(struct prinfo *)malloc(sizeof(struct prinfo)*MAX_BUFFERSIZE);
    int n=0;
    syscall(356,buffer,&n);
    //int cnt_tab[MAX_BUFFERSIZE];
    //int cnt_child[MAX_BUFFERSIZE];
    //for(int i=0;i<n;i++) cnt_tab=0;
    //for(int i=0;i<n;i++) cnt_child=0;
    /*for(int i=0;i<n;i++){
        if((i+1<n)&&(buffer[i].next_sibling_pid!=buffer[i+1].pid)&&(buffer[i].first_child_pid!=buffer[i+1].pid))
            buffer[i].next_sibling_pid=0;
        if(i+1==n) buffer[i].next_sibling_pid = 0;
    }*/
	int i,j;
    for(i=0;i<n;i++){
    	for(j=0;j<buffer[i].depth;j++) printf("\t");
    	printf("%s,%d,%ld,%d,%d,%d,%ld\n", buffer[i].comm, buffer[i].pid, buffer[i].state, buffer[i].parent_pid, buffer[i].first_child_pid, buffer[i].next_sibling_pid, buffer[i].uid);
    }
	free(buffer);  
    return 0;
}
