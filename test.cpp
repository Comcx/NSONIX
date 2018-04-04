
#include <stdio.h>

#include "buffer.cpp"

const int test = 0;


void show_pool(buffer_pool *pool)
{
    pool->show_free(10);
    cout<< endl;
    pool->show_hash();
}



int main(int argc, char *argv[])
{
    /*
    struct_free_buffer(0, buffer_heads);
    
    struct buffer_head *head = &buffer_heads[0];
    int i = 0;
    for (; i < 20; i++) {
        printf("%d\n", head->block_id);
        head = head->free_next;
    }
    */


    buffer_pool pool(0, "test.txt");
    pool.show_free(20);
    cout << endl;
    pool.show_hash();
    
    cout << "\ntest" << endl;
    buffer_head *blk = pool.get_blk(4);
    blk = pool.get_blk(7);
    blk = pool.get_blk(9);
    blk = pool.get_blk(5);
    show_pool(&pool);

    cout << "\ntest1" << endl;
    pool.brelse(blk);
    show_pool(&pool);

    char buffer[1024];
    blk = pool.bread(1, buffer);

    for (int i = 0; i< 1024; i++) {
        cout << buffer[i];
    }
    
    cout << "\ntest2" << endl;
    show_pool(&pool);

    return 0;
}









