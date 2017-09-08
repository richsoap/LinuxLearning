#include "pci_demo.h"
#include <linux/interrupt.h>
#include <linux/completion,h>
MODULE_LICENSE("Dual BSD/GPL");

static int cleanflag = 0;
static struct pci_demo_dev *mypci_dev;
static const int WRITE = 0;
static const int READ = 1;
static int using_count = 0;
static struct completion Completion;

irqreturn_t interfunc(int irq,void*dev_id ,struct pt_regs* pt_reg) {
	struct pci_demo_dev*dev = (struct pci-demo_dev*) dev_id;
	pci_unmap_single(dev->pdev,dev->dma_addr,dev->dma_size,dev->dma_dir);
	completion(Completion);
}

int dev_open(struct inode *inode,struct file*filp) {
	//for chardev
	struct pci_demo_dev* mydev;
	mydev = container_of(inode->i_cdev,struct pci_demo_dev, chardev);
	filp->private_data = mydev;
	//for irq
	int result = 0;
	if(using_count == 0) {
		result = request_irq(mypci_dev->irq,interfunc,SA_INTERRUPT,"short",NULL);
	}
	if(result != 0) {
		return result;
	}
	//for dma
	if((result = request_dma(mypci_dev.dma_chan,"my_dma"))) {
		free_irq(mypci_dev->irq,NULL);
		return result;
	}

	using_count ++;

	return 0;
}
int dev_release(struct inode *inode,struct file*file) {
	using_count --;
	if(using_count == 0) {
		free_irq(7,NULL);
		free_dma(mypci-dev.dma_chan);
	}
	return 0;
}

int dma_trans(int dir,size_t count,int offset) {
	dma_addr_t bus_addr;
	unsigned long flags;

	mypci_dev->dma_dir =(dir==WRITE?PCI_DMA_TODEVICE:PCI_DMA_FROMDEVICE);
	dev->dma_size = count;
	bus_addr = pci_map_single(mypci_dev->pdev,mypci_dev->buffer,count,dev->dma_dir);
	dev->dma_addr = bus_addr;

	writeb(DMA_COMMAND+DMA_START,0);
	writeb(DMA_COUNT,count);
	if(dir == WRITE) {
		writeb(DMA_SOURCE_ADDR, bus_addr);
		writeb(DMA_DESTINATION_ADDR,0x40000 + offset);
		writeb(DMA_COMMAND+DMA_DIR,0);
	}
	else {
		writeb(DMA_SOURCE_ADDR, 0x40000 + offset);
		writeb(DMA_DESTINATION_ADDR,bus_addr);
		wrriteb(DMA_COMMAND+DMA_DIR,1);
	}
	writeb(DMA_COMMAND+DMA_START,1);
	return 0;

}


ssize_t dev_read(struct file*file,char *buf,size_t count,loff_t *ppos) {
	if(*ppos + count > 4096) {
		count = 4096 - *ppos;
	}
	init_completion(&Completion);
	dma_trans(READ,count,*ppos);
	wait_for_completion(&Completion);
	int i;
	for(i = 0;i < count;i ++) {
		*(buf+i) = *(mypci_dev->buf+i);
	}
	return count;
}

ssize_t dev_write(struct file *file, const char*buf,size_t count,loff_t *ppos) {
	if(*ppos + count > 4096) {
		count = 4096 - *ppos;
	}
	int i;
	for(i = 0;i < count;i ++) {
		*(mypci_dev->buf+i) = *(buf+i);
	}
	init_completion(&Completion);
	dma_trans(WRITE,count,*ppos);
	wait_for_completion(&Completion);
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
	mypci_dev->data = kmalloc(4096 ,GFP_KERNEL);
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
	mypci_dev->irq=WANT_IRQ;
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
