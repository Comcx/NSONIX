
#include "sched.h"




#define MAX_POOL 10
#define MAX_HASH 4
#define MAX_BLK 1024



struct buffer_head {

    int dev_id;
    int blk_id;
    int h_code;

    int locked;
    int wait_write;
    int contain_data;

    char * data;
    buffer_head * h_next;
    buffer_head * h_prev;
    buffer_head * free_next;
    buffer_head * free_prev;

    buffer_head() : dev_id(0), blk_id(0), locked(FALSE), wait_write(FALSE), contain_data(FALSE),
                    data(0), h_code(0),
                    h_next(NULL), h_prev(NULL), free_next(NULL), free_prev(NULL) {}

    buffer_head(int dev_id, int blk_id, int locked = FALSE, char *data = 0) :
        dev_id(dev_id), blk_id(blk_id), locked(locked), wait_write(FALSE), contain_data(FALSE), data(data), h_code(0) {}
    

};














