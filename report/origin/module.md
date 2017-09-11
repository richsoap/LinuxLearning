<h1 style="text-align:center">Linux实验报告:PCI驱动编写</h1>
<div class="auther" style="text-align:center">杨庆龙</div>
<div class="date" style="text-align:center">2017.8.8</div>

## 实验环境
Ubuntu17.04(Linux-4.10.0-19-generic)
## 实验原理
在linux/pci.h中定义了PCI设备的驱动程序的结构体"pci_driver"
```C
struct pci_driver {
    struct list_head node;
    char *name;
    const struct pci_device_id *id_table;
    int  (*probe)  (struct pci_dev *dev, const struct pci_device_id *id);
    void (*remove) (struct pci_dev *dev);
    int  (*save_state) (struct pci_dev *dev, u32 state);
    int  (*suspend)(struct pci_dev *dev, u32 state);
    int  (*resume) (struct pci_dev *dev);
    int  (*enable_wake) (struct pci_dev *dev, u32 state, int enable);
}
```
其中，最重要的是用于识别设备用的id_table,用于检测设备的函数probe，以及用于卸载设备的函数remove。编写这三个部分是本次实验的重点。
## 实验内容
### id_table
id_table是结构体pci_device_id的指针，因此，我们需要定义一个静态的pci_device_id用于指明驱动所对应的设备id。该结构体的定义如下，
```
struct pci_device_id {
    __u32 vendor, device;/* Vendor and device ID or PCI_ANY_ID*/
    __u32 subvendor, subdevice;/* Subsystem ID's or PCI_ANY_ID */
    __u32 class, class_mask;/* (class,subclass,prog-if) triplet */
    kernel_ulong_t driver_data;/* Data private to the driver */
};
```
因为我们主要关心设备的主设备号，所以只指定vendor和device，其他都按照默认的走。又查阅edu.txt文件得到，edu设备的设备编号为0x123:0x11e8，因此代码如下
```
static struct pci_device_id my_pci_id {
    {0x1234,0x11e8,PCI_ANY_ID,PCI_ANY_ID,0,0,MY_PCI},
    {0,}
}

```

