#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

// cac ham khac ...................................
// ........................
// ...................................................

// can cai dat 2 ham de dang ky module nhan voi nhan HDH
static int p01_init(void)
{
    printk(KERN_ALERT "Khoi tao ok\n");
	return 0;
}

static void p01_exit(void)
{
    printk(KERN_ALERT "Ket thuc ok\n");
}

//goi 2 ham duoi de dang ky module nhan voi HDH
module_init(p01_init);
module_exit(p01_exit);
