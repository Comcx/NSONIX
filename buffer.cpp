

#include "include/sched.h"
#include "include/fs.h"


/*
* Ok, now that we have created the header of each block(blk) which is one part of buffer_block
* Let us create buffer pool!
*
*/

class buffer_pool {

private:
    buffer_head pool[MAX_POOL];
    buffer_head hash[MAX_HASH];
    string path;

public:

    buffer_pool() = default;
    buffer_pool(int dev_id, string path);
    
    buffer_head *get_blk(int blk_id);
    void brelse(buffer_head *blk);

    buffer_head *bread(int blk_id, char *buffer);
    void bwrite(buffer_head *blk);
    
    /*functions for debug*/
    void show_free(int time);
    void show_hash();
    

};



buffer_pool::buffer_pool(int dev_id, string path): path(path)
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
        /*get the new blk from free list!*/
        buffer_head *new_blk = this->pool[0].free_next;
        this->pool[0].free_next = new_blk->free_next;
        new_blk->free_next->free_prev = this->pool;

        /*if new blk is wait_write, write it first!*/
        if (new_blk->wait_write == TRUE) {
            this->bwrite(new_blk);
            new_blk->wait_write = FALSE;
        }

        new_blk->blk_id = blk_id;
        new_blk->locked = TRUE;

        /*insert to hash list*/
        new_blk->h_next = this->hash[hash_code].h_next;
        new_blk->h_prev = this->hash + hash_code;
        this->hash[hash_code].h_next = new_blk;
        new_blk->h_next->h_prev = new_blk;

        return new_blk;
    }
    /*return blk in hash directly*/
    return current;
}

void buffer_pool::brelse(buffer_head *blk)
{
    /*remove from hash list*/
    if (blk->contain_data == TRUE) {
        blk->wait_write = TRUE;
        blk->locked = FALSE;
    }
    else {
        blk->locked = FALSE;
    }
    
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
    /*
    for (int i = 0; i < MAX_BLK && !file.eof(); i++) {
        file >> buffer[i];
    }*/
    file.read(buffer, MAX_BLK);
    file.close();

    current->data = buffer;
    current->contain_data = TRUE;

    return current;
}

void buffer_pool::bwrite(buffer_head *blk)
{
    std::ofstream file(path, std::ios::in);
    file << string(blk->data);

}



void buffer_pool::show_free(int time)
{
     buffer_head * current = this->pool;
     for (int i = 0; i < time; i++) {
         print << current->blk_id << " ";
         current = current->free_next;
     }                                                 
}


void buffer_pool::show_hash() 
{
    for (int i = 0; i < MAX_HASH; i++) {
        
        if (this->hash[i].h_next == hash + i) {
            print << hash[i].h_code << ":";
            print << endl;
            continue;
        }
        else {
            cout << hash[i].h_code << ": ";
            buffer_head *current = hash[i].h_next;
            while (current != hash + i) {
                print << current->blk_id << " ";
                current = current->h_next;
            }

        }
        print << endl;
    }

}










