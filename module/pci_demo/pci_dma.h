#ifndef PCI_DEMO_H
#define PCI_DEMO_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <asm/io.h>

#define CHARDEV_NAME "my_pci"
#define MAXLENGTH 4096

#define DMA_SOURCE_ADDR 0x80
#define DMA_DESTINATION_ADDR 0x88
#define DMA_COUNT 0x90
#define DMA_COMMAND 0x98
#define DMA_INTERRUPT 0x100

#define DMA_START 0x01
#define DMA_DIR 0x02
#define WHAT_IRQ 7


typedef struct pci_demo_dev {
	char devname[24];
	int dev_num;
	struct class *dev_class;
	struct device *dev;
	struct pci_dev *pdev;
	struct cdev chardev;
	int majorNuber;
	struct dma_addr_t addr;
	struct dma_chan *dma_chan;
	void* buffer;
	int irq;

	size_t dma_size;
	int dma_dir;
	dma_addr_t dma_addr;

};

static struct pci_device_id pcie_tbl[] = {
	{0x1234,0x11e8,PCI_ANY_ID,PCI_ANY_ID,0,0,0},
	{0,}
};

#endif
