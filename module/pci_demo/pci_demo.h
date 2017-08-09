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

#define CHARDEV_NAME "pic_chardev"
#define MAXLENGTH 4096

typedef struct pci_demo_dev {
	char devname[24];
	void *data;
	int dev_num;
	int length;
	struct cdev chardev;
	struct class *dev_class;
	struct device *dev;
	struct pci_dev *pdev;
};

static struct pci_device_id pcie_tbl[] = {
	{0x104c,0x823f,PCI_ANY_ID,PCI_ANY_ID,0,0,0},

	{0x1234,0x11e8,PCI_ANY_ID,PCI_ANY_ID,0,0,0},
	{0,}
};

#endif
