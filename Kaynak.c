#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h> //qsort i used
#include <math.h>
#include <string.h>

#define MAX_BUFFER_SIZE 127

typedef struct BHNode {
    double value;
    int degree;
    char PID[5];
    int executiontime;
    int remaintime;
    int arrivetime;
    int lastqeueuentrytime;
    int TWT;
    struct BHNode* child;
    struct BHNode* parent;
    struct BHNode* sibling;
} BHNode;

typedef struct BinomialHeap {
    BHNode* head;
} BinomialHeap;

typedef struct InputData {
    int e;
    int tArr;
    char pid[10];
} InputData;

/* -------- Node and Heap Initialization -------- */

BHNode* nodeInit(double value, char* pid, int exectime, int arrtime, int remtime, int oldwaitvalue, int currtime)
{
    BHNode* node;

    node = (BHNode*)malloc(sizeof(BHNode));
    if (node == NULL)
        return NULL;

    strcpy(node->PID, pid);
    node->executiontime = exectime;
    node->remaintime = remtime;
    node->arrivetime = arrtime;

    node->TWT = oldwaitvalue;
    node->lastqeueuentrytime = currtime;

    node->value = value;
    node->degree = 0;
    node->child = NULL;
    node->parent = NULL;
    node->sibling = NULL;

    return node;
}

int compareProcesses(const void* a, const void* b) {
    InputData* p1 = (InputData*)a;
    InputData* p2 = (InputData*)b;

    if (p1->tArr != p2->tArr) {
        return p1->tArr - p2->tArr;
    }
    return 0;
}

BinomialHeap* heapInit(void)
{
    BinomialHeap* heap;

    heap = (BinomialHeap*)malloc(sizeof(BinomialHeap));
    if (heap == NULL)
        return NULL;

    heap->head = NULL;
    return heap;
}

/* -------- Heap Merge (by degree) -------- */

BHNode* heapMerge(BinomialHeap* heap1, BinomialHeap* heap2)
{
    BHNode* head;
    BHNode* tail;
    BHNode* h1It;
    BHNode* h2It;

    if (heap1->head == NULL)
        return heap2->head;
    if (heap2->head == NULL)
        return heap1->head;

    h1It = heap1->head;
    h2It = heap2->head;

    if (h1It->degree <= h2It->degree) {
        head = h1It;
        h1It = h1It->sibling;
    }
    else {
        head = h2It;
        h2It = h2It->sibling;
    }

    tail = head;

    while (h1It != NULL && h2It != NULL) {
        if (h1It->degree <= h2It->degree) {
            tail->sibling = h1It;
            h1It = h1It->sibling;
        }
        else {
            tail->sibling = h2It;
            h2It = h2It->sibling;
        }
        tail = tail->sibling;
    }

    tail->sibling = (h1It != NULL) ? h1It : h2It;
    return head;
}

/* -------- Heap Union -------- */

BHNode* heapUnion(BinomialHeap* original, BinomialHeap* uni)
{
    BHNode* new_head;
    BHNode* prev;
    BHNode* aux;
    BHNode* next;

    new_head = heapMerge(original, uni);
    original->head = NULL;
    uni->head = NULL;

    if (new_head == NULL)
        return NULL;

    prev = NULL;
    aux = new_head;
    next = aux->sibling;

    while (next != NULL) {
        if (aux->degree != next->degree ||
            (next->sibling != NULL &&
                next->sibling->degree == aux->degree)) {
            prev = aux;
            aux = next;
        }
        else {
            if (aux->value <= next->value) {
                aux->sibling = next->sibling;
                next->parent = aux;
                next->sibling = aux->child;
                aux->child = next;
                aux->degree++;
            }
            else {
                if (prev == NULL)
                    new_head = next;
                else
                    prev->sibling = next;

                aux->parent = next;
                aux->sibling = next->child;
                next->child = aux;
                next->degree++;
                aux = next;
            }
        }
        next = aux->sibling;
    }

    return new_head;
}

/* -------- Heap Operations -------- */

void heapInsert(BinomialHeap* heap, double value, char* pid, int exectime, int arrtime, int remtime, int oldwaitvalue, int currtime)
{
    BinomialHeap* temp;
    BHNode* node;

    node = nodeInit(value, pid, exectime, arrtime, remtime, oldwaitvalue, currtime);
    if (node == NULL)
        return;

    temp = heapInit();
    if (temp == NULL)
        return;

    temp->head = node;
    heap->head = heapUnion(heap, temp);
    free(temp);
}

void heapRemove(BinomialHeap* heap, BHNode* node, BHNode* before)
{
    BinomialHeap* temp;
    BHNode* child;
    BHNode* new_head;
    BHNode* next;

    if (node == heap->head)
        heap->head = node->sibling;
    else if (before != NULL)
        before->sibling = node->sibling;

    new_head = NULL;
    child = node->child;

    while (child != NULL) {
        next = child->sibling;
        child->sibling = new_head;
        child->parent = NULL;
        new_head = child;
        child = next;
    }

    temp = heapInit();
    if (temp == NULL)
        return;

    temp->head = new_head;
    heap->head = heapUnion(heap, temp);
    free(temp);
}

