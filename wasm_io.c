
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <emscripten/fetch.h>
#include "wasm_io.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

void fetch_data(avio_internal *internal) {
    if (internal->pos + internal->buffer_size >= internal->file_size) {
        return;
    }

    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);

    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;

    char range[128];
    snprintf(range, sizeof(range), "bytes=%llu-%llu", internal->pos + internal->buffer_size, MIN(internal->pos + BUFFER_SIZE - 1, internal->file_size - 1));
    const char* headers[] = {"Range", range, NULL};
    attr.requestHeaders = headers;

    emscripten_fetch_t *fetch = emscripten_fetch(&attr, internal->url);
    if (fetch->status >= 200 && fetch->status < 300) {
        memcpy(internal->buffer + internal->buffer_size, fetch->data, fetch->numBytes);
        internal->buffer_size += fetch->numBytes;
    } else {
        printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
    }

    emscripten_fetch_close(fetch);
}

void head(avio_internal *internal) {
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);

    strcpy(attr.requestMethod, "HEAD");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;

    emscripten_fetch_t *fetch = emscripten_fetch(&attr, internal->url);
    if (fetch->status >= 200 && fetch->status < 300) {
        size_t header_size = emscripten_fetch_get_response_headers_length(fetch);
        char *header_str = malloc(header_size+1);
        emscripten_fetch_get_response_headers(fetch, header_str, header_size+1);
        char **headers = emscripten_fetch_unpack_response_headers(header_str);
        free(header_str);
        for(int i = 0; headers[i] != NULL; i+=2) {
            if (strcasecmp(headers[i], "Content-Length") == 0) {
                internal->file_size = atoll(headers[i+1]);
            }
        }
        emscripten_fetch_free_unpacked_response_headers(headers);
    } else {
        printf("Downloading %s failed, HTTP failure status code: %d.\n", fetch->url, fetch->status);
    }

    emscripten_fetch_close(fetch);
}

int read_packet(void *data, uint8_t *buf, int buf_size) {
    avio_internal *internal = (avio_internal *)data;

    int32_t remaining_size = internal->buffer_size - internal->ptr;
    if (remaining_size < buf_size) {
        memmove(internal->buffer, internal->buffer + internal->ptr, remaining_size);
        internal->ptr = 0;
        internal->buffer_size = remaining_size;
        fetch_data(internal);
    }

    size_t read_size = MIN(buf_size, internal->buffer_size - internal->ptr);
    memcpy(buf, internal->buffer + internal->ptr, read_size);
    internal->ptr += read_size;
    internal->pos += read_size;
    return read_size;
}

int64_t seek_packet(void *data, int64_t offset, int whence) {
    avio_internal *internal = (avio_internal *)data;

    switch (whence) {
        case SEEK_SET:
            if (offset > internal->file_size) {
                return -1;
            }

            internal->pos = offset;
            internal->ptr = 0;
            internal->buffer_size = 0;
            return internal->pos;
        case SEEK_CUR:
        {
            int64_t new_pos = (int64_t) internal->pos + offset;
            if (new_pos > internal->file_size || new_pos < 0) {
                return -1;
            }

            internal->pos = new_pos;
            int32_t new_ptr = (int32_t) internal->ptr + offset;
            if (new_ptr > internal->buffer_size || new_ptr < 0) {
                internal->ptr = 0;
                internal->buffer_size = 0;
            } else {
                internal->ptr += offset;
            }
            return internal->pos;
        }
        case SEEK_END:
            if (offset > 0) {
                return -1;
            }

            internal->pos = internal->file_size + offset;
            return internal->pos;
        default:
            return -1;
    }
}
