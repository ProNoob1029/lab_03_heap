/**
 *  Author: Morar Dragoș Bogdan
 *  Group: 30424
 *  Lab: 3
 *  Subject: Iterative vs Recursive Bubble sort, and bottom up vs top down heap building comparison
 *  Bubble sort findings:
 *          When testing the number of operations done by each version,
 *      i've found them to be exactly identical, down to the single operation.
 *      This hints to the fact that both implementation take almost the same path
 *      to solve the problem, differing only slightly, the outer for loop of the
 *      iterative version being replaced by recursion on the other version.
 *          When comparing their CPU time, again I haven't been able to find
 *      much difference. I've tested it both on my laptop, with an
 *      Intel Core i7-7700HQ, and on my pc with an Intel Core i3-8100. Even
 *      when letting it run up to larger N sizes, the graph doesn't seem to split
 *      apart, only slightly before joining back again, probably caused by tasks
 *      from the OS. Maybe these CPU's are too old to take advantage of some
 *      loop unrolling optimisations that would come with one implementation
 *      or the other. At least this remove the worry of using the right one
 *      and leaves choice to preference.
 *  Heap findings:
 *          When testing heap building using bottom up and top down approaches,
 *      on the average case their complexity looks very close, both looking like
 *      O(log n), while top down doing close to twice the operations. On worst
 *      case, the real difference begins to show. Bottom up keeps O(log n)
 *      complexity, while top down goes to O(n) complexity. Bottom up is the
 *      clear winner, which is why it is used in heapsort.
 */

#include <iostream>
#include "Profiler.h"
#include "bubble.h"

constexpr char buildHeapComparisonAverage[] = "1_build_heap_comparison_average_case";
constexpr char buildHeapComparisonWorst[] = "1_build_heap_comparison_worst_case";

constexpr char buildHeapAssign[] = "build_heap_assign_comparison";
constexpr char buildHeapCompare[] = "build_heap_compare_comparison";

constexpr char topDown[] = "top_down";
constexpr char bottomUp[] = "bottom_up";

constexpr char sinkAssign[] = "bu_sink_assign";
constexpr char sinkCompare[] = "bu_sink_compare";

constexpr char swimAssign[] = "td_swim_assign";
constexpr char swimCompare[] = "td_swim_compare";

static int left(const int i) {
    return 2 * i;
}

static int right(const int i) {
    return 2 * i + 1;
}

static int parent(const int i) {
    return i / 2;
}

//templating because the compiler wouldn't convert a lambda to a function pointer if it had captures
template<typename Compare>
static void heapSink(int arr[], const int size, const int n, const int i, Compare compare) {
    const int leftI = left(i);
    const int rightI = right(i);
    int swapI = i;
    if (leftI <= size && compare(arr[leftI], arr[swapI])) {
        swapI = leftI;
    }
    if (rightI <= size && compare(arr[rightI], arr[swapI])) {
        swapI = rightI;
    }
    if (swapI != i) {
        std::swap(arr[i], arr[swapI]);
        profiler.countOperation(sinkAssign, n, 3);
        heapSink(arr, size, n, swapI, compare);
    }
}


static void maxHeapSink(int arr[], const int size, const int n, const int i) {
    heapSink(arr, size, n, i, [n](const int first, const int second) {
        profiler.countOperation(sinkCompare, n);
        return first > second;
    });
}

static void minHeapSink(int arr[], const int size, const int n, const int i) {
    heapSink(arr, size, n, i, [n](const int first, const int second) {
        profiler.countOperation(sinkCompare, n);
        return first < second;
    });
}

static void buildMaxHeapBottomUp(int arr[], const int size) {
    profiler.countOperation(sinkAssign, size, 0);
    //without this the operation never gets created and the profiler crashes when creating the groups
    for (int i = size / 2; i > 0; i--) {
        maxHeapSink(arr, size, size, i);
    }
}

static void buildMinHeapBottomUp(int arr[], const int size) {
    profiler.countOperation(sinkAssign, size, 0);
    //without this the operation never gets created and the profiler crashes when creating the groups
    for (int i = size / 2; i > 0; i--) {
        minHeapSink(arr, size, size, i);
    }
}

//templating because the compiler wouldn't convert a lambda to a function pointer if it had captures
template<typename Compare>
static void heapSwim(int arr[], const int n, int i, Compare compare) {
    while (i > 1 && compare(arr[parent(i)], arr[i])) {
        std::swap(arr[parent(i)], arr[i]);
        profiler.countOperation(swimAssign, n, 3);
        i = parent(i);
    }
}

static void maxHeapSwim(int arr[], const int n, const int i) {
    heapSwim(arr, n, i, [n](const int first, const int second) {
        profiler.countOperation(swimCompare, n);
        return first < second;
    });
}

