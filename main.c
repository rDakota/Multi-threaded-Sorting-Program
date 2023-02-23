/**
 * Multi-threaded sorting in c =>
 * developed a multithreaded application in C with Pthreads
 * @authors Gus Temple and Rowan Richter
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//--- Define necessary Structs for sorting and merging
typedef struct {
  double *array;
  int size;
} sortingParams;

typedef struct {
  sortingParams *first;
  sortingParams *second;
  sortingParams *third;
} mergingParams;

/*
Generate Sorting struct
*/
sortingParams *generateSorting(int size) {
  // Allocate a struct for the array
  sortingParams *arr = malloc(sizeof(sortingParams));
  // Initialize the struct's fields
  arr->array = malloc(size * sizeof(double));
  arr->size = size;
  return arr;
}

/*
Generate Merging struct
*/
mergingParams *generateMerging(sortingParams *a, sortingParams *b, sortingParams *c) {
  // Allocate a struct
  mergingParams *m = malloc(sizeof(mergingParams));
  // Initialize the struct's fields
  m->first = a;
  m->second = b;
  m->third = c;
  return m;
}

/*
Fill in array with random doubles
*/
void fillArray(sortingParams *arr) {
  srand(time(NULL)); // random
  // Generate random numbers between 1 and 99 and store them in the array
  for (int i = 0; i < arr->size; i++) {
    // divide random int by the max number in rand and multiply by 999.00 + 1.00
    // to achieve range of 1 to 1000
    arr->array[i] = ((double)rand() / (double)RAND_MAX) * 999.00 + 1.00;
  }
}

/*
Print Array
*/
void printArray(sortingParams *arr) {
  for (int i = 0; i < arr->size; i++) {
    printf("%.3lf ", arr->array[i]); // 3 decimals
  }
  printf("\n");
}

/*
Swapping function used for selection sort
*/
void swap(double *x, double *y) {
  double temp = *x;
  *x = *y;
  *y = temp;
}

/*
C implementation of selection sort
*/
void *selectionSort(void *arg) {
  sortingParams *arr = arg;
  // (size - 1) bc with the last iteration, the list will already be sorted
  for (int i = 0; i < arr->size - 1; i++) {
    int idxMinNum = i; // smallest num index
    for (int j = i + 1; j < arr->size; j++) {
      // ascending order <
      // choose min element
      if (arr->array[j] < arr->array[idxMinNum])
        idxMinNum = j;
    }
    // swap min to the right position
    swap(&arr->array[idxMinNum], &arr->array[i]);
  }
  return 0; // success
}

/*
Merge sort Function
*/
void *mergeSort(void *arg) {
  // initializations for easier readability
  mergingParams *m = arg;
  sortingParams *firstHalf = m->first;
  sortingParams *secondHalf = m->second;
  sortingParams *c = m->third;

  int firstIdx = 0;  // index for first half
  int secondIdx = 0; // index for second half
  int mergedIdx = 0; // index for merged array

  // while both indices are less than the sizes of their respective arrays
  while (firstIdx < firstHalf->size && secondIdx < secondHalf->size) {
    //if the smallest element of the first half is smaller than the smallest element from the second half
    if (firstHalf->array[firstIdx] < secondHalf->array[secondIdx]) {
      // add that element to the final sorted array, and increment the firstHalf idx variable
      c->array[mergedIdx] = firstHalf->array[firstIdx];
      firstIdx++;
    } else {
      // otherwise add the second half element
      c->array[mergedIdx] = secondHalf->array[secondIdx];
      secondIdx++;
    }
    // increment merged idx in either case
    mergedIdx++;
  }

  // once one of the indexing variables has reached the size of its array, fill the rest of the final sorted array with remaining elements
  while (firstIdx < firstHalf->size) {
    // merge first half array into array from c
    c->array[mergedIdx] = firstHalf->array[firstIdx];
    firstIdx++; 
    mergedIdx++;
  }
  while (secondIdx < secondHalf->size) {
    // merge second half array into array from c
    c->array[mergedIdx] = secondHalf->array[secondIdx];
    secondIdx++;
    mergedIdx++;
  }
  return 0; // success
}

/*
Copy Function
works for Full array copying and first half array copying
*/
void copyArray(sortingParams *og, sortingParams *copy) {
  for (int i = 0; i < copy->size; i++) {
    copy->array[i] = og->array[i];
  }
}

