#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

//Vector holding all numbers to be sorted
vector<int> arr;

pthread_mutex_t locker;

int sort_counter = 0;
int merge_counter = 0;

//Upper
int u = 0;
//Count of Numbers
int n = 0;
//# of Threads
int p = 0;

//Round # for Merging
int seg = 1;

void* sortArray(void* arg) {
    pthread_mutex_lock(&locker);

    int part = sort_counter++;

    if(part != p-1) {
        cout << "This is thread: " << part << endl;

        int start = part * floor(n / p);
        cout << "The segment start index is: " << start << endl;
        int end = start + floor(n / p);
        cout << "The segment end index is: " << end - 1 << endl;

        sort(arr.begin() + start, arr.begin() + end);

        cout << "This is the sorted segment: " << endl;
        for(int i = start; i < end; i++) {
            cout << arr[i] << " ";
        }

        cout << "\n\n";
    }
    else {
        int start = part * floor(n / p);
        cout << "The segment start index is: " << start << endl;
        int end = arr.size();
        cout << "The segment end index is: " << end - 1 << endl;
        
        sort(arr.begin() + start, arr.end());

        cout << "This is the sorted segment: " << endl;
        for(int i = start; i < arr.size(); i++) {
            cout << arr[i] << " ";
        }

        cout << "\n\n";
    }

    pthread_mutex_unlock(&locker);

    pthread_exit(0);
}

void* mergeArray(void* args) {
    pthread_mutex_lock(&locker);

    int part = merge_counter++;
    
    cout << "Start of Merge: " << endl;
    cout << "The round is: " << seg << endl;

    int start1 = part * floor(n / p) * 2 * seg;
    int end1 = 0;

    //Find when the end of 1st segment should be - this will be based on when it encounters a value smaller than it
    for(int i = start1; i < arr.size() - 1; i++) {
        if(arr[i] > arr[i + 1]) {
            end1 = i;
            break;
        }
    }

    //Start of second segment is 1 + end1
    int start2 = end1 + 1;
    int end2 = 0;

    //If end2 + the segment size is > the total numbers, then end2 = the array size
    //Otherwise it equals start2 + segment size
    if(n - (start2 + floor(n / p) * seg) < (floor(n / p) * seg))
        end2 = arr.size();
    else
        end2 = start2 + (floor(n / p) * seg);

    //We only merge if we can verify that segment 1 is not at the end of the array and that 
    //segment 2 actually exists - which happens when end1 is not 0 from the earlier for loop at the start of this function
    if(!(end1 == arr.size() - 1) && !(end1 == 0)) {
        cout << "Start 1 is: " << start1 << endl;
        cout << "End 1 is: " << end1 << endl;
        cout << "Start 2 is: " << start2 << endl;
        cout << "End 2 is: " << end2 - 1 << endl;

        inplace_merge(arr.begin() + start1, arr.begin() + start2, arr.begin() + end2);
    }

    cout << "The current merged array is: " << endl;
    for(int i = 0; i < n; i++) {
        if(i + 1 == arr.size())
            cout << arr[i] << "\n\n";
        else
            cout << arr[i] << " ";
    }

    pthread_mutex_unlock(&locker);

    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    for(int i = 0; i < argc; i++) {
        if(i == 1)
            n = strtol(argv[i], NULL, 10);
        else if(i == 2) 
            u = strtol(argv[i], NULL, 10);
        else if(i == 3)
            p = strtol(argv[i], NULL, 10);
    }

    pthread_t sort_threads[p];
    pthread_t merge_threads[p];

    //Populate vector with random numbers using seed
    srand(time(0));
    for(int i = 0; i < n; i++) {
        int newNum = rand() % u + 1;

        arr.push_back(newNum);
    }

    cout << "The populated array is: " << endl;
    for(int i = 0; i < n; i++) {
        if(i + 1 == arr.size())
            cout << arr[i] << "\n\n";
        else
            cout << arr[i] << " ";
    }

    //Create Sort Threads
    for(int i = 0; i < p; i++) {
        pthread_create(&sort_threads[i], NULL, sortArray, NULL);
    }
    
    //Join Sort Threads
    for(int i = 0; i < p; i++) {
        pthread_join(sort_threads[i], NULL);
    }

    cout << "The sorted and segmented array is: " << endl;
    for(int i = 0; i < n; i++) {
        if(i + 1 == arr.size())
            cout << arr[i] << "\n\n";
        else
            cout << arr[i] << " ";
    }

    //Calculate how many "segments" there will be initially after 1 merge round
    int segments = p % 2 + p / 2;

    //Divide by 2 and add the modulo to get how many segments there will be after each merge round from the initial segments total
    //i.e.
    //3 3 3 3 3
    //ROUND 1
    //3 3 3 -> 5/2 + 5%2 = 3 segments
    //ROUND 2
    //3 3 -> 3/2 + 3%2 = 2 segments
    //ROUND 3
    //3 -> 2/2 + 2%2 = 1 segment
    for(int i = segments; i != 0; i = i % 2 + i / 2) {
        //If the segment count is not even then we need to account for the extra segment that will not be merged with anything
        //i.e.
        //3 3 3 3 3 -> will become 3 segments after merging
        //So we will do 3 threads - 2 for each pair of 3's and 1 thread for the lone segment
        //Similarly, ROUND 3 - we only need 1 thread so 0 -> 1, because we are combining the sorted front part of the array with
        //the remaining back end of the array
        if(!(i % 2 == 0)) {
            for(int a = 0; a < i; a++) {
                pthread_create(&merge_threads[a], NULL, mergeArray, NULL);
            }

            for(int a = 0; a < i; a++) {
                pthread_join(merge_threads[a], NULL);
            }
        }
        //Else we just need 1 less thread for merging
        //i.e. 
        //3 3 3 -> We only need 1 thread because only 2 segments will be combined this round so 2 - 1 = 1 thread
        else {
            for(int a = 0; a < i - 1; a++) {
                pthread_create(&merge_threads[a], NULL, mergeArray, NULL);
            }

            for(int a = 0; a < i - 1; a++) {
                pthread_join(merge_threads[a], NULL);
            }
        }

        //Force the for loop to quit if we are down to 1 segment for final round
        if(i == 1)
            i -= 1;

        //Merge counter tells how much to "multiply" our start position in the merge function
        merge_counter = 0;
        //Seg tells us what round we are on
        seg++;
    }

    cout << "The merged array is: " << endl;
  for(int i = 0; i < n; i++) {
        if(i + 1 == arr.size())
            cout << arr[i] << "\n\n";
        else
            cout << arr[i] << " ";
    }

    return 0;
}