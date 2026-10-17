#pragma once
#include "Profiler.h"

constexpr char bubbleComparison[] = "1_bubble_comparison";
constexpr char bubbleRuntime[] = "bubble_runtime";

constexpr char bubbleAssignComparison[] = "2_bubble_assign_comparison";
constexpr char bubbleCompareComparison[] = "3_bubble_compare_comparison";

constexpr char bubbleIterativeRuntime[] = "bubble_iterative_runtime";
constexpr char bubbleRecursiveRuntime[] = "bubble_recursive_runtime";

constexpr char bubbleIterative[] = "bubble_iterative";
constexpr char bubbleRecursive[] = "bubble_recursive";

constexpr char bubbleIterativeAssign[] = "bubble_iterative_assign";
constexpr char bubbleIterativeCompare[] = "bubble_iterative_compare";
constexpr char bubbleRecursiveAssign[] = "bubble_recursive_assign";
constexpr char bubbleRecursiveCompare[] = "bubble_recursive_compare";

static auto profiler = Profiler();

static void bubbleSortIterative(int arr[], const int n) {
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
                profiler.countOperation(bubbleIterativeAssign, n, 3);
                swapped = true;
            }
            profiler.countOperation(bubbleIterativeCompare, n);
        }
        if (!swapped) {
            //No swaps means it's sorted
            break;
        }
    }
}

static void bubbleSortRecursive(int arr[], const int size, const int n) {
    if (size == 1) {
        // Already sorted
        return;
    }
    bool swapped = false;
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            std::swap(arr[i], arr[i + 1]);
            profiler.countOperation(bubbleRecursiveAssign, n, 3);
            swapped = true;
        }
        profiler.countOperation(bubbleRecursiveCompare, n);
    }
    if (!swapped) {
        //No swaps means it's sorted
        return;
    }
    //Largest element is sorted, going down
    bubbleSortRecursive(arr, size - 1, n);
}

static void printArray(int arr[], const int n) {
    for (int i = 0; i < n; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << '\n';
}

static void checkBubbleCorrectness() {
    constexpr int n = 5;
    int exampleArr[] = {5, 1, 4, 2, 8};
    int tempArr[n] = {};

    std::cout << "Checking bubble sort correctness\nExample array: ";
    printArray(exampleArr, n);

    std::cout << "\nBubble sort iterative profiled test\noutput: ";
    CopyArray(tempArr, exampleArr, n);
    bubbleSortIterative(tempArr, n);
    printArray(tempArr, n);
    if (IsSorted(tempArr, n)) {
        std::cout << "It's sorted!\n";
    } else {
        std::cout << "Not sorted!\n";
    }


    std::cout << "\nBubble sort recursive profiled test\noutput: ";
    CopyArray(tempArr, exampleArr, n);
    bubbleSortRecursive(tempArr, n, n);
    printArray(tempArr, n);
    if (IsSorted(tempArr, n)) {
        std::cout << "It's sorted!\n";
    } else {
        std::cout << "Not sorted!\n";
    }
}

static void profileBubbleRuntime() {
    constexpr int maxSize = 10000;
    constexpr int numOfTests = 10;
    int arr[maxSize], tempArr[numOfTests][maxSize];
    std::cout << "profiling bubble iterative vs recursive runtimes\n";
    for (int n = 100; n <= maxSize; n += 100) {
        std::cout << "n: " << n << '\n';
        FillRandomArray(arr, n);

        for (auto & test : tempArr) {
            CopyArray(test, arr, n);
        }

        profiler.startTimer(bubbleIterativeRuntime, n);
        for (auto & test : tempArr) {
            bubbleSortIterative(test, n);
        }
        profiler.stopTimer(bubbleIterativeRuntime, n);

        for (auto & test : tempArr) {
            CopyArray(test, arr, n);
        }

        profiler.startTimer(bubbleRecursiveRuntime, n);
        for (auto & test : tempArr) {
            bubbleSortRecursive(test, n, n);
        }
        profiler.stopTimer(bubbleRecursiveRuntime, n);
    }

    profiler.createGroup(bubbleRuntime, bubbleIterativeRuntime, bubbleRecursiveRuntime);
}

static void profileBubbleNumOfOps() {
    constexpr int maxSize = 1000;
    constexpr int numOfTests = 5;
    int arr[maxSize], tempArr[maxSize];
    std::cout << "profiling bubble iterative vs recursive num of ops\n";
    for (int n = 100; n <= maxSize; n += 100) {
        std::cout << "n: " << n << '\n';
        for (int test = 0; test < numOfTests; test++) {
            FillRandomArray(arr, n);
            CopyArray(tempArr, arr, n);
            bubbleSortIterative(arr, n);
            bubbleSortRecursive(tempArr, n, n);
        }
    }

    profiler.divideValues(bubbleIterativeAssign, numOfTests);
    profiler.divideValues(bubbleIterativeCompare, numOfTests);
    profiler.divideValues(bubbleRecursiveAssign, numOfTests);
    profiler.divideValues(bubbleRecursiveCompare, numOfTests);

    profiler.addSeries(bubbleIterative, bubbleIterativeAssign, bubbleIterativeCompare);
    profiler.addSeries(bubbleRecursive, bubbleRecursiveAssign, bubbleRecursiveCompare);

    profiler.createGroup(bubbleAssignComparison, bubbleIterativeAssign, bubbleRecursiveAssign);
    profiler.createGroup(bubbleCompareComparison, bubbleIterativeCompare, bubbleRecursiveCompare);
    profiler.createGroup(bubbleComparison, bubbleIterative, bubbleRecursive);
}

static void bubbleSortProfiling() {
    profiler.reset("Bubble_Sort_Iterative_vs_Recursive");
    profileBubbleNumOfOps();
    profileBubbleRuntime();
    profiler.showReport();
}