

#include <vector>
#include <string>

std::vector< std::string > lzw_table;
std::vector< unsigned short int > compressed_output;

int LZW(const unsigned char * Chunk, unsigned char * Compressed,
		 int lzw_index, int chunk_size, int compressed_start) {

		

		// Clear the table from previous LZW runs.
		lzw_table.clear();

		// Variables required for LZW compression.
		std::string P = "";
		int prev_match = 0;
		int match = 0;
		int inputByte = 0;

		// Load the LZW table with basic characters.
		for (int i = 0; i < 256; i++) {
			lzw_table.push_back(std::string(1, (unsigned char) i));
		}


		// Compress until finished with the data.
		while (inputByte < chunk_size) {

			// Get a new character.
			P.push_back(Chunk[inputByte]);
			match = 0;

			// Check against the LZW table to see if it's there.
			for (int i = 0; i < lzw_table.size(); i++) {

				// Match found in LZW table.
				if (P.compare(lzw_table[i]) == 0) {
					match = i;
					prev_match = match;
				}
			}

			// Match was not found.
			if (match == 0) {

				// Add the unique string to the table.
				lzw_table.push_back(P);

				// Send the previous match as the compressed output.
				compressed_output.push_back(prev_match);

				// Retain the most recent character.
				P = P[P.length()-1];
				prev_match = (int) P[0];

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



		// Variables for writing out data.
		int remaining = 0;
		int outputByte = 4;
		unsigned char temp=0;

		// Pack 13-bit values onto the 8-bit output array.
		for (int j = 0; j < compressed_output.size(); j++) {

			int symbol = compressed_output[j];
			remaining += 13;

			// Stay here until we need a new symbol.
			while (remaining >= 8) {

				// Ready to pack a byte.
				if (remaining >= 8) {

					temp |= symbol >> (remaining - 8);
					Compressed[compressed_start+outputByte] = temp;
					outputByte++;

					remaining = remaining - 8;
					temp = 0; // Zero it out.
				}

				// Not yet ready to pack; return for more data.
				if (remaining < 8 && remaining >0){

					temp |= symbol << (8 - remaining);

					// If there's no more data left, send what we have, zero-padded.
					if (j == compressed_output.size() - 1) {
							Compressed[compressed_start+outputByte] = temp;
							outputByte++;
							printf("temp val%d\n",temp);

					}
				}
			}
		}

		// Update the header to reflect the size of the compressed data.
		uint32_t compressed_size = outputByte -4;
		Compressed[compressed_start+3] = (compressed_size >> 23)&(0xff);
		Compressed[compressed_start+2] = (compressed_size >> 15)&(0xff);
		Compressed[compressed_start+1] = (compressed_size >> 7)&(0xff);
		Compressed[compressed_start+0] = (compressed_size << 1)&(0xff);
		printf("Compression Value%d\n",outputByte);

		compressed_output.clear();



		// Return the size of the compressed data + the 4-byte header.
		return outputByte;
	
}