#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/unistd.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
MODULE_LICENSE("Dual BSD/GPL");
#define __NR_hellocall 356

const int MAX_BUFFERSIZE=2048;

static int (*oldcall)(void);

struct prinfo {
pid_t parent_pid;
/* process id of parent */
pid_t pid;
/* process id */
pid_t first_child_pid; /* pid of youngest child */
pid_t next_sibling_pid; /* pid of older sibling */
long state;
long uid;
/* current state of process */
/* user id of process owner */
char comm[64];
/* name of program executed */
unsigned depth;
};

void dfs(struct prinfo *buf,struct task_struct *taskptr,int *nr,int depth){
    int rank=*nr;
    buf[rank].parent_pid=(taskptr->parent)?taskptr->parent->pid:0;
    buf[rank].pid=taskptr->pid;
    buf[rank].uid=taskptr->cred->uid;
    strcpy(buf[rank].comm,taskptr->comm);
    buf[rank].depth=depth;

    if(list_empty(&(taskptr->sibling))==1) buf[rank].next_sibling_pid=0;
    else buf[rank].next_sibling_pid=list_entry(taskptr->sibling.next, struct task_struct, sibling)->pid;

    if(list_empty(&(taskptr->children))==1) buf[rank].first_child_pid=0;
    else buf[rank].first_child_pid=list_entry(taskptr->children.next, struct task_struct, sibling)->pid;

    (*nr)++;

    struct list_head *head;
    struct task_struct *task;
    list_for_each(head,&(taskptr->children)){
        task=list_entry(head,struct task_struct, sibling);
        dfs(buf,task,nr,depth+1);
    }
}

static int ptree(struct prinfo *buf,int *nr)
{
    //printk("<0>" "this is my system second call!\n the uid = %d\n str: %s\n",n,str);
    int n=0;
    struct prinfo *kbuffer=(struct prinfo *)kmalloc(sizeof(struct prinfo)*MAX_BUFFERSIZE,GFP_KERNEL);
    read_lock(&tasklist_lock);
    dfs(kbuffer,&init_task,&n);
    read_unlock(&tasklist_lock);
    if(copy_to_user(buf, kbuffer, sizeof(struct prinfo)* MAX_BUFFERSIZE) || copy_to_user(nr, &n, sizeof(int)))
	{
		printk(KERN_INFO "ERROR!\n");
		return 1;
	}
	kfree(kbuffer);

    return 0;
}
static int addsyscall_init(void)
{
	long *syscall = (long*)0xc000d8c4;
	oldcall=(int(*)(void))(syscall[__NR_hellocall]);
	syscall[__NR_hellocall]=(unsigned long)ptree;
	printk("<0>" "module load!\n");
	return 0;
}

static void addsyscall_exit(void)
{
	long *syscall=(long*)0xc000d8c4;
	syscall[__NR_hellocall]=(unsigned long)oldcall;
	printk("<0>" "module exit!\n");
}

module_init(addsyscall_init);
module_exit(addsyscall_exit);








