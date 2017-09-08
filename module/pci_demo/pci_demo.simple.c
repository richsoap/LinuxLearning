#include "pci_demo.h"
MODULE_LICENSE("Dual BSD/GPL");

static int cleanflag = 0;
static struct pci_demo_dev *mypci_dev;

void dev_clean(void) {
	switch(cleanflag) {
		default:
		cleanflag = 0;
	}
}


static int dev_probe(struct pci_dev *pdev,const struct pci_device_id *ids) {
	mypci_dev = kmalloc(sizeof(struct pci_demo_dev), GFP_KERNEL);
	if(IS_ERR(mypci_dev)) {
		printk(KERN_ERR "kmall struct error");
		return -1;
	}
	strncpy(mypci_dev->devname,CHARDEV_NAME,sizeof(CHARDEV_NAME));
	cleanflag ++;
	if((alloc_chrdev_region(&(mypci_dev->dev_num),0,1,mypci_dev->devname))) {
		dev_clean();
		printk(KERN_ERR "chrdev error");
		return -1;
	}
	mypci_dev->dev_class = class_create(THIS_MODULE,mypci_dev->devname);
	mypci_dev->dev=device_create(mypci_dev->dev_class,NULL,MKDEV(MAJOR(mypci_dev->dev_num),0),mypci_dev,mypci_dev->devname);
	mypci_dev->pdev=pdev;
	pci_set_drvdata(pdev,mypci_dev);
	return 0;
}

static void dev_remove(struct pci_dev *pdev) {
	device_destroy(mypci_dev->dev_class,MKDEV(MAJOR(mypci_dev->dev_num),MINOR(mypci_dev->dev_num)));
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
