# Multi-threaded-Sorting-Program

Practice developing a multithreaded application in C with Pthreads
Multithreaded sorting program that works as follows: 
* An array of double values is divided into two smaller lists of equal size. Two separate threads (sorting threads) sort each sublist using selection sort. The two sublists are then merged by a third thread --- a merging thread -- which merges the two sorted sublists into a single sorted list (merge sort).
