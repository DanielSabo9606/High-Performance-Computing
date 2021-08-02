#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_MATRIX 100

// Thread count 
#define THREADS 8 

// Struct to store information
typedef struct m_matrix {
  int mRow;
  int mCol;
  double * arrMatrix;
  }Matrix;
 
  
struct arguments {
    Matrix * A;
    Matrix * B;
    Matrix * C;
    int start;
    int end;
};

// Free any memory
void memory_free(Matrix * matrix) {
  free(matrix->arrMatrix);
}

// read data of single matrix from current file position
void readMatrix(FILE* fp, Matrix * matrix){
  int row,col;
  double value;
  fscanf(fp, "%d,%d\n", &matrix->mRow, &matrix->mCol);
  matrix->arrMatrix = (double*) malloc(matrix->mRow * matrix->mCol * sizeof(double));
  for (row = 0; row < matrix->mRow; row++) {
  for( col = 0; col < matrix->mCol -1 ; col++){
      fscanf(fp, "%lf,", &value);
      matrix->arrMatrix[row * matrix->mCol + col] = value;
    }
    fscanf(fp, "%lf\n", &value);
    matrix->arrMatrix[row * matrix->mCol + matrix->mCol -1] = value;
  }
}

void printMatrix(Matrix * matrix){
  int row,col;
  printf("Size %d x %d\n", matrix->mRow, matrix->mCol);
  for (row = 0; row < matrix->mRow; row++) {
  for(col = 0; col < matrix->mCol; col++){
    printf ("%.2lf ", matrix->arrMatrix[row * matrix->mCol + col]);
    }
       printf ("\n");
        }
            printf ("\n");
}

void saveMatrix(char * fileName, Matrix * matrix){
  int row,col;
  FILE * fp;
  fp = fopen(fileName,"w");
  fprintf(fp,"%d,%d\n", matrix->mRow, matrix->mCol);

  for (row = 0; row < matrix->mRow; row++) {
  for(col = 0; col < matrix->mCol -1; col++){
    fprintf (fp,"%.2lf,", matrix->arrMatrix[row * matrix->mCol + col]);
    }
    fprintf (fp,"%.2lf\n", matrix->arrMatrix[row * matrix->mCol + matrix->mCol -1]);
  }
    fprintf (fp,"\n");
    fclose(fp);
}

void printMatrixes(Matrix ** matrix, int numMatrix){
  int index;
  for (index = 0; index < numMatrix; index++){
    printf("%d. Size %d x %d\n\n", index + 1, matrix[index]->mRow, matrix[index]->mCol);
  }

}

void readMatrixes(char * fileName, Matrix ***matrixes, int * numMatrix) {
  FILE * fp;
  Matrix * matrix;
  int index = 0;
  *matrixes = (Matrix**) malloc(MAX_MATRIX * sizeof(Matrix*));

  fp = fopen(fileName,"r");

  while(!feof(fp)){
    matrix = (Matrix*) malloc(sizeof(Matrix));
    readMatrix(fp, matrix);
    (*matrixes)[index++] = matrix;

    printMatrix(matrix);

  }

  *numMatrix = index;


  fclose(fp);
}

// Multiplication of matrixes
void matrixMulti(Matrix * A, Matrix * B, Matrix **C)
{
  int mRow, mCol, mI;
  int row,col,i;
  double sum;
  mRow = A->mRow;
  mCol = B->mCol;
  mI = A->mCol;

  (*C)->mCol = mCol;
  (*C)->mRow = mRow;
  (*C)->arrMatrix = (double*) malloc(mRow * mCol * sizeof(double));

  for (row = 0; row < mRow; row++){
    for (col = 0; col < mCol; col++){
      sum = 0;
      for (i = 0; i < mI; i++)
      {
        sum += A->arrMatrix[row * mI + i] * B->arrMatrix[i * mCol + col];
      }
      (*C)->arrMatrix[row * mCol + col] = sum;

    }
  }
}

void * matrixMul(void *arguments) {

    Matrix * A, * B, *C;
    int start;
    int end;
    int row,col,i;
    double sum;


    struct arguments *args = arguments; 
    	A = args->A;
    	B = args->B;
    	C = args->C;
    	start = args->start;
    	end = args->end;

    for (row = start; row <= end; row++){
    for (col = 0; col < B->mCol; col++){
      sum = 0;
      for (i = 0; i < A->mCol; i++)
      {
      sum += A->arrMatrix[row * A->mCol + i] * B->arrMatrix[i * B->mCol + col];
      }
      C->arrMatrix[row * B->mCol + col] = sum;
      }
    }

    pthread_exit(NULL);
    return NULL;
}

void pthreadMul(Matrix * A, Matrix * B, Matrix **C){
  int threadId;
  int mReturn; 
  int mSize;
  int threadCnt = THREADS;
  pthread_t threads[THREADS];  
  struct arguments args[THREADS];

  int mRow, mCol, mI;
  	mRow = A->mRow;
  	mCol = B->mCol;
  	mI = A->mCol;

  (*C)->mCol = mCol;
  (*C)->mRow = mRow;
  (*C)->arrMatrix = (double*) malloc(mRow * mCol * sizeof(double));
  
  if (A->mRow < THREADS)
  {
    threadCnt = A->mRow;
  }

  mSize = A->mRow / threadCnt;

  for (threadId = 0; threadId< threadCnt; threadId++) {

    args[threadId].A = A;
    args[threadId].B = B;
    args[threadId].C = *C;
       
    if (threadId < threadCnt - 1) 
    {
        args[threadId].start = threadId * mSize;
        args[threadId].end = (threadId + 1) * mSize - 1;

    } else { 
        args[threadId].start = threadId * mSize;
        args[threadId].end = A->mRow - 1;
    }
    
    mReturn = pthread_create(&threads[threadId], NULL, matrixMul, (void *)&args[threadId]);
    
    if (mReturn) {
        printf("ERROR; from pthread_create() is %d\n", mReturn);
        exit(-1);
    }
  }
  for (threadId = 0; threadId < threadCnt; threadId++) {
      pthread_join(threads[threadId],NULL);
  }
}

int matrixIndex(char * question, int max)
{
  int selection = 0;
  do {
    printf("%s:\n", question);
    scanf("%d", &selection);
    if (selection < 1 || selection > max)
      printf("Please input again\n");
  }
  while ((selection < 1 || selection > max));
  return selection;

}

int main()
{
  Matrix ** matrices = 0;
  int matrixNum = 0;
  int firstMatrix = 0 ;
  int secondMatrix = 1;
  int i;

  Matrix * result;

  readMatrixes("1627286-matrices.txt", &matrices, &matrixNum);
  printMatrixes(matrices, matrixNum);
  firstMatrix = matrixIndex("Input index of first matrix (start from 1)", matrixNum);
  secondMatrix = matrixIndex("Input index of second matrix (start from 1)", matrixNum);

  if (matrices[firstMatrix-1]->mCol != matrices[secondMatrix-1]->mRow) {
    printf("cant multiple matrix %d x %d and %d x %d \n",
      matrices[firstMatrix-1]->mRow, matrices[firstMatrix-1]->mCol, 
      matrices[secondMatrix-1]->mRow, matrices[secondMatrix-1]->mCol);

  }else
  {
    
    pthreadMul(matrices[firstMatrix-1], matrices[secondMatrix-1], &result);
    saveMatrix("output_matrix.txt", result);

     memory_free(result);
  }

  for (i = 0 ; i < matrixNum; i++)
  {
    memory_free(matrices[i]);

  }

  return 0;
}