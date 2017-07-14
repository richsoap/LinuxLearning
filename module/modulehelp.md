##设备号
在内核中，dev_t类型(linux/types.h)用来表示设备编号
```C
MAJOR(dev_t dev);//dev_t -> int
MINOR(dev_t dev);//dev_t -> int

MkKEV(int major,int minor);//int -> dev_t
```
## register and release
###register
```C
static function
---------------------
int register_chrdev_region(dev_t first,unsigned int count,char *name);
first is the first device number
count is the total number we need
name is the name of our device
---------------------
dynamic function
---------------------
int alloc_chrdev_region(dev_t *dev, unsigned int firstminor, unsigned int count, char *name);
dev will be our first device number
first minor is our first minor number
count and name is the same as register_chrdev_region
```
### release
```C
void unregister_chrdev_region(dev_t first, unsigned int count);
```
## file operation
included in <linux/fs.h>
### struct module *owner;
It is directed to its owner, so that system will be able to stop uninstalling device which is being used.
### loff_t(*llseek)(stuct file *,loff_t,int);
llseek function is used to change read/write position and return the new position(positive).loff_t is a "long offset".
### ssize_t(*read)(struct file*,char __user*,size_t, loff_t*);
It is used to read data from device. Null will lead to "Invalid argument". Returning positive number means read successfully.
### ssize_t(*aio_read)(struct kiocb *,char __user*, size_t,loff_t);
This is an asynchronous reading function.If it is null, it will be replaced by read.
### ssize_t(*write)(struct file*,const char __user*,size_t,loff_t*);
Send data to device. If it is NULL, all return will be "-EINVAL".
### ssize_t(*aio_write)(struct kiocb*,const char __user*,size_t,loff_t *);
An asynchronous writing function.
### int(*readdir)(struct file *,void*,filldir_t);
Read dirctory, only for file system.
### unsigned int (*poll)(struct file*,struct poll_table_struct*);
It will be used by poll, epoll and select to decide RW mode.
### int(*ioctl)(struct inode*,struct file*,unssigned int,unsigned long) ;
ioctl is used for some specical movements, like sleep a device and so on, which are not read or write.
### int(*mmap)(struct file*,struct vmr_area_struct*);
It is used to get address space. NULL means -ENODEV.
### int(*open)(struct inode*,struct file*);
It is used to open a device. It can be NULL and the device will always be open.
### int(*flush)(struct file*);
Flush.
### int(*release)(struct inode*,struct file*);
Close.
### int(*fsync)(struct file*,struct dentry*,int);
It is used to refresh data.
### int(*aio_fsync)(int,struct kiocb*,int);
It is an asynchronous fsync.
### int(*fasync)(int,struct file*.int);
It is used to change FASYNC flag.
### int(*lock)(struct file*,innt,struct file_lock);
Never used in device.
### ...

#### example
These are the most important ones.
```C
struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.llseek = scull_llseek,
	.read = scull_read,
	.write = scull_write,
	.ioctl = scull_ioctl,
	.open = scull_open,
	.release = scull_release,
};
```
## struct file
defined in <linux/fs.h>
### mode_t f_mode;
FMODE_READ, FOMDE_WRITE
### unsigned int f_flags;
All flags are defined in <linux/fcntl.h>
O_RDONLY, O_NONBLOCK(blocked or not) and O_SYNC
## struct inode
###dev_t i_rdev;
These units stand for the true device number.
### struct cdev*i_cdev;
It will lead to a chardevice.
### etc
unsigned int iminor(struct inode *inode);
unsigned int imajor(struct inode*inode);
### notice
"release" will only be called when every copy of file is closed.
## register chardevice
<linux/cdev.h>
###get cdev
struct cdev *my_cdev = cdev_alloc();
my_cdev->ops = &my_fops;
### init cdev
void  cdev_init(struct cdev *cdev, struct file_operations *fops);
### add cdev
int cdev_add(struct cdev *cdev, dev_t num, unsigned int count);
###  delete cdev
void cdev_del(struct cdev *dev);
## memory use
<linux/slab.h>
###void kmalloc(size_t size,int flags);
This function is used to get a block of memory in size. Return a point to the area, null when error.

"flags" is used to describe how to get memory. "GFP_KERNEL".
###void kfree(void *ptr);
It is used to free memory, ptr is the point.
