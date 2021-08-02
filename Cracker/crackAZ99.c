#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <unistd.h>
#include <pthread.h>

// thread defition

#define threadNum 8
struct arguments {
char * salt_and_encrypted;
int threadID;
};
int solution = 0;

// compile: cc -o crack CrackAZ99.c -lcrypt -lpthread
// run ./crack 

/******************************************************************************
  Demonstrates how to crack an encrypted password using a simple
  "brute force" algorithm. Works on passwords that consist only of 2 uppercase
  letters and a 2 digit integer.

  Compile with:
    cc -o CrackAZ99 CrackAZ99.c -lcrypt

  If you want to analyse the output then use the redirection operator to send
  output to a file that you can view using an editor or the less utility:
    ./CrackAZ99 > output.txt

  Dr Kevan Buckley, University of Wolverhampton, 2018 Modified by Dr. Ali Safaa 2019
******************************************************************************/

int count=0;     // A counter used to track the number of combinations explored so far

/**
 Required by lack of standard function in C.   
*/

void substr(char *dest, char *src, int start, int length){
  memcpy(dest, src + start, length);
  *(dest + length) = '\0';
}

/**
 This function can crack the kind of password explained above. All combinations
 that are tried are displayed and when the password is found, #, is put at the 
 start of the line. Note that one of the most time consuming operations that 
 it performs is the output of intermediate results, so performance experiments 
 for this kind of program should not include this. i.e. comment out the printfs.
*/

void threadCracker(void *arguments){
  int x, y, z;     // Loop counters
  char salt[7];    // String used in hashing the password. Need space for \0 // incase you have modified the salt value, then should modifiy the number accordingly
  char plain[7];   // The combination of letters currently being checked // Please modifiy the number when you enlarge the encrypted password.
  char *enc;       // Pointer to the encrypted password
  int threadId;	// Id of thread
  int valueStart, valueEnd; // start and end to find solution
  int size; //size for which part the thread should be checking 
  
  struct arguments *args = arguments; // this allows access member for cast argument of type to struct 
    threadId = args->threadID;  //store threadId from struct argument 

  substr(salt, args->salt_and_encrypted, 0, 6);
  size - 100 / threadNum;
  
  // so this will compute values for start and end mumber for our search.
  if(threadId < threadNum -1)
  {
  	valueStart = threadId * size;
  	valueEnd = (threadId+1)*size-1;
  } 
  
  else { //fixes the issue of valueEnd as its always 99
  	valueStart = threadId*size;
  	valueEnd = 100-1;
  }

	 for (z = valueStart; z <= valueEnd; z++) {
        for (x = 'A'; x <= 'Z'; x++) {
            for (y = 'A'; y <= 'Z'; y++) {
                if (solution == 0) { 
                    sprintf(plain, "%c%c%02d", x, y, z);
                    enc = (char *) crypt(plain, salt);
                    count++;
                    if (strcmp(args->salt_and_encrypted, enc) == 0) {
                        printf("#%-8d%s %s\n", count, plain, enc);
                        solution = 1;
                    }
                } else {   // only when solution is found
                    pthread_exit(NULL);
                    return NULL;
                   
        } 
      }
    }
  }
	pthread_exit(NULL);
	return NULL;
}
void crack(char *salt_and_encrypted){
  int threadId;        
  int returnC;  
  
  pthread_t threads[threadNum];  // Declare threads arrays which hold handle of threads
  
  struct arguments args[threadNum];  // Declare input argument for each thread

  for (threadId = 0; threadId < threadNum; threadId++) {

    args[threadId].salt_and_encrypted = salt_and_encrypted;
    args[threadId].threadID = threadId;

    returnC = pthread_create(&threads[threadId], NULL, threadCracker, (void *)&args[threadId]);  // Launch threads
    
    if (returnC) {
        printf("ERROR: %d\n", returnC);
        exit(-1);
    }
  }
  
  for (threadId = 0; threadId < threadNum; threadId++) {
      pthread_join(threads[threadId],NULL);  // Wait threads finish
  }
 
}

int main(int argc, char *argv[]){
  crack("$6$AS$5nmL8OmGQwiLcy3Wnjmd5v3wOqlMy4ok1nl6af6XItDXRkHEH8e28vJSMCBfB.rARqR0berjf871KRVHT1Rub1"); //Copy and Paste your ecrypted password here using EncryptShA512 program
  printf("%d solutions found\n", count);

  return 0;
}

