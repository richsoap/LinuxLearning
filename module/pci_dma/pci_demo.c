#include "pci_demo.h"
#include <asm/delay.h>
MODULE_LICENSE("Dual BSD/GPL");

static int cleanflag = 0;
static struct pci_demo_dev *mypci_dev;
static int using_count = 0;
static struct completion Comp;

irqreturn_t probeirq(int irq,void*dev_id) {
	mypci_dev->irq = irq;
	return 0;
}

irqreturn_t interfunc(int irq,void*dev_id) {
	struct pci_demo_dev*dev;
	complete(&Comp);
	printk(KERN_ERR "interfunc\n");
	dev = (struct pci_demo_dev*) dev_id;
	if(dev == mypci_dev->dev) {
		
	}
	pci_unmap_single(dev->pdev,dev->dma_addr,dev->dma_size,dev->dma_dir);
	return 0;
}

int dev_open(struct inode *inode,struct file*filp) {
	int result;
	//for chardev
	int i;
	struct pci_demo_dev* mydev;
	printk(KERN_ERR "open file\n");
	mydev = container_of(inode->i_cdev,struct pci_demo_dev, chardev);
	filp->private_data = mydev;
	//for irq
	result = 0;
	if(using_count == 0) {
		mypci_dev->irq=mypci_dev->irq<=0?4:mypci_dev->irq;
		enable_irq(mypci_dev->irq);
		result = request_irq(mypci_dev->irq,interfunc,IRQF_SHARED,"my_pci",mypci_dev->dev);
		printk(KERN_ERR "irq result is %d\n",result);

		if(result != 0) {
			disable_irq(mypci_dev->irq);
			return result;
		}
	//for dma
		for(i = 0;i < MAX_DMA_CHANNELS;i ++) {
			result = request_dma(i,"my_dma");
			if(result == 0) {
				mypci_dev->dma_chan = i;
				break;
			}
		}
		printk(KERN_ERR "dma result is %d\n",result);
		if(result) {
			free_irq(mypci_dev->irq,mypci_dev->dev);
			return result;
		}
	
	}

	printk(KERN_ERR "open successfully\n");
	using_count ++;

	return 0;
}
int dev_release(struct inode *inode,struct file*file) {
	using_count --;
	if(using_count == 0) {
		free_irq(mypci_dev->irq,mypci_dev->dev);
		disable_irq(mypci_dev->irq);
		free_dma(mypci_dev->dma_chan);
	
	}
	return 0;
}

int dma_trans(int write,size_t count,int offset) {
	int i;
	u8 inter;
	dma_addr_t bus_addr;
	printk(KERN_ERR "trans direction is %d\n",write);
	mypci_dev->dma_dir =(write?PCI_DMA_TODEVICE:PCI_DMA_FROMDEVICE);
	mypci_dev->dma_size = count;
	bus_addr = pci_map_single(mypci_dev->pdev,mypci_dev->buffer,count,mypci_dev->dma_dir);
	mypci_dev->dma_addr = bus_addr;
	printk(KERN_ERR "addr has been done\n");

writeb(0,DMA_COMMAND+mypci_dev->base);
	writew(count,DMA_COUNT+mypci_dev->base);
	if(write) {
		writel(bus_addr,DMA_SOURCE_ADDR+mypci_dev->base);
		writel(0x40000+offset,DMA_DESTINATION_ADDR+mypci_dev->base);
		writeb(0x05,DMA_COMMAND+mypci_dev->base);
	}
	else {
		writel(0x40000+offset,DMA_SOURCE_ADDR+mypci_dev->base);
		writel(bus_addr,DMA_DESTINATION_ADDR+mypci_dev->base);
		writeb(0x07,DMA_COMMAND+mypci_dev->base);
	}
	for(i = 0;i < 1000;i ++) {
		inter = readb(mypci_dev->base + 0x100);	
		if(inter & 0xffff) {
			complete(&Comp);
pci_unmap_single(mypci_dev->pdev,mypci_dev->dma_addr,mypci_dev->dma_size,mypci_dev->dma_dir);
			return count;
		}
	}
pci_unmap_single(mypci_dev->pdev,mypci_dev->dma_addr,mypci_dev->dma_size,mypci_dev->dma_dir);
	complete(&Comp);
	return 0;

}


