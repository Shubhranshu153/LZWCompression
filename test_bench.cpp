
#include <iostream>
#include <cstdint>
#include <string.h>
#include <vector>

#define FILEIN "./read_data"

#define MAX_INPUT_SIZE 500*1024*1024
#define MAX_OUTPUT_SIZE 500*1024*1024
#define CHUNK_SIZE 8*1024

using namespace std;

typedef struct chunks{
     unsigned char chunk[CHUNK_SIZE];
}chunks; 

unsigned int load_data(unsigned char *Data)
{
    //Opening a file in byte read mode
    FILE *data_file= fopen(FILEIN,"rb"); 
    if(data_file==NULL)
    {
        cout<<"err1"<<endl;
        return 0;
    }
    
    //reading file contents 
    long long int bytes_read = fread(Data,sizeof(unsigned char),MAX_INPUT_SIZE,data_file);
    
    //close the file
    if(fclose(data_file)!=0)
    {   
        cout<<"err2"<<endl;
        return 0;
    }

    return bytes_read;
}

vector<chunks> chunking( unsigned char *input, long long int input_size)
{
    
    long long int chunk_start=0;
    vector <chunks> input_chunks;
    while(chunk_start<input_size)
    {
        if(chunk_start+CHUNK_SIZE <=input_size)
        {
            chunks *chunk_struct=(chunks*)malloc(sizeof(chunks));
            memcpy(chunk_struct->chunk,input+chunk_start,CHUNK_SIZE); 
            input_chunks.push_back(*chunk_struct);
        }
        else
        {
            int size=input_size-chunk_start;
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
    unsigned char * Input=(unsigned char*)malloc(MAX_INPUT_SIZE);
    unsigned char * Output=(unsigned char*)malloc(MAX_OUTPUT_SIZE);

  
    long long int bytes_read =load_data(Input);
    
    if(bytes_read==0)
    {
        cout<<"file_not_read"<<endl;
    }
    
    vector<chunks> input_chunks=chunking(Input,bytes_read);
    cout<<input_chunks.size()<<endl;

}