#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define PAGESIZE 4096
#define PAGESIZE_HEX 1000

typedef unsigned char BYTE;


void *startofheap = NULL;

typedef struct structinfo
{
    int size;
    int status;
    BYTE *next, *prev;
}structinfo;


void analyze() 
{ 
    printf("\n--------------------------------------------------------------\n"); 
    if(!startofheap) 
    { 
        printf("no heap\n"); 
        printf("Program break on %p\n", sbrk(0));
        return; 
    } 

    structinfo* ch = (structinfo*)startofheap; 

    for (int no=0; ch; ch = (structinfo*)ch->next,no++) 
    { 
        printf("%d | current addr: %p |", no, ch); 
        printf("size: %d | ", ch->size); 
        printf("info: %d | ", ch->status); 
        printf("next: %p | ", ch->next); 
        printf("prev: %p", ch->prev); 
        printf("      \n"); 
    } 

    printf("program break on address: %p\n\n",sbrk(0)); 
} 


// chunkhead* get_last_chunk() //you can change it when you aim for performance 
//  {   
//  if(!startofheap) //I have a global void *startofheap = NULL; 
//   return NULL;   
//  chunkhead* ch = (chunkhead*)startofheap; 
//  for (; ch->next; ch = (chunkhead*)ch->next); 
//  return ch; 
//  } 



void myfree(BYTE *address)
{
    BYTE *target_address = address - sizeof(structinfo);
    structinfo *chunk = (structinfo*) target_address;
    if(chunk == NULL)
    {
        return;
    }
    //free the chunk
    chunk->status = 0;

    structinfo *prev_chunk = (structinfo*)chunk->prev; // null?
    structinfo *next_chunk = (structinfo*)chunk->next; // null?

    //base case
    if(next_chunk == NULL && prev_chunk == startofheap && prev_chunk->status == 0)
    {
        prev_chunk->size = chunk->size + prev_chunk->size;
        startofheap = prev_chunk;
        sbrk(-prev_chunk->size);
        startofheap = NULL;
        chunk = NULL;
        return;
    }
    // merge with the previous
    if(prev_chunk != NULL && prev_chunk->status == 0) {
        //merge
        prev_chunk->size = prev_chunk->size + chunk->size;
        prev_chunk->next = chunk->next;
        if(next_chunk)
            next_chunk->prev = (BYTE*)prev_chunk;
        else
        {
            //do nothing
        }
        chunk = NULL;
        

    }


    //merge with the next
    if(next_chunk == NULL) {
        //move the program break
        prev_chunk->next = NULL;
        sbrk(-prev_chunk->size);
    }
    else if(next_chunk->status == 0) {
        //merge
        chunk->size = chunk->size + next_chunk->size;
        chunk->next = next_chunk->next;
        if(next_chunk)
            next_chunk = NULL;
    }
    

    


    // Base case where after removing, the start of heap will be NUll
    //if((structinfo*)startofheap->next == NULL)
    
    // This will need to be modified for your own implementation!
    // BYTE *target_address = address - sizeof(structinfo);
    // structinfo *chunk = (structinfo*) target_address;
    // startofheap = NULL;
    // sbrk(-chunk->size); // Not always required, only when we need to move the program break back!

}


BYTE* mymalloc(int demand)
{
    int counter = 0;
      // calculating bytes needed to allocate (accounting for structinfo and PAGESIZE)
    int demand_bytes = demand + sizeof(structinfo);
    int page_required = demand_bytes / PAGESIZE + 1;
    int real_demand = page_required * PAGESIZE;

    // no memory allocated yet
    if (startofheap == NULL)
    {
        structinfo *chunk = (structinfo*) sbrk(sizeof(structinfo));
        chunk->size = real_demand;
        chunk->status = 1;
        startofheap = chunk;
        sbrk(real_demand - sizeof(structinfo));

        return (BYTE*) chunk + sizeof(structinfo);
    }

    //memory has been previously allocated (must find free memory to allocate)
    // traverse the chunks looking for best fit - is the status 0, size < real_demand
    structinfo *current = (structinfo*) startofheap;
    structinfo *previous = NULL;
    structinfo *bestfit = NULL;
   // bestfit->size = INT_MAX;
    while(current != NULL) {
        counter++;

        if(current->status == 0 && current->size >= real_demand) {
            // potentially the best-fit
            if(bestfit == NULL || bestfit->size > current->size) {
                //should point to the best fit
                bestfit = current;
            }
        }
        previous = current;
        current = (structinfo*) current->next;
        
    }
    // bestfit the chunk you want use

    //means no bestfits available so need to append the block to the end
    if(bestfit == NULL) {
        //create a new chunk
        structinfo *newChunk = (structinfo*) sbrk(sizeof(structinfo));
        //newChunk->prev = current;
        newChunk->size = real_demand;
        newChunk->status = 1;
        current = newChunk;
        previous->next  = (BYTE*)newChunk;
        newChunk->prev = (BYTE*)previous;
        sbrk(real_demand - sizeof(structinfo));
        return (BYTE*) newChunk + sizeof(structinfo);
    }
    else {
        //does real_demand fit perfectly into chunk?
        //splitting - only if the "left over" is bigger than a pagesize
        if(bestfit->size - real_demand >= PAGESIZE)
        {

            BYTE* secondChunkAddress = (BYTE*)bestfit + (page_required * PAGESIZE);
            structinfo* secondChunk = (structinfo*)secondChunkAddress;//might need to typecast it

            secondChunk->size = bestfit->size - real_demand;
            bestfit->size = real_demand;
            secondChunk->prev = (BYTE*)bestfit;
            secondChunk->next = bestfit->next;

            bestfit->status = 1;
            secondChunk->status = 0;
            bestfit->next = (BYTE*)secondChunk;
            return (BYTE*) bestfit + sizeof(structinfo);
        }
        else
        {
            bestfit->status = 1;
            return (BYTE*) bestfit + sizeof(structinfo);
        }
    }


    return NULL;

}



int main()
{

    BYTE*a[100];
    analyze(); 
    for(int i=0;i<100;i++) 
        a[i]= mymalloc(1000);
    for(int i=0;i<90;i++) 
        myfree(a[i]);   
    analyze(); //50% of points if this is correct 
    myfree(a[95]); 
    a[95] = mymalloc(1000); 
    analyze();//25% points, this new chunk should fill the smaller free one 
    //(best fit) 
    for(int i=90;i<100;i++) 
        myfree(a[i]); 
    analyze();// 25% should be an empty heap now with the start address 
    //from the program start 
    return 0;
}


