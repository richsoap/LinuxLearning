Example:
Ring buffer in NETWORK card
Filesystem buffers written/read by a SCSI deviice
## DMA mapping
### Consistent DMA mapping
```
dma_addr_t dma_handle;
cpu_addr = dma_alloc_coherent(dev,size.&dma_handle,gfp);

dev: is a struct device *;
gfp: usually GFP_ATOMIC
size:bytes

dma_free_coherent(dev,size,cpu_addr,dma_handle);

```
### Streaming DMA mapping
#### Less than HIGHMEM
```
struct device *dev = &my_dev->dev;
dma_addr_t dma_handle;
void *addr = buffer->ptr;
size_t size = buffer->len;

dma_handle = dma_map_single(dev,addr,size,direction);
if(dma_mappiing_error(dev,dma_handle)) {
	//////
	goto error_handling;
}

dma_unmap_single(dev,dma_handle,size,dirction);
```
#### More than HIGHMEM
```
struct device *dev = &my_dev->dev;
dma_addr_t dma_handle;
struct page * page = buffer->page;
size_t size = buffer->len;
unsigned long offset = buffer->offset;

dma_handle = dma_map_page(dev,page,offset,size,direction);
if(dma_mappiing_error(dev,dma_handle)) {
	//////
	goto error_handling;
}

dma_unmap_page(dev,dma_handle,size,dirction);

```

#### DMA Direction
##### flags
```
DMA_BIDIRECTIONAL: each to each
DMA_TO_DEVICE
DMA_FROM_DEVICE
DMA_NONE: for debugging
```

#### DMA Up-data
```
dma_sync_single_for_cpu(dev,dma_handle,size,direction);
dma_sync_single_for_device(dev,dma_handle,size,derection);
```
## writel to where
//from drivers/base/platform.c
```
struct resource * platform_get_resource(struct platform_device *dev,unsigned int type, unsigned int num);
platform_device: linux/platform_device.h

type: linux/ioport.h {IORESOURCE_IO,IORESOURCE_MEM...}
num: 0

/////////////////
root resource

&iomem_resource
&ioport_resource

request_resource(struct resource * root,struct resource * new);

////////////////////
base = (void __iomem *)devm_ioremap_resource(struct device *dev, struct resource *res); 
linux/device.h


offset = 0x80;

writel(val,base + offset);
```