ssize_t dev_read(struct file*file,char *buf,size_t count,loff_t *ppos) {
	int result;
	printk(KERN_ERR "pci_demo read\n");
	if(*ppos + count > 4096) {
		count = 4096 - *ppos;
	}
	init_completion(&Comp);
	result = dma_trans(0,count,*ppos);
	printk(KERN_ERR "return to read\n");
	wait_for_completion(&Comp);
	copy_to_user(buf,mypci_dev->buffer,count);
	printk(KERN_ERR "read done\n");
	return result;
}

ssize_t dev_write(struct file *file, const char*buf,size_t count,loff_t *ppos) {
	int result;
	printk(KERN_ERR "pci_demo write\n");
	if(*ppos + count > 4096) {
		count = 4096 - *ppos;
	}
	copy_from_user(mypci_dev->buffer,buf,count);
	init_completion(&Comp);
	result = dma_trans(1,count,*ppos);
	printk(KERN_ERR "return to write\n");
	wait_for_completion(&Comp);
	printk(KERN_ERR "write done\n");
	return result;
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
		case 5:
		release_mem_region(mypci_dev->start,mypci_dev->length);
		case 4:
		kfree(mypci_dev->addr);
		case 3:
		cdev_del(&mypci_dev->chardev);
		case 2:
		class_destroy(mypci_dev->dev_class);
		kfree(mypci_dev->buffer);
		case 1:
		kfree(mypci_dev);
		default:
		cleanflag = 0;
	}
}


static int dev_probe(struct pci_dev *pdev,const struct pci_device_id *ids) {
	unsigned long  result;
	int i,p;
	u32 *val;
	printk(KERN_ERR "probe dev\n");
	val = kmalloc(sizeof(u32),GFP_KERNEL);
	for(i = 0;i < 5;i ++) {
			pci_read_config_dword(pdev,i*4,val);
			printk(KERN_ERR "%x ",*val);
		printk("\n");
	}

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
	mypci_dev->start = 0xfea00000;
	mypci_dev->length = 0x100000;
mypci_dev->ioresource = request_mem_region(mypci_dev->start,mypci_dev->length,mypci_dev->devname);
	if(mypci_dev->ioresource == NULL) {
		printk(KERN_ERR "resource failed\n");
		dev_clean();
		return 21;
	}
	cleanflag ++;
	mypci_dev->base = (void __iomem *)ioremap(mypci_dev->start,mypci_dev->length);
	printk(KERN_ERR "start and end is %x,%x\nbase is %x\n",mypci_dev->ioresource->start,mypci_dev->ioresource->end,mypci_dev->base);
	mypci_dev->irq = 0;
	for(i = 3;i < 8;i ++) {
		result = request_irq(i,probeirq,IRQF_SHARED,"my_pci",mypci_dev->dev);
		if(result) {
			printk(KERN_ERR "irq %d result error\n",i);
			continue;
		}
		for(p = 0;p < 3;p ++) {
			writeb(0,0x60+mypci_dev->base);
			writeb(0,0x64+mypci_dev->base);
			udelay(500);
			writeb(0xff,0x60+mypci_dev->base);
			udelay(500);
			writeb(0xff,0x64+mypci_dev->base);
			udelay(500);
			writeb(0,0x60+mypci_dev->base);
			writeb(0,0x64+mypci_dev->base);
			udelay(500);
			if(mypci_dev->irq != 0) {
				printk(KERN_ERR "irq is %d",i);
				break;
			}
		}
		free_irq(i,mypci_dev->dev);
		if(mypci_dev->irq != 0)
		  break;
		printk(KERN_ERR "irq %d no echo\n",i);
	}
	if(mypci_dev->irq == 0) {
		printk(KERN_ERR "irq can't get\n");
		mypci_dev->irq = -1;
	}
	printk(KERN_ERR "\nirq done\n");
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
