#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_AUTHOR("Shriram");
MODULE_DESCRIPTION("Hello Linux");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");

static int __init helloworld_lkm_init(void) {
        printk (KERN_INFO "Hello Linux, my love >3\n");
        return 0;
}

static void __exit helloworld_lkm_exit(void) {
        printk(KERN_INFO "Goodbye, world\n");
}

module_init(helloworld_lkm_init);
module_exit(helloworld_lkm_exit);
