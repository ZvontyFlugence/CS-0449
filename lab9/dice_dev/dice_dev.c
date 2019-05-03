// Z'vonty Flugence (zaf17)

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/random.h>

#include <asm/uaccess.h>

static int max_value = 6;
module_param(max_value, int, S_IRUGO|S_IWUSR);

static ssize_t dice_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos)
{
	// allocate buffer of given bytes on stack
	unsigned char* buffer = kmalloc(count, GFP_KERNEL);
	// fill buffer with random bytes (0-255)
	get_random_bytes(buffer, count);
	// Set each byte in range of (0-5)
	int i = 0;
	for(; i < count; i++) {
		buffer[i] %= max_value;
	}
	*ppos = *ppos + count;

	if(copy_to_user(buf, buffer, count)) {
		return -EINVAL;
	}

	kfree(buffer);
	return count;
}

static const struct file_operations dice_fops = {
	.owner		= THIS_MODULE,
	.read		= dice_read,
};

static struct miscdevice dice_dev = {
	MISC_DYNAMIC_MINOR,
	"dice",
	&dice_fops
};

static int __init
dice_init(void)
{
	int ret;

	ret = misc_register(&dice_dev);
	if (ret)
		// printk(KERN_ERR
		//        "Unable to register \"Hello, world!\" misc device\n");

	return ret;
}

module_init(dice_init);

static void __exit
dice_exit(void)
{
	misc_deregister(&dice_dev);
}

module_exit(dice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Z'vonty Flugence (zaf17)");
MODULE_DESCRIPTION("Returns randomly generated rolls of a 6-sided die");
MODULE_VERSION("dev");
