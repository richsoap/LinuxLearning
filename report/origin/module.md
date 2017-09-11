<h1 style="text-align:center">Linuxʵ�鱨��:PCI������д</h1>
<div class="auther" style="text-align:center">������</div>
<div class="date" style="text-align:center">2017.8.8</div>

## ʵ�黷��
Ubuntu17.04(Linux-4.10.0-19-generic)
## ʵ��ԭ��
��linux/pci.h�ж�����PCI�豸����������Ľṹ��"pci_driver"
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
���У�����Ҫ��������ʶ���豸�õ�id_table,���ڼ���豸�ĺ���probe���Լ�����ж���豸�ĺ���remove����д�����������Ǳ���ʵ����ص㡣
## ʵ������
### id_table
id_table�ǽṹ��pci_device_id��ָ�룬��ˣ�������Ҫ����һ����̬��pci_device_id����ָ����������Ӧ���豸id���ýṹ��Ķ������£�
```
struct pci_device_id {
    __u32 vendor, device;/* Vendor and device ID or PCI_ANY_ID*/
    __u32 subvendor, subdevice;/* Subsystem ID's or PCI_ANY_ID */
    __u32 class, class_mask;/* (class,subclass,prog-if) triplet */
    kernel_ulong_t driver_data;/* Data private to the driver */
};
```
��Ϊ������Ҫ�����豸�����豸�ţ�����ָֻ��vendor��device������������Ĭ�ϵ��ߡ��ֲ���edu.txt�ļ��õ���edu�豸���豸���Ϊ0x123:0x11e8����˴�������
```
static struct pci_device_id my_pci_id {
    {0x1234,0x11e8,PCI_ANY_ID,PCI_ANY_ID,0,0,MY_PCI},
    {0,}
}

```

