

#include <vector>
#include <string>
#include <map>

#define MAX_INPUT_SIZE 500*1024*1024
#define MAX_OUTPUT_SIZE 500*1024*1024
#define CHUNK_SIZE 8*1024


typedef struct chunks{
     unsigned char chunk[CHUNK_SIZE];
}chunks; 




std::vector< unsigned short int > LZW_compress(unsigned char *input_chunk,int chunk_size) {

		std::map< std::vector<unsigned char>,int > lzw_table;
        std::vector< unsigned short int > compressed_output;
		// Clear the table from previous LZW runs.
		lzw_table.clear();

	
		
		int prev_match = 0;
		int match = 0;
		int inputByte = 0;

		// Load the LZW table with basic characters.
		for (int i = 0; i < 256; i++) {
			std::vector<unsigned char> s;
			s.push_back((unsigned char)i);
			lzw_table.insert(std::pair<std::vector<unsigned char>,int>(s,i));
		}
		std::vector<unsigned char> s;

	

		// Compress until finished with the data.
		while (inputByte < chunk_size) {

		// 	// Get a new character.
		// 	P.push_back(Chunk[inputByte]);
			s.push_back(input_chunk[inputByte]);
			
			match = 0;

			// Check against the LZW table to see if it's there.
			if(lzw_table.find(s)!=lzw_table.end())
			{
				match = lzw_table[s];
				prev_match = match;
				
			}
				

			// Match was not found.
			if (match == 0) {

				// Add the unique string to the table.
				lzw_table.insert(std::pair<std::vector<unsigned char>,int>(s,lzw_table.size()));

				// Send the previous match as the compressed output.
				compressed_output.push_back(prev_match);

				// Retain the most recent character.
				s.clear();
				s.push_back(*(input_chunk+inputByte));

				prev_match=lzw_table[s];
			

				// If no more input is left, send out what we have.
				if (inputByte == chunk_size - 1) {
					compressed_output.push_back(prev_match);
				}

			} else {

				// If no more input is left, send out what we have.
				if (inputByte == chunk_size - 1) {
					compressed_output.push_back(match);
				}
			}

			inputByte++;
		}
		std::cout<<"last elem "<<input_chunk[inputByte-1]<<std::endl;
		// Return the size of the compressed data + the 4-byte header.
		return compressed_output;
	
}

void LZW_decompress(std::vector< unsigned short int > compressed_output) {

		
		// Clear the table from previous LZW runs.
		std::map< int,std::vector<unsigned char> > lzw_table;
		
		lzw_table.clear();

		if(compressed_output.size()==0)
			return;
		
		int prev_match = 0;
		int match = 0;
		int inputByte = 0;

		// Load the LZW table with basic characters.
		for (int i = 0; i < 256; i++) {
			std::vector<unsigned char> s;
			s.push_back((unsigned char)i);
		
			lzw_table.insert(std::pair<int,std::vector<unsigned char>>(i,s));
		}
		int old = compressed_output[0], n; 
        std::vector<unsigned char> s(lzw_table[old]); 
		unsigned char c;
		int count=256;

		FILE *file_out=fopen("./output_data","ab");
		fwrite((void*)(&compressed_output[0]),sizeof(unsigned char),1,file_out);

		for (int i = 0; i < compressed_output.size() - 1; i++) { 
			n = compressed_output[i + 1]; 
		
			if (lzw_table.find(n) == lzw_table.end()) { 
				s = lzw_table[old]; 
				s.push_back(c); 
			} 
			else { 
				s = lzw_table[n]; 
			}
			std::string str(s.begin(),s.end());
			for(int i=0;i<str.size();i++)
			{
				unsigned char temp_ch=str[i];
				fwrite((void*)(&temp_ch),sizeof(unsigned char),1,file_out);
			}	
			c = s[0]; 
			std::vector<unsigned char> temp_s(lzw_table[old].begin(),lzw_table[old].end());
			temp_s.push_back(c);
			lzw_table[count] = temp_s; 
			count++;
			old = n; 
		}
		fclose(file_out);

		
	

	
}