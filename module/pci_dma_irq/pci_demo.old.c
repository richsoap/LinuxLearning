#include "pci_demo.h"
MODULE_LICENSE("Dual BSD/GPL");

static int cleanflag = 0;
static struct pci_demo_dev *mypci_dev;
static int using_count = 0;
static struct completion Comp;

irqreturn_t interfunc(int irq,void*dev_id) {
	printk(KERN_ERR "interfunc\n");
	struct pci_demo_dev*dev = (struct pci_demo_dev*) dev_id;
	pci_unmap_single(dev->pdev,dev->dma_addr,dev->dma_size,dev->dma_dir);
	complete(&Comp);
	return 0;
}

int dev_open(struct inode *inode,struct file*filp) {
	//for chardev
	printk(KERN_ERR "open file\n");
	int result;
	struct pci_demo_dev* mydev;
	mydev = container_of(inode->i_cdev,struct pci_demo_dev, chardev);
	filp->private_data = mydev;
	//for irq
	result = 0;
	if(using_count == 0) {
		result = request_irq(mypci_dev->irq,interfunc,IRQF_SHARED,"short",NULL);
		printk(KERN_ERR "irq result is  %d\n",result);
	}
	if(result != 0) {
		return result;
	}
	//for dma
	if((result = request_dma(mypci_dev->dma_chan,"my_dma"))) {
		printk(KERN_ERR "dma result is %d\n",result);
		free_irq(mypci_dev->irq,NULL);
		return result;
	}

	printk(KERN_ERR "open successfully\n");
	using_count ++;

	return 0;
}
int dev_release(struct inode *inode,struct file*file) {
	using_count --;
	if(using_count == 0) {
		free_irq(7,NULL);
		free_dma(mypci_dev->dma_chan);
	}
	return 0;
}

int dma_trans(int write,size_t count,int offset) {
	printk(KERN_ERR "trans dir is %d\n",write);
	dma_addr_t bus_addr;
	mypci_dev->dma_dir =(write?PCI_DMA_TODEVICE:PCI_DMA_FROMDEVICE);
	mypci_dev->dma_size = count;
	bus_addr = pci_map_single(mypci_dev->pdev,mypci_dev->buffer,count,mypci_dev->dma_dir);
	mypci_dev->dma_addr = bus_addr;
/*
writeb(0,DMA_COMMAND);
	writew(count,DMA_COUNT);
	if(write) {
		writel(bus_addr,DMA_SOURCE_ADDR);
		writel(0x40000+offset,DMA_DESTINATION_ADDR);
		writeb(0x05,DMA_COMMAND);
	}
	else {
		writel(0x40000+offset,DMA_SOURCE_ADDR);
		writel(bus_addr,DMA_DESTINATION_ADDR);
		writeb(0x07,DMA_COMMAND);
	}

*/
	writeb(0,0x98);
	writew(count,0x90);
	if(write) {
		writel(bus_addr,0x80);
		writel(0x40000+offset,0x88);
		writeb(0x05,0x98);
	}
	else {
		writel(0x40000+offset,0x80);
		writel(bus_addr,0x88);
		writeb(0x07,0x98);
	}
	return 0;

}


ssize_t dev_read(struct file*file,char *buf,size_t count,loff_t *ppos) {
	printk(KERN_ERR "pci_demo read\n");
	if(*ppos + count > 4096) {
		count = 4096 - *ppos;
	}
	init_completion(&Comp);
	dma_trans(0,count,*ppos);
	wait_for_completion(&Comp);
	strncpy(buf,mypci_dev->buffer,count*sizeof(char));
	return count;
}

ssize_t dev_write(struct file *file, const char*buf,size_t count,loff_t *ppos) {
	printk(KERN_ERR "pci_demo write\n");
	if(*ppos + count > 4096) {
		count = 4096 - *ppos;
	}
	strncpy(mypci_dev->buffer,buf,count*sizeof(char));
	init_completion(&Comp);
	dma_trans(1,count,*ppos);
	wait_for_completion(&Comp);
	return count;
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
		case 4:
			kfree(mypci_dev->addr);
		case 3:
		cdev_del(&mypci_dev->chardev);
		case 2:
		kfree(mypci_dev->buffer);
		case 1:
		kfree(mypci_dev);
		default:
		cleanflag = 0;
	}
}


static int dev_probe(struct pci_dev *pdev,const struct pci_device_id *ids) {
	int result;
	printk(KERN_ERR "probe dev\n");
	mypci_dev = kmalloc(sizeof(struct pci_demo_dev), GFP_KERNEL);
	if(IS_ERR(mypci_dev)) {
		printk(KERN_ERR "kmall struct error/n");
		return -1;
	}
	strncpy(mypci_dev->devname,CHARDEV_NAME,sizeof(CHARDEV_NAME));
	mypci_dev->buffer = kmalloc(4096 ,GFP_KERNEL);
	cleanflag ++;
	if(IS_ERR(mypci_dev->buffer)) {
		dev_clean();
		printk(KERN_ERR "kmall data error\n");
		return -1;
	}
	if((alloc_chrdev_region(&(mypci_dev->dev_num),0,1,mypci_dev->devname))) {
		dev_clean();
		printk(KERN_ERR "chrdev error\n");
		return -1;
	}
	cleanflag ++;
	mypci_dev->dev_class = class_create(THIS_MODULE,mypci_dev->devname);
	cdev_init(&(mypci_dev->chardev),&fops);
	mypci_dev->chardev.owner = THIS_MODULE;
	result = cdev_add(&(mypci_dev->chardev),mypci_dev->dev_num,1);
	if(result) {
		printk(KERN_ERR "add error\n");
		dev_clean();
		return result;
	}
	cleanflag ++;
	mypci_dev->addr = kmalloc(sizeof(dma_addr_t),GFP_KERNEL);
	if(IS_ERR(mypci_dev->addr)) {
		printk(KERN_ERR "addr error\n");
		dev_clean();
	}
	cleanflag ++;
	mypci_dev->dev=device_create(mypci_dev->dev_class,NULL,MKDEV(MAJOR(mypci_dev->dev_num),0),mypci_dev,mypci_dev->devname);
	mypci_dev->pdev=pdev;
	mypci_dev->irq=WANT_IRQ;
	printk(KERN_ERR "probe done\n");
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
