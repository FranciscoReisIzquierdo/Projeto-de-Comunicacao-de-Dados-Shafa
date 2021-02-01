#ifndef FSIZE_H_INCLUDED
#define FSIZE_H_INCLUDED

#define FSIZE_DEFAULT_BLOCK_SIZE 524288         // Default block size = 512 KBytes
#define FSIZE_MIN_BLOCK_SIZE 512                // Min block size = 512 Bytes
#define FSIZE_MAX_BLOCK_SIZE 67108864           // Max block size = 64 MBytes
#define FSIZE_MAX_NUMBER_OF_BLOCKS 4294967296   // Max number of blocks that can be returned = 2^32 blocks
#define FSIZE_MAX_SIZE_FSEEK 2147483648         // Max size permitted in fseek/ftell functions = 2 GBytes
#define FSIZE_ERROR_BLOCK_SIZE -1               // Error: Block size is larger than max value
#define FSIZE_ERROR_NUMBER_OF_BLOCKS -2         // Error: Number of Blocks exceeds max value permitted
#define FSIZE_ERROR_IN_FILE -3                  // Error: Opening or reading file
#define FSIZE_ERROR_IN_FTELL -1L                // Error: When using ftell()

long long fsize(FILE *fp_in, unsigned char *filename, unsigned long *the_block_size, long *size_of_last_block);


long long fsize(FILE *fp_in, unsigned char *filename, unsigned long *the_block_size, long *size_of_last_block)
{
    unsigned long long total;
    long long n_blocks;
    unsigned long n_read, block_size;
    unsigned char *temp_buffer;
    int fseek_error;
    FILE *fp;

    block_size = *the_block_size;
    if (block_size > FSIZE_MAX_BLOCK_SIZE) return (FSIZE_ERROR_BLOCK_SIZE);
    if (block_size == 0UL) block_size = FSIZE_DEFAULT_BLOCK_SIZE;
    if (block_size < FSIZE_MIN_BLOCK_SIZE) block_size = FSIZE_MIN_BLOCK_SIZE;
    *the_block_size = block_size;

    if (filename == NULL || *filename == 0) fp = fp_in;
    else
    { fp = fopen(filename, "rb");
      if (fp == NULL) return (FSIZE_ERROR_IN_FILE);
    }

    fseek_error = fseek(fp, 0L, SEEK_SET);
    if (fseek_error) return (FSIZE_ERROR_IN_FILE);

    fseek_error = fseek(fp, 0L, SEEK_END);
    if (!fseek_error)
    { total = ftell(fp);
      if (total == FSIZE_ERROR_IN_FTELL) return (FSIZE_ERROR_IN_FILE);
      n_blocks = total/block_size;
      if (n_blocks*block_size == total) *size_of_last_block = block_size;
      else
      { *size_of_last_block = total - n_blocks*block_size;
        n_blocks++;
      }
      return(n_blocks);
    }

    n_blocks = FSIZE_MAX_SIZE_FSEEK/block_size-1; // In reality fseek() can't handle FSIZE_MAX_SIZE_FSEEK of 2GBytes, so let's use a smaller size
    fseek_error = fseek(fp, n_blocks * block_size, SEEK_SET);
    if (fseek_error) return (FSIZE_ERROR_IN_FILE);

    temp_buffer = malloc(sizeof(unsigned char)*block_size);
    do
    { n_blocks++;
      n_read = fread(temp_buffer, sizeof(unsigned char), block_size, fp);
    } while (n_read == block_size && n_blocks <= FSIZE_MAX_NUMBER_OF_BLOCKS);

    free(temp_buffer);
    if (n_blocks > FSIZE_MAX_NUMBER_OF_BLOCKS) return(FSIZE_ERROR_NUMBER_OF_BLOCKS);

    if (n_read == 0L)
    { *size_of_last_block = block_size;
      n_blocks--;
    }
    else *size_of_last_block = n_read;

    if (filename == NULL || *filename == 0)
    { fseek_error = fseek(fp, 0L, SEEK_SET);
      if (fseek_error) return (FSIZE_ERROR_IN_FILE);
    }
    else fclose(fp);

    return(n_blocks);
}

#endif // FSIZE_H_INCLUDED
