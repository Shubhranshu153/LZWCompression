
#include <iostream>
#include <cstdint>
#include <string.h>
#include <vector>
#include "LZW.h"
#include <list>
#include <pthread.h>
#include <chrono>
#define FILEIN "./input_data"


using namespace std;




unsigned int load_data(char *Data)
{
    //Opening a file in byte read mode
    FILE *data_file= fopen(FILEIN,"r"); 
    if(data_file==NULL)
    {
        cout<<"err1"<<endl;
        return 0;
    }
    
    //reading file contents 
    long long int bytes_read = fread(Data,sizeof(char),MAX_INPUT_SIZE,data_file);
    
    //close the file
    if(fclose(data_file)!=0)
    {   
        cout<<"err2"<<endl;
        return 0;
    }

    return bytes_read;
}



vector<chunks> chunking( char *input, long long int input_size)
{
    
    long long int chunk_start=0;
    vector <chunks> input_chunks;
    while(chunk_start<input_size)
    {
        if(chunk_start+CHUNK_SIZE <=input_size)
        {
            chunks *chunk_struct=(chunks*)malloc(sizeof(chunks));
            memcpy((void *)chunk_struct->chunk,(const void*)(input+chunk_start),CHUNK_SIZE); 
            input_chunks.push_back(*chunk_struct);
        }
        else
        {
            int size=input_size-chunk_start+1;
            chunks *chunk_struct=(chunks*)malloc(sizeof(chunks));
            memcpy(chunk_struct->chunk,input+chunk_start,size); 
            input_chunks.push_back(*chunk_struct);
        }
        chunk_start=chunk_start+CHUNK_SIZE;
        
        
    }
    return input_chunks;

}

int main()
{

    char * Input=(char*)malloc(MAX_INPUT_SIZE);
    
    int thread_number=1;
    cout<<"Enter desired number of threads. Default is 1: ";
    cin>>thread_number;


    // Load data
    long long int bytes_read =load_data(Input);

    // Display bytes read
    cout<<"Input File Size "<<bytes_read/(1024.0*1024.0)<<" MB"<<endl;
    
    //remove exiting output file
    remove("./output_data");

    if(bytes_read==0)
    {
        cout<<"file_not_read"<<endl;
    }

    auto start = std::chrono::steady_clock::now();
    //chunk the data into manageable units for LZW and multithreading    
    vector<chunks> input_chunks(chunking(Input,bytes_read));
  
   
    
    // Special case for end chunk
    int end_chunk_size=bytes_read%(CHUNK_SIZE);
    int chunk_size=CHUNK_SIZE;
   
    // placeholder for thread IDs
    pthread_t pids[thread_number];


    //We create threads and start compressing chunk by chunk
    for(int i=0;i<input_chunks.size();)
    {
        int threads_to_join=0;
        for(int j=0;j<thread_number;j++)
        {
            //no more chunks to process
            if(i==input_chunks.size())
                break;

            if(i==input_chunks.size()-1)
                chunk_size=end_chunk_size;
            
            // handles the data structure to be passed into the thread
            int chunk_number=i;
            compress_chunks *compress_chunk=(compress_chunks*)malloc(sizeof(compress_chunks));
            compress_chunk->input_chunk=input_chunks[i].chunk;
            compress_chunk->chunk_number=chunk_number;
            compress_chunk->chunk_size=chunk_size;
            
            //spawn thread
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_create(&pids[j],&attr,LZW_compress,(void*)compress_chunk);
            
            
            threads_to_join++;
            i++;

        }

        //join the threads. Basically waits for the thread to complete
        for(int j=0;j<threads_to_join;j++)
        {
            pthread_join(pids[j],NULL);
        }
    
    
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
   
    
    int calc_compressed_size=0;
    int count=0;
    
    // Calculate the compressed size and compression ratio
    for(auto i:compressed_output_collection)
    {
        calc_compressed_size+=sizeof(unsigned short int)*(i.size()-1);  
    }

    cout<<"Compressed size "<<calc_compressed_size/(1024.0*1024)<<" MB"<<endl;
    cout<<"Compression ratio "<<float(bytes_read)/(float)calc_compressed_size<<endl;
    std::cout << "Compression time in ms: " << elapsed_seconds.count()*1000 << "ms\n";
    std::unordered_map<int,vector<unsigned short int>> compress_ordered;
    
    //organize serially all the chunks
    for(auto i:compressed_output_collection)
    {
        int chunk_num=i.front();
        i.pop_front();
        vector <unsigned short int> temp(i.begin(),i.end());
        compress_ordered.insert(std::pair<int,vector<unsigned short int>>(chunk_num,temp));
       
    }

    // Decompression for testing
    for(int i=0;i<compress_ordered.size();i++)
        LZW_decompress(compress_ordered[i]);

}