There are 3 different version of merge sort to be found in this project.
mergesortPT2.cpp is a "hybrid" implementation of merge sort. Parallelization
of this algorithm is not that hard to implement - efficency of the parallel
version is quite a different story. Since merge sort is a recursive algorithm
in which there are two recursive calls and they are independent of each other
(we have a call to sort the left part of the array and then the right part of
the array, these two parts of the array do not share any information during the
sorting). As such, it is quite easy to see how a parallel algorithm could be
made. Each recursive call could be spawned in a new thread. But, this is not
efficient. The increase in the number of the threads can be exponential (in the
worst case). For most computers today, this is not an option. This is not the
only problem though. A lot of time can be lost with spawning new threads for
very small arrays (for example with one or two elements). This is a waste of
resources, since it will take longer to make a new thread, check the data and
then join the thread back. Thus in this hybrid version what we have done is:
put a limit on the number of threads that can be made and also put a limit on
the size of the array for which we are spawning a new thread.

1) put a limit on the number of threads
Limit was set to maximum of 100 threads and can be changed in the code. If all
the threads are used up, next call will not make a new thread, but will instead
use sequential mergesort

2) put a limit on the array size
Limit used in the code was 10000 elements. Notice that if the array is small
(less than 10000 elements), there will never be any new threads and this is
essentially a sequential mergesort. This way we assure that the new threads
are made only when they are necessary. Parallel mergesort is incredibly
inefficient for small arrays.

mergesortPT3.cpp is a comepltely parallel implentation of merge sort. But,
unlike the above, which does the recursion in parallel, this algorithm
separates the array onto a predetermined number of subarrays and hands them
to corresponding number of threads (which cannot be changed, it does not
increase or decrease, thus you have full control over how many threads your
computer can handle). This is very very inefficient for small arrays. 
After separating out our main array to subarrays, and each subarray getting a
corresponding thread, these threads then call a sequential mergesort.

These parallel sorting alogrithms require the array size to be quite large
(over 100 000 elements) to have time similar to the time needed for sequential
sort. This of course will vary based on the computer power, choice of the
number of workers and other factors, but as far as I have tested it out, it has
little to no improvement (and if anything in same case, it is even worse!) as
compared to sequential merge sort.

In the last effort to make a feasible parallel merge sort algorithm, bitonic
merge sort was tested out (iterative version). This algorithm was very easy
to paralellize and is quite efficient. The only drawback is that it requires
the number of elements in the list to be a power of 2 - but it can be
implemented so it goes around this problem (separate out array into subarrays
that have length which is power of 2) and in this case it would be a neat
parallel sorting algorithm.
