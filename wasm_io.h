#include <stdint.h>

#define BUFFER_SIZE 1048576

typedef struct {
    uint64_t pos;  // current position in the file
    uint32_t ptr;  // Poiter to the current position in the buffer
    uint32_t buffer_size;  // Current size of the buffer
    uint64_t file_size;  // Size of the file
    uint8_t buffer[BUFFER_SIZE];
    char *url;
} avio_internal;


int read_packet(void *data, uint8_t *buf, int buf_size);

int64_t seek_packet(void *data, int64_t offset, int whence);

void head(avio_internal *internal);