/*
Copy Second Half Function
*/
void copySecondHalf(sortingParams *og, sortingParams *copy) {
  for (int i = 0; i < copy->size; i++) {
    if (og->size % 2 == 0) { // if even start at index of the copy
      copy->array[i] = og->array[(copy->size) + i];
    } else { // if even start at index of the copy - 1 to properly fill index
      copy->array[i] = og->array[(copy->size - 1) + i];
    }
  }
}

/*
Main
*/
int main(int argc, char *argv[]) {
  //--- process command line argument and prints error messages when necessary
  if (argc != 2) {
    fprintf(stderr, "usage: main <arraysize>\n");
    return 1; // error
  } else if (argc == 2) {
    int n = atoi(argv[1]); // convert arg to int
    if (n <= 0) {
      printf("n must be a positive int.\n");
      return 1; // error
    }
    printf("The int argument supplied is %d\n", n);
  }

  //--- generate array a
  int size = atoi(argv[1]);
  sortingParams *a = generateSorting(size);
  // fill the array with random doubles
  printf("Random array of size %d:\n", size);
  fillArray(a); // fill a with random doubles

  //--- create array B, C, firstHalf, secondHalf
  sortingParams *b = generateSorting(size);
  sortingParams *c = generateSorting(size);
  // Halving of size
  int firstHalfSize = atoi(argv[1]) / 2;
  int secondHalfSize = firstHalfSize + (size % 2);
  // declare half sized structs
  sortingParams *firstHalf = generateSorting(firstHalfSize);
  sortingParams *secondHalf = generateSorting(secondHalfSize);

  //--- make timespec object
  struct timespec ts_begin, ts_end;
  double elapsed;

  //----------- ONE THREAD CASE --------------
  copyArray(a, b);
  //printArray(a); // test a unsorted b4
  //printArray(b); // test b unsorted b4
  // start a timer
  clock_gettime(CLOCK_MONOTONIC, &ts_begin);
  
  //--- create sortThread to sort B
  pthread_t sortThread;
  
  // create thread to sort the array
  pthread_create(&sortThread, NULL, selectionSort, b);
  
  // join the thread
  pthread_join(sortThread, NULL);
  
  // stop the timer
  clock_gettime(CLOCK_MONOTONIC, &ts_end);
  
  // figure out total time
  elapsed = ts_end.tv_sec - ts_begin.tv_sec;
  elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0;
  
  //printArray(a); // test a unsorted after
  //printArray(b); // test b sorted after
  printf("Sorting is done in %fms when ONE thread is used\n", elapsed * 1000);

  //------------------- TWO THREADS CASE ------------
  
  copyArray(a, firstHalf);
  copySecondHalf(a, secondHalf);
  
  //printArray(a); // test a array b4
  //printArray(firstHalf); // test second half unsorted b4
  //printArray(secondHalf); // test second half unsorted b4
  
  //start timer
  clock_gettime(CLOCK_MONOTONIC, &ts_begin);

  // initialize 2 threads to sort each half of the array
  pthread_t A1, A2;

  // create each thread, then join it
  pthread_create(&A1, NULL, selectionSort, firstHalf);
  pthread_create(&A2, NULL, selectionSort, secondHalf);
  pthread_join(A1, NULL);
  pthread_join(A2, NULL);
  
  //printArray(a); // test a array after 
  //printArray(firstHalf); // test first half sorted after
  //printArray(secondHalf); // test second half sorted after

  // package params for merging
  mergingParams *test = generateMerging(firstHalf, secondHalf, c);

  // initialze, create, and join another thread to merge the sorted arrays
  pthread_t threadM;
  pthread_create(&threadM, NULL, mergeSort, test);
  pthread_join(threadM, NULL);

  // end timer
  clock_gettime(CLOCK_MONOTONIC, &ts_end);  

  // figure out total time
	elapsed = ts_end.tv_sec - ts_begin.tv_sec; 
	elapsed += (ts_end.tv_nsec - ts_begin.tv_nsec) / 1000000000.0;
  //printArray(c); // test c array after
  printf("Sorting is done in %fms when TWO threads are used\n", elapsed * 1000);

  //----------------------------------------------------
  //--- Free the memory used by the structs
  free(a);
  free(b);
  free(test);
  return 0; // Success
}
