# About

This program is created for understanding how to implement multi-threading in C++. We use LZW compression to understand it.

# LZW Compression

LZW compression is a general purpose dictionary based compression algorithm which is primarily sequential.

LZW compression pseudocode:

```
*     PSEUDOCODE
  1     Initialize table with single character strings
  2     P = first input character
  3     WHILE not end of input stream
  4          C = next input character
  5          IF P + C is in the string table
  6            P = P + C
  7          ELSE
  8            output the code for P
  9          add P + C to the string table
  10           P = C
  11         END WHILE
  12    output code for P
  ```
  LZW decompression pseudocode:
  ```
  *    PSEUDOCODE
1    Initialize table with single character strings
2    OLD = first input code
3    output translation of OLD
4    WHILE not end of input stream
5        NEW = next input code
6        IF NEW is not in the string table
7               S = translation of OLD
8               S = S + C
9       ELSE
10              S = translation of NEW
11       output S
12       C = first character of S
13       OLD + C to the string table
14       OLD = NEW
15   END WHILE
```
credits: https://www.geeksforgeeks.org/lzw-lempel-ziv-welch-compression-technique/

# How to parallelize

The technique i used for this is to chunk the input data into multiple small chunks and run the compression function each chunk independently. We also encode the location of the chunk 
 in respect to the original file as the header of the compressed chunk.
 
Steps to do:
1. Chunk the data.
2. Initiate the threads using pthread.
3. Modify the function to use in pthread_create call.
4. Identify critical section and put mutex locks
5. Join the threads to synchronize,
6. The compressed chunks may not be stored in order. So reorganize it.
7. Decompress and check for difference using diff.


# How to use
1. make clean
2. make
3. ./LZW
4. Set the number threads to create
5. Use htop to see the processor usage.
6. Compare runtime.


# Versioning
- 1.0
    - First Release
    - Future work to include various other compression algorithm

# Author
* **Subhransu Mahapatra**
   
