#include<linux/init.h>
#include<linux/module.h>

int GiaiThua(int n);
void InMaTran(int n);

static char *ten = "Viet Nam";
static int giatri = 6;

module_param(giatri, int, S_IRUGO);
module_param(ten, charp, S_IRUGO);

static int p02_init(void)
{
	printk(KERN_ALERT "\n%s\n", ten);

    	printk(KERN_ALERT "Khoi tao ok\n");
        
        printk(KERN_INFO "\n%d! = %d",giatri, GiaiThua(giatri));

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
                        printk(KERN_ALERT "%d\t", j);
                }

                printk(KERN_ALERT "\n");
	}
}

static void p02_exit(void)
{
    printk(KERN_ALERT "Ket thuc ok\n");

	int n = 6;
	printk(KERN_ALERT "\n6! = %d", GiaiThua(n));

	InMaTran(9);
}

module_init(p02_init);
module_exit(p02_exit);