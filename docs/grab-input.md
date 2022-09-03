# input-grabbing

In Linux one can block a device input based on its input file. 

```c
/**
 * Thread that take a input path (fd) in format: /dev/input/input<N>
 * where <N> is the input that is blocked / grabbed
 * @param[in] path path to dev/input to block
 * @return fd
 */
int block_input(const char* path){
    int fd = open(path, O_RDONLY| O_NONBLOCK, 0);
    if (fd < 0){ fprintf(stderr, "Cant open %s error\n", path);  return -1;}
    if(ioctl(fd, EVIOCGRAB, 1)< 0 ){
        fprintf(stderr, "Can't grab %s\n", path);
        close(fd);
        return -1;
    }
    printf("Blocked[%d] %s \n", fd, path);
    return fd;
}

/**
 * Takes vector of file descriptors and closes them
 * @param[out] block vector
 * @param[in] block number of elements in vector
 */
void unblock_inputs(int * block, const int * block_count){
    int count =0;
    int block_id = -1;
    for(count=0 ; count < *block_count; ++count ){
        block_id = block[count];
        if(block_id > 0){
            close(block_id);
            printf("UnBlocked[%d]\n", block_id);
        }
    }
}
```