#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ata Halicioglu");
MODULE_DESCRIPTION("Process Visualizer Module");

static int root_pid = 1;

module_param(root_pid, int, S_IRUGO);
MODULE_PARM_DESC(root_pid, "Root PID to start the process tree from");

static struct proc_dir_entry* proc_entry;


static void show_process_tree(struct seq_file *m, struct task_struct *task, int level) {
    struct list_head *list;
    struct task_struct *child;

    seq_printf(m, "%*sPID: %d, Command: %s, Start Time: %llu\n", level * 2, "", task->pid, task->comm, task->start_time);

    list_for_each(list, &task->children) {
        child = list_entry(list, struct task_struct, sibling);
        show_process_tree(m, child, level + 1);  
    }
}

static int proc_show(struct seq_file *m, void *v) {
    struct task_struct *task;

    rcu_read_lock();
    for_each_process(task) {
        if (task->pid == root_pid) {
            show_process_tree(m, task, 0);
            break;
        }
    }
    rcu_read_unlock();
    return 0;
}

static int proc_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_fops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init mymodule_init(void) {
    proc_entry = proc_create("psvis", 0, NULL, &proc_fops);
    if (!proc_entry) {
        return 1;
    }
    return 0;
}

static void __exit mymodule_exit(void) {
    proc_remove(proc_entry);
}

module_init(mymodule_init);
module_exit(mymodule_exit);
