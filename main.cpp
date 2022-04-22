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

//Vectpr holding final sorted array
vector<int> final_arr;

pthread_mutex_t locker;

int sort_counter = 0;
int merge_counter = 0;
//Upper
int u = 0;
//Count of Numbers
int n = 0;
//# of Threads
int p = 0;

int remaining_segments = 0;
//# of Rounds
int seg = 1;

//Length Counter
int length = 0;

void* sortArray(void* arg) {
    pthread_mutex_lock(&locker);

    int part = sort_counter++;

    if(part != p-1) {
        cout << "This is thread: " << part << endl;

        int start = part * floor(n / p);
        cout << "The start is: " << start << endl;
        int end = start + floor(n / p);
        cout << "The end is: " << end << endl;

        sort(arr.begin() + start, arr.begin() + end);

        cout << "This is the sorted section: " << endl;
        for(int i = start; i < end; i++) {
            cout << arr[i] << " ";
        }

        cout << endl;
    }
    else {
        int start = part * floor(n / p);
        
        sort(arr.begin() + start, arr.end());

        cout << "This is the sorted section: " << endl;
        for(int i = start; i < arr.size(); i++) {
            cout << arr[i] << " ";
        }

        cout << endl;
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
    int end1 = start1 + (floor(n / p) * seg) - 1;
    int start2 = end1 + 1;
    int end2 = 0;

    if(n - (start2 + floor(n / p) * seg) < (floor(n / p) * seg))
        end2 = arr.size();
    else
        end2 = start2 + (floor(n / p) * seg);

    // if(seg < seg / 2) {
    //     if(n - (start2 + floor(n / p)) < floor(n / p))
    //         end2 = arr.size() - 1;
    //     else
    //         end2 = start2 + floor(n / p);

    cout << "Start 1 is: " << start1 << endl;
    cout << "End 1 is: " << end1 << endl;
    cout << "Start 2 is: " << start2 << endl;
    cout << "End 2 is: " << end2 << endl;
    //merge(arr.begin() + start1, arr.begin() + end1, arr.begin() + start2, arr.begin() + end2, final_arr.begin());

    inplace_merge(arr.begin() + start1, arr.begin() + start2, arr.begin() + end2);
    //}

    // seg++;
    cout << "The sorted array is: " << endl;
    for(int i = 0; i < n; i++) {
        cout << arr[i] << endl;
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

    int mergeCount = p - 1;
    pthread_t merge_threads[p];

    //Populate vector with random numbers using seed
    srand(time(0));
    for(int i = 0; i < n; i++) {
        int newNum = rand() % u + 1;

        arr.push_back(newNum);

        cout << newNum << endl;
    }

    //Create Sort Threads
    for(int i = 0; i < p; i++) {
        pthread_create(&sort_threads[i], NULL, sortArray, NULL);
    }
    
    //Join Sort Threads
    for(int i = 0; i < p; i++) {
        pthread_join(sort_threads[i], NULL);
    }
    
    // //Create Merge Threads
    // for(int i = 0; i < p - 1; i++) {
    //     pthread_create(&merge_threads[i], NULL, mergeArray, NULL);
    // }

    // //Join Merge Threads
    // for(int i = 0; i < p - 1; i++) {
    //     pthread_join(merge_threads[i], NULL);
    // }

    cout << "The sorted array is: " << endl;
    for(int i = 0; i < n; i++) {
        cout << arr[i] << endl;
    }

    length = floor(n / p);
    bool isEven = false;
    int segments = p;

    for(int i = segments; i != 0; i /= 2) {
        cout << "The amount of segments: " << i << endl;

        remaining_segments = i;

        for(int a = 0; a < i / 2; a++) {
            pthread_create(&merge_threads[a], NULL, mergeArray, NULL);
        }

        for(int a = 0; a < i / 2; a++) {
            pthread_join(merge_threads[a], NULL);
        }

        merge_counter = 0;
        seg++;
    }


    cout << "The sorted array is: " << endl;
    for(int i = 0; i < n; i++) {
        cout << arr[i] << endl;
    }

    cout << "The merged array is: " << endl;
    for(int i = 0; i < final_arr.size(); i++) {
        cout << final_arr[i] << endl;
    }

    return 0;
}