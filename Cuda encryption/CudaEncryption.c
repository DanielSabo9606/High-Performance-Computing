#include <stdlib.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <stdbool.h>


// Definitions for crypted text/text length and num

#define eText 10
#define pText 4
#define aNum 26

__device__ void CudaCrypt(char* rawPassword, char * newPassword) {

	newPassword[0] = rawPassword[0] + 2;
	newPassword[1] = rawPassword[0] - 2;
	newPassword[2] = rawPassword[0] + 1;
	newPassword[3] = rawPassword[1] + 3;
	newPassword[4] = rawPassword[1] - 3;
	newPassword[5] = rawPassword[1] - 1;
	newPassword[6] = rawPassword[2] + 2;
	newPassword[7] = rawPassword[2] - 2;
	newPassword[8] = rawPassword[3] + 4;
	newPassword[9] = rawPassword[3] - 4;

	for (int i = 0; i < 10; i++) {
		if (i >= 0 && i < 6) { //checking all lower case letter limits
			if (newPassword[i] > 122) {
				newPassword[i] = (newPassword[i] - 122) + 97;
			} else if (newPassword[i] < 97) {
				newPassword[i] = (97 - newPassword[i]) + 97;
			}
		} else { //checking number section
			if (newPassword[i] > 57) {
				newPassword[i] = (newPassword[i] - 57) + 48;
			} else if (newPassword[i] < 48) {
				newPassword[i] = (48 - newPassword[i]) + 48;
			}
		}
	}
}

__global__ void kernelCracker(char * crackedText, char* plainText)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;
	int z = blockIdx.z * blockDim.z + threadIdx.z;
	char cText[4];
	char nText[10];
	_Bool found = true;
	
	if (x <26 && y < 26 && z <100)
	{
	    //convert xyz values
	    cText[0] = 97 +x;
	    cText[1] = 97+y;
	    cText[2] = 48 + (z/10);
	    cText[3] = 48 + (z%10);
	    
	    CudaCrypt(cText, nText); //encrypt cText to nText
	    
        
        for (int i = 0; i <10; i++)
        {
            if(crackedText[i] !=nText[i])
            {
             found = false;
             break;
            }
            }
            if(found)  // so If solution is found use it for the buffer
            {
              plainText[0] = cText[0];
              plainText[1] = cText[1];  
              plainText[2] = cText[2];  
              plainText[3] = cText[3];    
            
            }
            }
            }
            
void GpuCracker (char *crackedText)
{
printf("GPU cracking in progressL %s\n", crackedText);
//declaring/alocation of memory for storing
char * memCryptedText;
char * memPlainText;
char * result[5]

cudaMalloc((void**)&memCryptedText, eText);
cudaMalloc((void**)&memPlainText, pText);

cudaMemcpy(memCryptedText, crackedText, eText, cudaMemcpyHostToDevice);
// we need to check for 26x26x100 different combinations which are devided into smaller blocks
dim3 block(8,8,16); 
dim3 grid(aNum/8+1, aNum/8+1, 100/16+1);

// launch kerner cracker and transfer results into host:
kernelCracker<<<grid,block>>>(memCryptedText, memPlainText);
cudaMemcpy(result, memPlainText, pText, cudaMemcpyHostToDevice);
result[4]='\0';
//Display Results:
printf("result: %s\n", result);
cudaFree(memCryptedText);
cudaFree(memPlainText);
}

#define FILE_NAME "encrypted_password.bin"

int main(int argc, char *argv[]){

    char * filename;                // File name contain encrypted text
    char *encryped_text = NULL;     // encrypted text
    int fsize = 0;                  // size of file, it also length of encrypted text
    FILE *fp;                       // pointer to file

    filename = argv[1];
    fp = fopen(filename, "r");      // open file in reading mode
    if(fp) {
        fseek(fp, 0, SEEK_END);     // move file cursor to end of file
        fsize = ftell(fp);          // get size of file
        rewind(fp);                 // set file cursor to beginning of file, ready for reading

        encryped_text = (char*) malloc(sizeof(char) * fsize);        // Allocate buffer to store content of file
        fread(encryped_text, 1, fsize, fp);                          // read content of file to buffer

        fclose(fp);                                                  // Close file
    }
  crack_gpu(encryped_text);

  free(encryped_text);

  return 0;
}





















            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            