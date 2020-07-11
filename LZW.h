

#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <list>
#include <pthread.h>

#define MAX_INPUT_SIZE 500 * 1024 * 1024
#define MAX_OUTPUT_SIZE 500 * 1024 * 1024
#define CHUNK_SIZE 10 * 1024

using namespace std;

//Initialize Mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Global vector that stores the compressed output
vector<list<unsigned short int>> compressed_output_collection;

// Structure used for chunking input data
typedef struct chunks
{
	char chunk[CHUNK_SIZE];
} chunks;

//Structure for passing arguments into the thread
typedef struct compress_chunks
{
	char *input_chunk;
	int chunk_size;
	int chunk_number;
} compress_chunks;


//LZW compress function
void *LZW_compress(void *input_chunk_struct)
{

	//unpacking of chunk data
	compress_chunks *compress_chunk = (compress_chunks *)malloc(sizeof(compress_chunks));
	compress_chunk = (compress_chunks *)input_chunk_struct;
	int chunk_number = compress_chunk->chunk_number;
	int chunk_size = compress_chunk->chunk_size;
	char *input_chunk = compress_chunk->input_chunk;
	free(compress_chunk);

	//LZW table. Map is used for O(1) retrival 
	std::unordered_map<string, int> lzw_table;

	//A list that stores the compressed data along with the corresponding chunk number
	std::list<unsigned short int> compressed_output;
	
	
	// Clear the table from previous LZW runs.
	compressed_output.push_back((unsigned short int)chunk_number);


	int prev_match = 0;
	int match = 0;
	int inputByte = 0;

	// Load the LZW table with basic characters.
	for (int i = 0; i < 256; i++)
	{
		string s;
		s.push_back(char(i));
		lzw_table.insert(std::pair<string, int>(s, i));
	}
	string s;

	// Compress until finished with the data.
	while (inputByte < chunk_size)
	{

		// 	// Get a new character.

		s.push_back(input_chunk[inputByte]);

		// Check against the LZW table to see if it's there.
		if (lzw_table.find(s) != lzw_table.end())
		{
			match = lzw_table[s];
			prev_match = match;

			
		}
		else
		{
			// Add the unique string to the table.
			lzw_table.insert(std::pair<string, int>(s, lzw_table.size()));

			compressed_output.push_back(prev_match);

			// Retain the most recent character.
			s = "";
			s = s + input_chunk[inputByte];

			prev_match = lzw_table[s];

			// If no more input is left, send out what we have.
		}
		inputByte++;
	}
	compressed_output.push_back(prev_match);
	//setting critical section. We add the compressed chunk into the collection
	pthread_mutex_lock(&mutex);
		compressed_output_collection.push_back(compressed_output);
	pthread_mutex_unlock(&mutex);

	pthread_exit(0);
}

//LZW decompress function
void LZW_decompress(std::vector<unsigned short int> compressed_output)
{


	std::unordered_map<int, string> lzw_table;

	if (compressed_output.size() == 0)
		return;

	int prev_match = 0;
	int match = 0;
	int inputByte = 0;

	// Load the LZW table with basic characters.
	for (int i = 0; i < 256; i++)
	{
		string s;
		s.push_back((char)i);
		lzw_table.insert(std::pair<int, string>(i, s));
	}

	int prev = compressed_output[0], n;
	string s(lzw_table[prev]);
	char c;

	// count is for number of elements in the LZW table
	int count = 256;


	FILE *file_out = fopen("./output_data", "a");
	fwrite((void *)(&compressed_output[0]), sizeof(char), 1, file_out);

	for (int i = 0; i < compressed_output.size() - 1; i++)
	{
		n = compressed_output[i + 1];

	// if n is not in the table we push it into the table and get a new string
		if (lzw_table.find(n) == lzw_table.end())
		{
			s = lzw_table[prev];
			s.push_back(c);
		}
	// if it is inside the table we retrive the string
		else
		{
			s = lzw_table[n];
		}
    // writes out the string into the file
		for (int i = 0; i < s.size(); i++)
		{
			char temp_ch = s[i];
			fwrite((void *)(&temp_ch), sizeof(char), 1, file_out);
		}
	
		c = (char)s[0];

		string temp_s(lzw_table[prev]);
		temp_s.push_back(c);
		lzw_table[count] = temp_s;
		count++;
		prev = n;
	}
	fclose(file_out);
}
