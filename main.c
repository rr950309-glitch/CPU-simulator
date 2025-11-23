#include <stdio.h>

typedef struct Process {
    int pid;            // process ID
    int arrival_time;   // 何時進入系統
    int burst_time;     // 需要多少 CPU 時間
    int remaining_time; // RR, Preemptive SJF 用
    int priority;       // 若有 Priority Scheduling
    int start_time;
    int finish_time;
    int waiting_time;
    int turnaround_time;
} Process;

typedef struct Node {
    Process *proc;
    struct Node *next;
} Node;

typedef struct Queue {
    Node *front;
    Node *rear;
} Queue;

int is_empty(Queue *q){

    if(q->front == NULL)
        return 1;

    else
        return 0;

}

void enqueue(Queue *q, Process *p){

    Node *node = (Node *)malloc(sizeof(Node));
    node->proc = p;
    node->next = NULL;

    if(is_empty(q)){

        q->front = node;
        q->rear = node;

    }

    else{

        q->rear->next = node;
        q->rear = node;

    }

}

Process* dequeue(Queue *q){

    if(is_empty(q))
        return NULL;
        
    else{

        Queue *temp = NULL;
        temp = q->front;

        q->front = q->front->next;
        free(q->front);

        return q->front->proc;

    }

}


typedef Process* (*scheduler_func)(Queue *ready_queue, int now);

Process* schedule_fcfs(Queue *ready_queue, int now) {
    return dequeue(ready_queue);
}

Process* schedule_npsjf(Queue *ready_queue, int now) {
    Node *cur = ready_queue->front;
    Node *best_prev = NULL, *best = cur;
    int min = cur->proc->burst_time;

    while (cur != NULL) {
        //找出時間為now時ready_queue中burst_time最小的Node
        if (cur->next->proc->burst_time < min) {
            min = cur->next->proc->burst_time;
            best_prev = cur;
            best = cur->next;
        }
        cur = cur->next;
    }

    // 把best 放到queue 中front 的位置
    //best不在queue裡front的位置
    if (best != ready_queue->front) {
        if (ready_queue->rear == best) ready_queue->rear = best_prev;
        best_prev->next = best->next;
        best->next = ready_queue->front;
        ready_queue->front = best;
    }

    return dequeue(ready_queue);
}

Process* schedule_rr(Queue *ready_queue, int now) {
    return dequeue(ready_queue);
}

void simulate(Queue *job_queue, scheduler_func scheduler) {
    Queue ready_queue = {0};
    int now = 0;
    Process *running = NULL;

    while (!is_empty(job_queue) || !is_empty(&ready_queue) || running != NULL) {

        // 1. 有 arrival 的 process -> ready queue
        while (!is_empty(job_queue) && job_queue->front->proc->arrival_time <= now) {
            enqueue(&ready_queue, dequeue(job_queue));
        }

        // 2. 若 CPU idle，從 scheduler 選一個 process
        if (running == NULL && !is_empty(&ready_queue)) {
            running = scheduler(&ready_queue, now);
            if (running->start_time == -1) running->start_time = now;
        }

        // 3. 執行 1 單位時間
        if (running != NULL) {
            running->remaining_time--;
            if (running->remaining_time == 0) {
                running->finish_time = now + 1;
                running = NULL; // process done
            }
        }

        now++;
    }
}

int main() {
    Queue job_queue = {0};

    load_processes_from_file("input.txt", &job_queue);

    printf("Choose algorithm: 1=FCFS, 2=SJF, 3=RR\n");
    int option;
    scanf("%d", &option);

    scheduler_func scheduler;
    if (option == 1) scheduler = schedule_fcfs;
    else if (option == 2) scheduler = schedule_npsjf;
    else scheduler = schedule_rr;

    simulate(&job_queue, scheduler);

    return 0;
}
