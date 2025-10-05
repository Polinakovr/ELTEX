#include <linux/kernel.h>
#include <linux/module.h>
static int __init test_init(void)
{
    pr_info("\nHELLO, modules loaded\n");
    return 0;
}
static void __exit test_exit(void)
{
    pr_info("\nBye,modules unloaded\n");
}
module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");
