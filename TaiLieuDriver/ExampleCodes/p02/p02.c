#include<linux/init.h>
#include<linux/module.h>

int GiaiThua(int n);
void InMaTran(int n);

static int p02_init(void)
{
    printk(KERN_ALERT "Khoi tao ok\n");
        
	int n = 6;
        printk("\n6! = %d", GiaiThua(n));

        InMaTran(9);
        return 0;
}

int GiaiThua(int n)
{
        int gt = 2;
        int i = 3;

        for(i=3; i<=n; i++)
        {
                gt *= i;
        }

        return gt;
}

void InMaTran(int n)
{
        int i, j;

        for(i=1; i<=n; i++)
        {
                for(j=1; j<=n; j++)
                {
                        printk("%d\t", j);
                }

                printk("\n");
	}
}

static void p02_exit(void)
{
    printk(KERN_ALERT "Ket thuc ok\n");

	int n = 6;
	printk("\n6! = %d", GiaiThua(n));

	InMaTran(9);
}

module_init(p02_init);
module_exit(p02_exit);
