# Development Guidelines<a name="EN-US_TOPIC_0000001152860497"></a>

-   [Sample Code](#section1034515054620)

When porting LittleFS to a new hardware device, you need to declare  **lfs\_config**:

```
const struct lfs_config cfg = {
    // block device operations
    .read  = user_provided_block_device_read,
    .prog  = user_provided_block_device_prog,
    .erase = user_provided_block_device_erase,
    .sync  = user_provided_block_device_sync,

    // block device configuration
    .read_size = 16,
    .prog_size = 16,
    .block_size = 4096,
    .block_count = 128,
    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = 500,
};
```

**.read**,  **.prog**,  **.erase**, and  **.sync**  correspond to the read, write, erase, and synchronization APIs at the bottom layer of the hardware platform, respectively.

**read\_size**  indicates the number of bytes read each time. You can set it to a value greater than the physical read unit to improve performance. This value determines the size of the read cache. However, if the value is too large, more memory is consumed.

**prog\_size**  indicates the number of bytes written each time. You can set it to a value greater than the physical write unit to improve performance. This value determines the size of the write cache and must be an integral multiple of  **read\_size**. However, if the value is too large, more memory is consumed.

**block\_size**: indicates the number of bytes in each erase block. The value can be greater than that of the physical erase unit. However, a smaller value is recommended because each file occupies at least one block. The value must be an integral multiple of  **prog\_size**.

**block\_count**  indicates the number of blocks that can be erased, which depends on the capacity of the block device and the size of the block to be erased \(**block\_size**\).

## Sample Code<a name="section1034515054620"></a>

The sample code is as follows:

```
int main(void) {
    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&lfs, &cfg);
        lfs_mount(&lfs, &cfg);
    }

    // read current count
    uint32_t boot_count = 0;
    lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs, &file);
    lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs, &file);

    // release any resources we were using
    lfs_unmount(&lfs);

    // print the boot count
    printf("boot_count: %d\n", boot_count);
}
```

