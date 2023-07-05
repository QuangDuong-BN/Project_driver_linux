#include<linux/kernel.h>

#include<linux/init.h>

#include<linux/module.h>

#include<linux/fs.h>

//1 entry point
// moi entry point phai co function prototype giong trong cau truc file_operations
static ssize_t p03_read(struct file *filp, char *buffer, size_t length, loff_t *offset);

//Kernel <= 3.0 
//ssize_t (*read) (struct file *, char __user *, size_t, loff_t *);

static int Major;

//Cấu trúc để đăng ký các hàm qua driver interface được được qui định theo chuẩn
// = dang ky cac Entry point voi kernel
static struct file_operations fops={

     .owner=THIS_MODULE,

     .read=p03_read,   

};

static int __init p03_init(void)
{

    //Đăng ký thiết bị kiểu character với major id hệ thống cung cấp
     Major=register_chrdev(0,"thietbi1",&fops);   

     if(Major<0)
     {
           printk(KERN_ALERT "Dang ky that bai\n");
    }

     else{

           printk(KERN_ALERT "Dang ky thiet bi thanh cong voi MajorID=%d\n",Major);
     }

	return 0;

}

static void __exit p03_exit(void)
{
   //Giải phóng thiết bị đã đăng ký

     unregister_chrdev(Major,"KTMT-Device");       

     printk(KERN_ALERT "Thiet bi da bi ngat khoi he thong\n");
}

//Hàm read được cung cấp qua driver interface
static ssize_t p03_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{

     return 127; //Hảm read của driver chỉ đơn giản trả về một giá trị
}

module_init(p03_init);
module_exit(p03_exit);
