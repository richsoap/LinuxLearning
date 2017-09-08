#include "pci_demo.h"
MODULE_LICENSE("Dual BSD/GPL");

static int cleanflag = 0;
static struct pci_demo_dev *mypci_dev;

int dev_open(struct inode *inode,struct file*filp) {
	struct pci_demo_dev* mydev;
	mydev = container_of(inode->i_cdev,struct pci_demo_dev, chardev);
	filp->private_data = mydev;
	return 0;
}
int dev_release(struct inode *inode,struct file*file) {
	////////////
	return 0;
}
ssize_t dev_read(struct file*file,char *buf,size_t count,loff_t *ppos) {
	/////////////
	return 0;
}

ssize_t dev_write(struct file *file, const char*buf,size_t count,loff_t *ppos) {
	/////////////
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dev_open,
	.release = dev_release,
	.read = dev_read,
	.write = dev_write,
};

void dev_clean(void) {
	switch(cleanflag) {
		case 2:
		kfree(mypci_dev->data);
		case 1:
		kfree(mypci_dev);
		default:
		cleanflag = 0;
	}
}


static int dev_probe(struct pci_dev *pdev,const struct pci_device_id *ids) {
	int result;
	mypci_dev = kmalloc(sizeof(struct pci_demo_dev), GFP_KERNEL);
	if(IS_ERR(mypci_dev)) {
		printk(KERN_ERR "kmall struct error");
		return -1;
	}
	strncpy(mypci_dev->devname,CHARDEV_NAME,sizeof(CHARDEV_NAME));
	mypci_dev->data = kmalloc(sizeof(char) * MAXLENGTH,GFP_KERNEL);
	cleanflag ++;
	if(IS_ERR(mypci_dev->data)) {
		dev_clean();
		printk(KERN_ERR "kmall data error");
		return -1;
	}
	if((alloc_chrdev_region(&(mypci_dev->dev_num),0,1,mypci_dev->devname))) {
		dev_clean();
		printk(KERN_ERR "chrdev error");
		return -1;
	}
	cleanflag ++;
	mypci_dev->dev_class = class_create(THIS_MODULE,mypci_dev->devname);
	cdev_init(&(mypci_dev->chardev),&fops);
	mypci_dev->chardev.owner = THIS_MODULE;
	result = cdev_add(&(mypci_dev->chardev),mypci_dev->dev_num,1);
	if(result) {
		printk("add error");
		kfree(mypci_dev->data);
		kfree(mypci_dev);
		return result;
	}
	mypci_dev->dev=device_create(mypci_dev->dev_class,NULL,MKDEV(MAJOR(mypci_dev->dev_num),0),mypci_dev,mypci_dev->devname);
	mypci_dev->pdev=pdev;
	pci_set_drvdata(pdev,mypci_dev);
	return 0;
}

static void dev_remove(struct pci_dev *pdev) {
	device_destroy(mypci_dev->dev_class,MKDEV(MAJOR(mypci_dev->dev_num),MINOR(mypci_dev->dev_num)));
	cdev_del(&mypci_dev->chardev);
	unregister_chrdev_region(mypci_dev->dev_num,1);
	dev_clean();
}

static struct pci_driver pcie_dev = {
	.name = CHARDEV_NAME,
	.id_table = pcie_tbl,
	.probe = dev_probe,
	.remove = dev_remove,
};


static int __init pci_demo_init(void) {
	return pci_register_driver(&pcie_dev);
}
static void  __exit pci_demo_exit(void) {
	pci_unregister_driver(&pcie_dev);
}

module_init(pci_demo_init);
module_exit(pci_demo_exit);
