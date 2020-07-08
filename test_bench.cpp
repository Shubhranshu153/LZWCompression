
#include <iostream>
#include <cstdint>
#include <string.h>
#include <vector>
#include "LZW.h"

#define FILEIN "./read_data"


using namespace std;



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
    unsigned char * Input=(unsigned char*)malloc(MAX_INPUT_SIZE);
    unsigned char * Output=(unsigned char*)malloc(MAX_OUTPUT_SIZE);

  
    long long int bytes_read =load_data(Input);
    remove("./output_data");
    cout<<"bytes read "<<bytes_read<<endl;
    
    if(bytes_read==0)
    {
        cout<<"file_not_read"<<endl;
    }
    
    vector<chunks> input_chunks(chunking(Input,bytes_read));
    vector<vector< unsigned short int >> compressed_output;
    cout<<"size"<<input_chunks.size()<<endl;
    
    int end_chunk_size=bytes_read%(CHUNK_SIZE);
    
    int chunk_size=CHUNK_SIZE;
    for(int i=0;i<input_chunks.size();i++)
    {
        if(i==input_chunks.size()-1)
            chunk_size=end_chunk_size;
       cout<<chunk_size<<"  ";
       compressed_output.push_back(LZW_compress(input_chunks[i].chunk,chunk_size));
    }
    
    int calc_compressed_size=0;
    int count=0;
    for(auto i:compressed_output)
    {
        calc_compressed_size+=sizeof(unsigned short int)*i.size();
      
    }
    cout<<calc_compressed_size/1024.0<<endl;

    for(auto i:compressed_output)
    {
        LZW_decompress(i);
    }
}