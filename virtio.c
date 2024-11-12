#include "virtio.h"

VIRTIO virtio;

void initVIRTIO(u8 disk_image[]){
    virtio.queue_notify = 9999;
}

Result load_virtio(u64 addr, u64 size){
    Result ret;
    ret.exception = NullException;
    if(size != 32) {
        ret.exception = LoadAccessFault;
        return ret;
    }
    u64 val;
    switch(addr){
        case VIRTIO_MAGIC:
            val = 0x74726976;
            break;
        case VIRTIO_VERSION:
            val = 0x1;
            break;
        case VIRTIO_DEVICE_ID:
            val = 0x2;
            break;
        case VIRTIO_VENDOR_ID:
            val = 0x554d4551;
            break;    
        case VIRTIO_DEVICE_FEATURES:
            val = 0;
            break;
        case VIRTIO_DRIVER_FEATURES:
            val = virtio.driver_features;
            break;
        case VIRTIO_QUEUE_NUM_MAX:
            val = 8;
            break;
        case VIRTIO_QUEUE_PFN:
            val = virtio.queue_pfn;
            break;
        case VIRTIO_STATUS:
            val = virtio.status;
            break;
        default:
            val = 0;        
    }
    ret.value = val;
    return ret;
}

Result store_virtio(u64 addr, u64 size, u64 val){
    Result ret;
    ret.exception = NullException;
    if(size != 32){
        ret.exception = StoreAMOAccessFault;
        return ret;
    }
    val = (u32)val;
    switch(addr){
        case VIRTIO_DEVICE_FEATURES:
            virtio.driver_features = val;
            break;
        case VIRTIO_GUEST_PAGE_SIZE:
            virtio.page_size = val;
            break;
        case VIRTIO_QUEUE_SEL:
            virtio.queue_sel = val;
            break;
        case VIRTIO_QUEUE_NUM:
            virtio.queue_num = val;
            break;    
        case VIRTIO_QUEUE_PFN:
            virtio.queue_pfn = val;
            break;
        case VIRTIO_QUEUE_NOTIFY:
            virtio.queue_notify = val;
            break;
        case VIRTIO_STATUS:
            virtio.status = val;
            break;
        default:
            break;        
    }
    return ret;
}

bool is_virtio_interrupting(){
    if(virtio.queue_notify != 9999){
        virtio.queue_notify = 9999;
        return true;
    }
    return false;
}

u64 get_new_id(){
    virtio.id = virtio.id+1;
    return virtio.id;
}

u64 desc_addr(){
    return (u64)virtio.queue_pfn * (u64)virtio.page_size;
}

u64 read_disk(u64 addr){
    return (u64)virtio.disk[addr];
}

void write_disk(u64 addr, u64 val){
    virtio.disk[addr] = (u8)val;
}

void virtio_disk_access(){
    u64 d_addr = desc_addr();
    u64 a_addr = desc_addr()+0x40;
    u64 u_addr = desc_addr()+4096;
    
    u16 offset = load(a_addr+1, 16);
    u16 index = load(a_addr+(offset%DESC_NUM)+2, 16);

    u64 d_addr0 = d_addr + VRING_DESC_SIZE*index;
    u64 addr0 = load(d_addr0, 64);
    u16 next0 = load(d_addr0+14, 16);

    u64 d_addr1 = d_addr + VRING_DESC_SIZE*next0;
    u64 addr1 = load(d_addr1, 64);
    u32 len1 = load(d_addr1+8, 32);
    u16 flags1 = load(d_addr1+12, 16);

    u64 blk_sector = load(addr0+8, 64);
    if((flags1&2) == 0){
        for(int i = 0;i < len1;i++){
            write_disk(blk_sector*512+i, load(addr1+i, 8));
        }
    }else{
        for(int i = 0;i < len1;i++){
            u64 data = read_disk(blk_sector*512+i);
            store(addr1+i, 8, data);
        }
    }
    store(u_addr+2, 16, get_new_id()%8);
}