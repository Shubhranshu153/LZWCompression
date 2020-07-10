

#include <vector>
#include <string>
#include <unordered_map>
#include <map>

#define MAX_INPUT_SIZE 500*1024*1024
#define MAX_OUTPUT_SIZE 500*1024*1024
#define CHUNK_SIZE 8*1024


typedef struct chunks{
     char chunk[CHUNK_SIZE];
}chunks; 

using namespace std;


std::vector< unsigned short int > LZW_compress(char* input_chunk,int chunk_size) {

		std::unordered_map< string,int > lzw_table;
        std::vector< unsigned short int > compressed_output;
		// Clear the table from previous LZW runs.
		lzw_table.clear();

	
		
		int prev_match = 0;
		int match = 0;
		int inputByte = 0;

		// Load the LZW table with basic characters.
		for (int i = 0; i < 256; i++) {
			string s;
			s.push_back(char(i));
			lzw_table.insert(std::pair<string,int>(s,i));
		}
		string s;

		// Compress until finished with the data.
		while (inputByte < chunk_size) {

		// 	// Get a new character.
		
			s.push_back(input_chunk[inputByte]);
			
			

			// Check against the LZW table to see if it's there.
			if(lzw_table.find(s)!=lzw_table.end())
			{
				match = lzw_table[s];
				prev_match = match;
			
				// If no more input is left, send out what we have.
			}
			else
			{
				// Add the unique string to the table.
				lzw_table.insert(std::pair<string,int>(s,lzw_table.size()));
			
				compressed_output.push_back(prev_match);

				// Retain the most recent character.
				s="";
				s=s+input_chunk[inputByte];
				
			
				
				prev_match=lzw_table[s];
				
				// If no more input is left, send out what we have.		
			}
			inputByte++;
		}

		compressed_output.push_back(prev_match);
		
	
		// Return the size of the compressed data + the 4-byte header.
		return compressed_output;
	
}


void LZW_decompress(std::vector< unsigned short int > compressed_output) {

		
		// Clear the table from previous LZW runs.
		std::unordered_map< int,string > lzw_table;
		
		lzw_table.clear();

		if(compressed_output.size()==0)
			return;
		
		int prev_match = 0;
		int match = 0;
		int inputByte = 0;

		// Load the LZW table with basic characters.
		for (int i = 0; i < 256; i++) {
			string s;
			s.push_back((char)i);
			lzw_table.insert(std::pair<int,string>(i,s));
		}
		int old = compressed_output[0], n; 
        string s(lzw_table[old]); 
		char c;
		int count=256;

		FILE *file_out=fopen("./output_data","a");
		fwrite((void*)(&compressed_output[0]),sizeof(char),1,file_out);

		for (int i = 0; i < compressed_output.size() - 1; i++) { 
			n = compressed_output[i + 1]; 
		
			if (lzw_table.find(n) == lzw_table.end()) { 
				s = lzw_table[old]; 
				s.push_back(c); 
			}
			else
			{
				s = lzw_table[n]; 	
			}
		
			for(int i=0;i<s.size();i++)
			{
				char temp_ch=s[i];
				fwrite((void*)(&temp_ch),sizeof(char),1,file_out);
			}	
			c =(char) s[0]; 
		
			string temp_s(lzw_table[old]);
			temp_s.push_back(c);
			lzw_table[count] = temp_s; 
			count++;
			old = n; 
		}
		fclose(file_out);
}

