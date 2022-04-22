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

    for(int i = start1; i < arr.size() - 1; i++) {
        if(arr[i] > arr[i + 1]) {
            end1 = i;
            break;
        }
    }

    //int end1 = start1 + (floor(n / p) * seg) - 1;
    int start2 = end1 + 1;
    int end2 = 0;

    if(n - (start2 + floor(n / p) * seg) < (floor(n / p) * seg))
        end2 = arr.size();
    else
        end2 = start2 + (floor(n / p) * seg);

    if(!(end1 == arr.size() - 1) && !(end1 == 0)) {
        cout << "Start 1 is: " << start1 << endl;
        cout << "End 1 is: " << end1 << endl;
        cout << "Start 2 is: " << start2 << endl;
        cout << "End 2 is: " << end2 - 1 << endl;

        inplace_merge(arr.begin() + start1, arr.begin() + start2, arr.begin() + end2);
    }
    // cout << "Start 1 is: " << start1 << endl;
    // cout << "End 1 is: " << end1 << endl;
    // cout << "Start 2 is: " << start2 << endl;
    // cout << "End 2 is: " << end2 - 1 << endl;

    // inplace_merge(arr.begin() + start1, arr.begin() + start2, arr.begin() + end2);

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

    int mergeCount = p - 1;
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

    int segments = p % 2 + p / 2;

    for(int i = segments; i != 0; i = i % 2 + i / 2) {
        //cout << "The amount of segments: " << i << endl;

        if(!(i % 2 == 0)) {
            for(int a = 0; a < i; a++) {
                pthread_create(&merge_threads[a], NULL, mergeArray, NULL);
            }

            for(int a = 0; a < i; a++) {
                pthread_join(merge_threads[a], NULL);
            }
        }
        else {
            for(int a = 0; a < i - 1; a++) {
                pthread_create(&merge_threads[a], NULL, mergeArray, NULL);
            }

            for(int a = 0; a < i - 1; a++) {
                pthread_join(merge_threads[a], NULL);
            }
        }

        if(i == 1)
            i -= 1;

        // for(int a = 0; a < i; a++) {
        //     pthread_create(&merge_threads[a], NULL, mergeArray, NULL);
        // }

        // for(int a = 0; a < i; a++) {
        //     pthread_join(merge_threads[a], NULL);
        // }
    
       
        // if(i % 2 == 0) {
        //     for(int a = 0; a < i / 2; a++) {
        //         pthread_create(&merge_threads[a], NULL, mergeArray, NULL);
        //     }

        //     for(int a = 0; a < i / 2; a++) {
        //         pthread_join(merge_threads[a], NULL);
        //     }
        // }

        // for(int a = segments; a < seg; a++) {
        //     pthread_create(&merge_threads[a], NULL, mergeArray, NULL);
        // }

        // for(int a = 0; a < seg; a++) {
        //     pthread_join(merge_threads[a], NULL);
        // }
    

        merge_counter = 0;
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