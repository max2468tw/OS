#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define FRAME_SIZE 256        // size of the frame
#define TOTAL_NUMBER_OF_FRAMES 256  // total number of frames in physical memory
#define ADDRESS_MASK  0xFFFF  //mask all but the address
#define OFFSET_MASK  0xFF //mask all but the offset
#define TLB_SIZE 16       // size of the TLB
#define PAGE_TABLE_SIZE 256  // size of the page table
#define BUFFER_SIZE 10	// number of characters to read for each line from input file
#define CHUNK 256 // number of bytes to read

int PageTable[PAGE_TABLE_SIZE];  // array to hold the frame numbers in the page table

int TLBPageNumber[TLB_SIZE];  // array to hold the page numbers in the TLB
int TLBFrameNumber[TLB_SIZE]; // array to hold the frame numbers in the TLB

int PhysicalMemory[TOTAL_NUMBER_OF_FRAMES][FRAME_SIZE]; // physical memory 2D array

int pageFaults = 0;   // counter to track page faults
int TLBHits = 0;      // counter to track TLB hits
int AvailableFrame = 0;  // counter to track the first available frame
int NumOfTLBEntries = 0;   // counter to track the number of entries in the TLB

// input file and backing store
FILE    *address_file;
FILE    *backing_store;
FILE 	*pFile;
// how we store reads from input file
char    address[BUFFER_SIZE];
int     logical_address;

// the buffer containing reads from backing store
signed char     buffer[CHUNK];

// the value of the byte (signed char) in memory
signed char     value;

// headers for the functions used below
void init(void);
void getPage(int address);
void readFromStore(int pageNumber);
void insertIntoTLB(int pageNumber, int frameNumber);

// initialize the PageTable and TLB
void init(void)
{
	for(int i = 0; i < PAGE_TABLE_SIZE; i++)
		PageTable[i] = -1;
	for(int i = 0; i < TLB_SIZE; i++)
	{
		TLBPageNumber[i] = -1;
		TLBFrameNumber[i] = -1;
	}
}
// function to take the logical address and obtain the physical address and value stored at that address
void getPage(int logical_address){

    // obtain the page number and offset from the logical address
    int pageNumber = ((logical_address & ADDRESS_MASK)>>8);
    int offset = (logical_address & OFFSET_MASK);

    // first try to get page from TLB
    int frameNumber = -1; // initialized to -1 to tell if it's valid in the conditionals below

    int i;  // look through TLB for a match
    for(i = 0; i < TLB_SIZE; i++){
        if(TLBPageNumber[i] == pageNumber){   // if the TLB index is equal to the page number
            frameNumber = TLBFrameNumber[i];  // then the frame number is extracted
            TLBHits++;                // and the TLBHit counter is incremented
        }
    }

    // if the frameNumber was not found
    if(frameNumber == -1){
        if(PageTable[pageNumber] != -1){         // if the page is found in those contents
            frameNumber = PageTable[pageNumber]; // extract the frameNumber from PageTable
        }
        if(frameNumber == -1){                     // if the page is not found in those contents
            readFromStore(pageNumber);             // page fault, call to readFromStore to get the frame into physical memory and the page table
            pageFaults++;                          // increment the number of page faults
            frameNumber = AvailableFrame - 1;  // and set the frameNumber to the current AvailableFrame index
		}
    }

    insertIntoTLB(pageNumber, frameNumber);  // call to function to insert the page number and frame number into the TLB
    value = PhysicalMemory[frameNumber][offset];  // frame number and offset used to get the signed value stored at that address
    // output the virtual address, physical address and value of the signed char to the console
    //printf("Virtual address: %d Physical address: %d Value: %d\n", logical_address, (frameNumber << 8) | offset, value);
    printf("%d\t%d\n", (frameNumber << 8) | offset, value);
	fprintf(pFile, "%d %d\n", (frameNumber << 8) | offset, value);
}

