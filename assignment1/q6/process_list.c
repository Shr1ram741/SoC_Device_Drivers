#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>

#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 10, 0)
#include <linux/sched/signal.h>
#endif
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kallsyms.h>

#define OURMODNAME "prcs_list"

MODULE_AUTHOR("Shriram");
MODULE_DESCRIPTION("Shows all processes");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");

static int show_prcs_in_tasklist(void)
{
        struct task_struct *p;
#define MAXLEN   128
        char tmp[MAXLEN];
        int numread = 0, n = 0, total = 0;

        rcu_read_lock();
        for_each_process(p) {
                memset(tmp, 0, 128);
                n = snprintf(tmp, 128, "Name: %-16sPID: %-8dState: %-8ld\n", p->comm, p->pid, p->state);
                numread += n;
                pr_info("%s", tmp);
                //pr_debug("n=%d numread=%d tmp=%s\n", n, numread, tmp);

                cond_resched();
                total++;
        }
        rcu_read_unlock();

        return total;
}

static int __init prcs_init(void)
{
        int total;

        pr_info("%s: inserted\n", OURMODNAME);
        total = show_prcs_in_tasklist();
        pr_info("%s: total # of processes on system: %d\n", OURMODNAME, total);

        return 0;               /* success */
}

static void __exit prcs_exit(void)
{
        pr_info("%s: removed\n", OURMODNAME);
}

module_init(prcs_init);
module_exit(prcs_exit);
