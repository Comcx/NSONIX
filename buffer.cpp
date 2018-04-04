

#include "include/sched.h"

/*
* First we define some parameters about buffer
*
*
*/

#define MAX_POOL 10
#define MAX_HASH 4
#define MAX_BLK 1024
#define FREE 0
#define LOCKED 1
#define DATA 2
#define WAIT_WRITE 3


struct buffer_head {

    int dev_id;
    int blk_id;
    int stat;
    int h_code;

    char * data;
    buffer_head * h_next;
    buffer_head * h_prev;
    buffer_head * free_next;
    buffer_head * free_prev;

    buffer_head() : dev_id(0), blk_id(0), stat(FREE), data(0), h_code(0),
                    h_next(NULL), h_prev(NULL), free_next(NULL), free_prev(NULL) {}

    buffer_head(int dev_id, int blk_id, int stat = FREE, char *data = 0) :
        dev_id(dev_id), blk_id(blk_id), stat(stat), data(data), h_code(0) {}
    

};


/*
* Ok, we have created the header of each block(blk) which is one part of buffer_block
* Let us create buffer pool!
*
*/

class buffer_pool {

private:
    buffer_head pool[MAX_POOL];
    buffer_head hash[MAX_HASH];
    string path;
    //const int FREE_MAX;
    const int BLK_SIZE;

public:

    buffer_pool() = default;
    buffer_pool(int dev_id, string path);
    
    buffer_head *get_blk(int blk_id);
    void brelse(buffer_head *blk);

    buffer_head *bread(int blk_id, char *buffer);
    
    /*functions for debug*/
    void show_free(int time);
    void show_hash();
    

};



buffer_pool::buffer_pool(int dev_id, string path): BLK_SIZE(MAX_BLK), path(path)
{
    
    this->pool[0].dev_id = dev_id;
    this->pool[0].free_next = pool + 1;
    this->pool[0].free_prev = pool + MAX_POOL - 1;
                                                            
    buffer_head *tmp = pool + 1;
    for (int i = 1; i < MAX_POOL - 1; i++) {
        
        tmp->free_next = pool + i + 1;
        tmp->free_prev = pool + i - 1;
        tmp->blk_id = i;
        tmp = pool + i + 1;
    }
    pool[MAX_POOL-1].free_next = pool;
    pool[MAX_POOL-1].free_prev = pool + MAX_POOL - 2;
    pool[MAX_POOL-1].blk_id = MAX_POOL - 1;
    
    for (int i = 0; i < MAX_HASH; i++) {
       this->hash[i].h_code = i;
       this->hash[i].h_next = hash + i;
       this->hash[i].h_prev = hash + i;
    }

                                                            
}// end 

buffer_head *buffer_pool::get_blk(int blk_id)
{
    int hash_code = blk_id % MAX_HASH;
    
    buffer_head *current = this->hash[hash_code].h_next;
    while (current != hash + hash_code && current->blk_id != blk_id) {
        current = current->h_next;
    }
    
    /*find no blk in hash list, alloc a blk from free list*/
    if (current == hash + hash_code) {
        /*get new blk from free list*/

        /*if free list is already empty*/
        if (this->pool[0].free_next == this->pool) {
            return NULL;
        }

        buffer_head *new_blk = this->pool[0].free_next;
        this->pool[0].free_next = new_blk->free_next;
        new_blk->free_next->free_prev = this->pool;

        new_blk->blk_id = blk_id;
        new_blk->stat = LOCKED;

        /*insert to hash list*/
        new_blk->h_next = this->hash[hash_code].h_next;
        new_blk->h_prev = this->hash + hash_code;
        this->hash[hash_code].h_next = new_blk;
        new_blk->h_next->h_prev = new_blk;

        return new_blk;
    }
    return current;
}

void buffer_pool::brelse(buffer_head *blk)
{
    /*remove from hash list*/
    blk->stat = FREE;
    blk->h_prev->h_next = blk->h_next;
    blk->h_next->h_prev = blk->h_prev;

    /*insert to the last of free list*/
    blk->free_next = this->pool;
    blk->free_prev = this->pool[0].free_prev;

    blk->free_prev->free_next = blk;
    blk->free_next->free_prev = blk;

}

buffer_head *buffer_pool::bread(int blk_id, char *buffer)
{
    buffer_head *current = get_blk(blk_id);

    std::ifstream file(path);

    for (int i = 0; i < MAX_BLK && !file.eof(); i++) {
        file >> buffer[i];
    }
    current->data = buffer;
    current->stat = DATA;

}




void buffer_pool::show_free(int time)
{
     buffer_head * current = this->pool;
     for (int i = 0; i < time; i++) {
         std::cout << current->blk_id << std::endl;
         current = current->free_next;
     }                                                 
}


void buffer_pool::show_hash() 
{
    for (int i = 0; i < MAX_HASH; i++) {
        
        if (this->hash[i].h_next == hash + i) {
            cout << hash[i].h_code << ":";
            cout << endl;
            continue;
        }
        else {
            cout << hash[i].h_code << ": ";
            buffer_head *current = hash[i].h_next;
            while (current != hash + i) {
                cout << current->blk_id << " ";
                current = current->h_next;
            }

        }
        cout << endl;
    }

}