BHNode* heapMin(BinomialHeap* heap)
{
    BHNode* min;
    BHNode* min_prev;
    BHNode* curr;
    BHNode* prev;

    if (heap->head == NULL)
        return NULL;

    min = heap->head;
    min_prev = NULL;
    prev = min;
    curr = min->sibling;

    while (curr != NULL) {
        if (curr->value < min->value) {
            min = curr;
            min_prev = prev;
        }
        prev = curr;
        curr = curr->sibling;
    }

    if (min != NULL) {
        heapRemove(heap, min, min_prev);
    }
    return min;
}

void heapFree(BinomialHeap* heap)
{
    while (heapMin(heap) != NULL)
        ;
    free(heap);
}

double calculateC(int ei, int emax) {
    if (emax == 0) return 1.0;
    double term = (2.0 * ei) / (3.0 * emax);
    double expo = -1.0 * pow(term, 3);
    return 1.0 / exp(expo);
}

double getPriority(int ei, int erem, int emax, int tArr, int isFirstInsertion) {
    if (isFirstInsertion) {
        return (double)ei;
    }

    double c = calculateC(ei, emax);
    return c * erem;

}

void printHeap(BHNode* node) {
    if (node == NULL)
        return;
    printf("(%s: %.3f) ", node->PID, node->value);
    printHeap(node->child);
    printHeap(node->sibling);
}

double calculateAwt(int quantum, InputData* inputs, int totalprocesses, int emax, int decide) {
    BinomialHeap* heap = heapInit();

    int doneprocesses = 0;
    int nextprocess = 0;
    int time = 0;// current time we're in

    BHNode* cp = NULL;  //cp is current process and its only one.
    int quantumtimer = 0;
    double TSWT = 0.0; // tswt is total system wait time bc we need it for awt.

    while (doneprocesses < totalprocesses || cp != NULL || heap->head != NULL) {
        while (nextprocess < totalprocesses && inputs[nextprocess].tArr == time) {
            double prior = (double)inputs[nextprocess].e;

           if(decide)
            printf("Time %d : %s arrived.\n", time, inputs[nextprocess].pid);

            heapInsert(heap, prior, inputs[nextprocess].pid, inputs[nextprocess].e, inputs[nextprocess].tArr, inputs[nextprocess].e, 0, time);

            nextprocess++;
        }

        if (cp == NULL) {
            cp = heapMin(heap);
            if (cp != NULL) {
                int waittime = time - cp->lastqeueuentrytime;
                cp->TWT += waittime;

                quantumtimer = 0;
                if(decide)
                printf("Time %d : %s (waited so far: %d)\n", time, cp->PID, cp->TWT);

            }
        }

        if (decide) {
            printf("Time %d: ", time);

            if (cp != NULL)
                printf("allocated cpu: %s -> ", cp->PID);
            else printf("allocated cpu: IDLE -> ");

            printf("heap processes: { ");
            printHeap(heap->head);
            printf("}\n");
        }

        if (cp != NULL) {
            cp->remaintime--;
            quantumtimer++;

            if (cp->remaintime == 0) { // if finished
                if(decide)
                printf("Time %d : %s finished.\n", time + 1, cp->PID);
                TSWT += cp->TWT;
                doneprocesses++;
                free(cp);
                cp = NULL;
                quantumtimer = 0;  // everything refreshed
            }

            else if (quantumtimer == quantum) {
                if(decide)
                printf("Time %d : %s re-queueing.\n", time + 1, cp->PID);

                double newprior = getPriority(cp->executiontime,
                    cp->remaintime, emax, cp->arrivetime, 0);

                heapInsert(heap, newprior, cp->PID, cp->executiontime,
                    cp->arrivetime, cp->remaintime, cp->TWT, time + 1);

                free(cp);
                cp = NULL;
                quantumtimer = 0; //refreshing
            }
        }

        time++;
    }

    heapFree(heap);
    return TSWT / totalprocesses;
}

/* -------- Main -------- */

int main(void)
{

    InputData inputs[100];
    int count = 0;
    int quantum = 2;
    int emax = 0;

    FILE* fp = fopen("Metin.txt", "r");
    if (fp == NULL) {
        printf("error . there's no file.");
        return 1;
    }

    int inpute, inputarr;

    while (fscanf(fp, "%d,%d", &inpute, &inputarr) != EOF) {
        inputs[count].e = inpute;
        inputs[count].tArr = inputarr;
        sprintf(inputs[count].pid, "P%d", count + 1);

        if (inpute > emax)
            emax = inpute;
        count++;
    }
    fclose(fp);

    if (count == 0) {
        printf("No processes found in file.\n");
        return 0;
    }

    qsort(inputs, count, sizeof(InputData), compareProcesses);

    double bestAwt = 1000000000;
    int bestQ = 1;

    for (int q = 1; q <= 10; q++) {
        double awt = calculateAwt(q, inputs, count, emax, 0);
        if (awt < bestAwt) {
            bestAwt = awt;
            bestQ = q;
        }
    }

    printf("Selected quantum value q = %d\n", bestQ);
    printf("For the selected q value:\n");
    printf("--------------------------------------------------\n");
   
    double lastAwt = calculateAwt(bestQ, inputs, count, emax, 1);

    printf("--------------------------------------------------\n");
    printf("Average Waiting Time (AWT): %.2f\n", lastAwt);

    return 0;

    
}
