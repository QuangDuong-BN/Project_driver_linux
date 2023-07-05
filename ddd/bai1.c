#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <crypto/skcipher.h>
#include <linux/scatterlist.h>

#define DEVICE_NAME "encryption_device"
#define KEY_SIZE 16
#define BUFFER_SIZE 1024

static char encryption_key[KEY_SIZE];

static struct crypto_skcipher *tfm;

static int major_number;

static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device opened.\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device closed.\n");
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
    struct skcipher_request *req;
    struct scatterlist sg_in, sg_out;
    char decrypted_data[BUFFER_SIZE];
    int ret;

    if (length > BUFFER_SIZE)
    {
        printk(KERN_INFO "Data size exceeds buffer size.\n");
        return -EINVAL;
    }

    req = skcipher_request_alloc(tfm, GFP_KERNEL);
    if (!req)
    {
        printk(KERN_INFO "Failed to allocate request.\n");
        return -ENOMEM;
    }

    sg_init_one(&sg_in, buffer, length);
    sg_init_one(&sg_out, decrypted_data, length);

    skcipher_request_set_crypt(req, &sg_in, &sg_out, length, encryption_key);

    ret = crypto_skcipher_decrypt(req);
    if (ret)
    {
        printk(KERN_INFO "Decryption failed.\n");
        skcipher_request_free(req);
        return ret;
    }

    skcipher_request_free(req);

    if (copy_to_user(buffer, decrypted_data, length))
    {
        printk(KERN_INFO "Failed to copy decrypted data to userspace.\n");
        return -EFAULT;
    }

    printk(KERN_INFO "Data decrypted and sent back to userspace.\n");
    return length;
}

static ssize_t device_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
    struct skcipher_request *req;
    struct scatterlist sg_in, sg_out;
    char encrypted_data[BUFFER_SIZE];
    int ret;

    if (length > BUFFER_SIZE)
    {
        printk(KERN_INFO "Data size exceeds buffer size.\n");
        return -EINVAL;
    }

    if (copy_from_user(encryption_key, buffer, KEY_SIZE))
    {
        printk(KERN_INFO "Failed to copy key from userspace.\n");
        return -EFAULT;
    }

    req = skcipher_request_alloc(tfm, GFP_KERNEL);
    if (!req)
    {
        printk(KERN_INFO "Failed to allocate request.\n");
        return -ENOMEM;
    }

    sg_init_one(&sg_in, buffer + KEY_SIZE, length - KEY_SIZE);
    sg_init_one(&sg_out, encrypted_data, length - KEY_SIZE);

    skcipher_request_set_crypt(req, &sg_in, &sg_out, length - KEY_SIZE, encryption_key);

    ret = crypto_skcipher_encrypt(req);
    if (ret)
    {
        printk(KERN_INFO "Encryption failed.\n");
        skcipher_request_free(req);
        return ret;
    }

    skcipher_request_free(req);

    if (copy_to_user((char *)buffer + KEY_SIZE, encrypted_data, length - KEY_SIZE))
    {
        printk(KERN_INFO "Failed to copy encrypted data to userspace.\n");
        return -EFAULT;
    }

    printk(KERN_INFO "Data encrypted and sent back to userspace.\n");
    return length;
}

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};

static int __init encryption_device_init(void)
{
    tfm = crypto_alloc_skcipher("ecb(aes)", 0, 0);
    if (IS_ERR(tfm))
    {
        printk(KERN_ALERT "Failed to allocate transform handle.\n");
        return PTR_ERR(tfm);
    }

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0)
    {
        printk(KERN_ALERT "Failed to register encryption device with %d\n", major_number);
        crypto_free_skcipher(tfm);
        return major_number;
    }

    printk(KERN_INFO "Encryption device registered with major number %d.\n", major_number);
    return 0;
}

static void __exit encryption_device_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    crypto_free_skcipher(tfm);
    printk(KERN_INFO "Encryption device unregistered.\n");
}

module_init(encryption_device_init);
module_exit(encryption_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Encryption Device Driver");