static void minHeapSwim(int arr[], const int n, const int i) {
    heapSwim(arr, n, i, [n](const int first, const int second) {
        profiler.countOperation(swimCompare, n);
        return first > second;
    });
}

static void buildMaxHeapTopDown(int arr[], const int size) {
    profiler.countOperation(swimAssign, size, 0);
    //without this the operation never gets created and the profiler crashes when creating the groups
    for (int i = 2; i <= size; i++) {
        maxHeapSwim(arr, size, i);
    }
}

static void buildMinHeapTopDown(int arr[], const int size) {
    profiler.countOperation(swimAssign, size, 0);
    //without this the operation never gets created and the profiler crashes when creating the groups
    for (int i = 2; i <= size; i++) {
        minHeapSwim(arr, size, i);
    }
}

static void heapsort(int arr[], const int n) {
    buildMaxHeapBottomUp(arr, n);
    int heapSize = n;
    for (int i = n; i >= 2; i--) {
        std::swap(arr[1], arr[i]);
        heapSize--;
        maxHeapSink(arr, heapSize, n, 1);
    }
}

static void checkHeapCorrectness() {
    constexpr int n = 8;
    constexpr int size = n - 1;
    int exampleArr[] = {0, 5, 1, 4, 2, 8, 7, 3};
    int tempArr[n] = {};

    std::cout << "\n=====================================================\n";
    std::cout << "\nChecking heap algorithms correctness!\n\nExample array: ";
    printArray(exampleArr, n);

    std::cout << "\nBuilding max heap bottom up test\noutput: ";
    CopyArray(tempArr, exampleArr, n);
    buildMaxHeapBottomUp(tempArr, size);
    printArray(tempArr, n);

    std::cout << "\nBuilding min heap bottom up test\noutput: ";
    CopyArray(tempArr, exampleArr, n);
    buildMinHeapBottomUp(tempArr, size);
    printArray(tempArr, n);

    std::cout << "\nBuilding max heap top down test\noutput: ";
    CopyArray(tempArr, exampleArr, n);
    buildMaxHeapTopDown(tempArr, size);
    printArray(tempArr, n);

    std::cout << "\nBuilding min heap top down test\noutput: ";
    CopyArray(tempArr, exampleArr, n);
    buildMinHeapTopDown(tempArr, size);
    printArray(tempArr, n);

    std::cout << "\nHeapsort test\noutput: ";
    CopyArray(tempArr, exampleArr, n);
    heapsort(tempArr, size);
    printArray(tempArr, n);
    if (IsSorted(tempArr, n)) {
        std::cout << "It's sorted!\n";
    } else {
        std::cout << "Not sorted!\n";
    }
}

static void profileBuildHeap(const bool worstCase = false) {
    constexpr int maxSize = 10000;
    constexpr int numOfTests = 10;
    const int sorted = worstCase ? ASCENDING : UNSORTED;
    int arr[maxSize + 1] = {}, tempArr[maxSize + 1] = {};
    std::cout << "profiling bottom up vs top down heap building num of ops\n";
    for (int n = 100; n <= maxSize; n += 100) {
        std::cout << "n: " << n << '\n';
        for (int test = 0; test < numOfTests; test++) {
            FillRandomArray(arr + 1, n, 10, 50000, false, sorted);
            CopyArray(tempArr + 1, arr + 1, n);
            buildMaxHeapBottomUp(arr, n);
            buildMaxHeapTopDown(tempArr, n);
        }
    }

    profiler.divideValues(swimAssign, numOfTests);
    profiler.divideValues(swimCompare, numOfTests);
    profiler.divideValues(sinkAssign, numOfTests);
    profiler.divideValues(sinkCompare, numOfTests);

    profiler.addSeries(bottomUp, sinkAssign, sinkCompare);
    profiler.addSeries(topDown, swimAssign, swimCompare);

    profiler.createGroup(buildHeapAssign, sinkAssign, swimAssign);
    profiler.createGroup(buildHeapCompare, sinkCompare, swimCompare);

    const char *groupName = worstCase ? buildHeapComparisonWorst : buildHeapComparisonAverage;

    profiler.createGroup(groupName, bottomUp, topDown);
}

static void heapProfiling() {
    profiler.reset("Build_Heap_Bottom_Up_vs_Top_Down_Average_Case");
    profileBuildHeap();
    profiler.reset("Build_Heap_Bottom_Up_vs_Top_Down_Worst_Case");
    profileBuildHeap(true);
    profiler.reset();
}

int main() {
    heapProfiling();
    bubbleSortProfiling();
    checkHeapCorrectness();
    checkBubbleCorrectness();
    return 0;
}