// function to insert a page number and frame number into the TLB with a FIFO replacement
void insertIntoTLB(int pageNumber, int frameNumber){
    int i;  // if it's already in the TLB, break
    for(i = 0; i < NumOfTLBEntries; i++){
        if(TLBPageNumber[i] == pageNumber){
            break;
        }
    }

    // if the number of entries is equal to the index
    if(i == NumOfTLBEntries){
        if(NumOfTLBEntries < TLB_SIZE){  // and the TLB still has room in it
            TLBPageNumber[NumOfTLBEntries] = pageNumber;    // insert the page and frame on the end
            TLBFrameNumber[NumOfTLBEntries] = frameNumber;
        }
        else{                                            // otherwise move everything over
            for(i = 0; i < TLB_SIZE - 1; i++){
                TLBPageNumber[i] = TLBPageNumber[i + 1];
                TLBFrameNumber[i] = TLBFrameNumber[i + 1];
            }
            TLBPageNumber[NumOfTLBEntries-1] = pageNumber;  // and insert the page and frame on the end
            TLBFrameNumber[NumOfTLBEntries-1] = frameNumber;
        }
		if(NumOfTLBEntries < TLB_SIZE){                    // if there is still room in the arrays, increment the number of entries
			NumOfTLBEntries++;
		}
    }
    // if the index is not equal to the number of entries
    else{
        for(i = i; i < NumOfTLBEntries - 1; i++){      // iterate through up to one less than the number of entries
            TLBPageNumber[i] = TLBPageNumber[i + 1];      // move everything over in the arrays
            TLBFrameNumber[i] = TLBFrameNumber[i + 1];
        }

        TLBPageNumber[NumOfTLBEntries-1] = pageNumber;
        TLBFrameNumber[NumOfTLBEntries-1] = frameNumber;
    }
}

// function to read from the backing store and bring the frame into physical memory and the page table
void readFromStore(int pageNumber){
    // first seek to byte CHUNK in the backing store
    // SEEK_SET in fseek() seeks from the beginning of the file
    if (fseek(backing_store, pageNumber * CHUNK, SEEK_SET) != 0) {
        fprintf(stderr, "Error seeking in backing store\n");
    }

    // now read CHUNK bytes from the backing store to the buffer
    if (fread(buffer, sizeof(signed char), CHUNK, backing_store) == 0) {
        fprintf(stderr, "Error reading from backing store\n");
    }

    // load the bits into the first available frame in the physical memory 2D array
    int i;
    for(i = 0; i < CHUNK; i++){
        PhysicalMemory[AvailableFrame][i] = buffer[i];
    }

    // and then load the frame number into the page table in the first available frame
    PageTable[pageNumber] = AvailableFrame;

    // increment the counters that track the next available frames
    AvailableFrame++;
}

// main opens necessary files and calls on getPage for every entry in the addresses file
int main(int argc, char *argv[])
{
    // perform basic error checking
    if (argc != 3) {
        fprintf(stderr,"Usage: ./a.out [input file]\n");
        return -1;
    }

    // open the file containing the backing store
    backing_store = fopen(argv[1], "rb");

    if (backing_store == NULL) {
        fprintf(stderr, "Error opening BACKING_STORE.bin %s\n",argv[1]);
        return -1;
    }

    // open the file containing the logical addresses
    address_file = fopen(argv[2], "r");

    if (address_file == NULL) {
        fprintf(stderr, "Error opening addresses.txt %s\n",argv[2]);
        return -1;
    }

	//initialize the PageTable and TLB
    init();
	
    pFile = fopen( "results.txt","w" );

    if (pFile == NULL) {
        fprintf(stderr, "Error opening results.txt\n");
        return -1;
    }

    //int numberOfTranslatedAddresses = 0;
    // read through the input file and output each logical address
    while ( fgets(address, BUFFER_SIZE, address_file) != NULL) {
        logical_address = atoi(address);

        // get the physical address and value stored at that address
        getPage(logical_address);
        //numberOfTranslatedAddresses++;  // increment the number of translated addresses
    }

	// calculate and print out the stats
    //printf("Number of translated addresses = %d\n", numberOfTranslatedAddresses);
    //double pfRate = pageFaults / (double)numberOfTranslatedAddresses;
    //double TLBRate = TLBHits / (double)numberOfTranslatedAddresses;

    printf("TLB hits: %d\n", TLBHits);
	fprintf(pFile, "TLB hits: %d\n", TLBHits);
    //printf("TLB Hit Rate = %.3f\n", TLBRate);
    printf("Page faults: %d\n", pageFaults);
	fprintf(pFile, "Page faults: %d\n", pageFaults);
    //printf("Page Fault Rate = %.3f\n",pfRate);


    // close the input file and backing store
    fclose(address_file);
    fclose(backing_store);
	fclose(pFile);
    return 0;
}
