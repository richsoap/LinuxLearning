#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/cdev.h>

#include <linux/uaccess.h>


MODULE_LICENSE("Dual BSD/GPL");

static int scull_major = 0;
static int scull_minor = 0;
static int scull_quantum = 4096;
static int scull_qset = 1000;

/*MODULE_PARM(scull_major,"i");
MODULE_PARM(scull_minor,"i");
MODULE_PARM(scull_quantum,"i");
MODULE_PARM(scull_qset,"i");
*/
struct scull_qset {
	void **data;
	struct scull_qset *next;
};

typedef struct scull_dev {
	struct scull_qset *data;
	struct scull_dev * next;
	int quantum;
	int qset;
	unsigned long size;
	struct cdev cdev;
}; 


struct scull_dev* myscull;

int scull_open(struct inode *inode, struct file* filp); 
int scull_release(struct inode *inode,struct file*filp);
ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t scull_write(struct file*filp, char __user *buf, size_t count, loff_t *f_pos); 
void scull_cleanup(void);

struct file_operations scull_fops = {
	read:scull_read,
	open:scull_open,
    write:(void *)scull_write,
	release:scull_release,
};




struct scull_qset *scull_follow(struct scull_dev *dev,int n) {
	struct scull_qset * out;
	out = dev->data;
	while(n--) {
		if(!out->next) {
			out->next=kmalloc(sizeof(struct scull_qset),GFP_KERNEL);
			memset(out->next,0,sizeof(struct scull_qset));
		}
		out=out->next;
	}
	return out;
}

int scull_trim(struct scull_dev *dev) {
	struct scull_qset *next;
	int qset = dev->qset;
	int i;
	struct scull_qset* dptr;
	for (dptr = dev->data;dptr != NULL;dptr = next) {
		if(dptr->data) {

			for(i = 0;i < qset;i ++) 
			  kfree(dptr->data[i]);
			kfree(dptr->data);
			dptr->data=NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = scull_quantum;
	dev->qset = scull_qset;
	dev->data = NULL;
	return 0;
}

void scull_setup_cdev(struct scull_dev *dev,int index) {
	int err,devno;
	devno = MKDEV(scull_major,scull_minor +  index);

	cdev_init(&dev->cdev,&scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;
	err = cdev_add(&dev->cdev,devno,1);
	if(err)
	  printk(KERN_NOTICE "Error %d adding scull%d",err,index);
}

int scull_open(struct inode *inode, struct file* filp) {
	struct scull_dev *dev;
	dev = container_of(inode->i_cdev,struct scull_dev,cdev);
	filp->private_data = dev;

	if((filp->f_flags & O_ACCMODE) == O_WRONLY) {
		scull_trim(dev);
	}
	return 0;
}

int scull_release(struct inode *inode,struct file*filp) {
	return 0;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos){
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int rest;
	int itemsize =  quantum * qset;
	
	int item,s_pos,q_pos;
	ssize_t retval = 0;

	if(*f_pos >= dev->size)
	  goto out;
	if(*f_pos+count>dev->size)
	  count = dev->size - *f_pos;

	item = (long)*f_pos/itemsize;
	rest = (long)*f_pos%itemsize;
	s_pos = rest / quantum;
	q_pos = rest % quantum;

	dptr = scull_follow(dev,item);
	if(dptr == NULL || !dptr->data || !dptr->data[s_pos])
	  goto out;

	if(count > quantum - q_pos)
	  count = quantum - q_pos;

	if(copy_to_user(buf,dptr->data[s_pos] + q_pos,count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;
out:
	return retval;

}

ssize_t scull_write(struct file*filp, char __user *buf, size_t count, loff_t *f_pos) {
	struct scull_dev *dev = filp->private_data;
	struct scull_qset *dptr;
	int quantum = dev->quantum,qset = dev->qset;
	int itemsize = quantum * qset;
	int  item,s_pos,q_pos,rest;
	ssize_t retval=-ENOMEM;
	

	item = (long)*f_pos / itemsize;
	rest = (long)*f_pos %  itemsize;
	s_pos = rest / quantum;
	q_pos = rest % quantum;

	dptr  = scull_follow(dev,item);
	if(dptr == NULL)
	  goto out;
	if(!dptr->data) {
		dptr->data = kmalloc(qset * sizeof(char *),GFP_KERNEL);
		if(!dptr->data)
		  goto out;
		memset(dptr->data,0,qset*sizeof(char*));
	}
	if(!dptr->data[s_pos]) {
		dptr->data[s_pos] = kmalloc(quantum,GFP_KERNEL);
		if(!dptr->data[s_pos])
		  goto out;
	}

	if(count > quantum - q_pos)
	  count = quantum - q_pos;
	if(copy_from_user(dptr->data[s_pos]+q_pos,buf,count)) {
		retval = -EFAULT;
		goto out;
	}
	*f_pos += count;
	retval = count;

	if(dev->size < *f_pos)
	  dev->size = *f_pos;
out:
	return retval;
}


int __init scull_init(void) {
	int result,i;
//	SET_MODULE_OWNER(&scull_fops);
	(&scull_fops)->owner = THIS_MODULE;
	result  = register_chrdev(scull_major,"scull",&scull_fops);
	if(result < 0) {
		printk(KERN_WARNING "scull: Can't get major %d\n",scull_major);
		return result;
	}
	if(scull_major  == 0)
	  scull_major=result;

	myscull = kmalloc(sizeof(struct scull_dev),GFP_KERNEL);
	if(!myscull) {
		result = -ENOMEM;
		goto fail;
	}
	memset(myscull,0,sizeof(struct scull_dev));
	myscull->quantum  = scull_quantum;
	myscull->qset = scull_qset;
	scull_setup_cdev(myscull,0);
	return 0;

fail:
	scull_cleanup();
	return result;
}

void scull_cleanup(void) {
	int i;
	unregister_chrdev(myscull,"scull");
	scull_trim(myscull);
	kfree(myscull);
	
}



module_init(scull_init);
module_exit(scull_cleanup);
