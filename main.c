#include <stdio.h>
#include <stdlib.h>

typedef struct Process {
    int pid;            // process ID
    int arrival_time;   // 何時進入系統
    int burst_time;     // 需要多少 CPU 時間
    int remaining_time; // RR, Preemptive SJF 用
    int response_time;
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
    Process *memo;
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
    }else{
        q->rear->next = node;
        q->rear = node;
    }
}

Process* dequeue(Queue *q){

    if(is_empty(q)) return NULL;
    
    Node *temp = NULL;
    Process *item = NULL;
    temp = q->front;
    item = q->front->proc;

    if(q->front == q->rear){
        q->front = q->front->next;
        q->rear = NULL;
        free(temp);
        return item;
    }else{
        q->front = q->front->next;
        free(temp);
        return item;
    }

}

Process *createpro(){
    Process *temp = malloc(sizeof(Process));
    temp->pid = 0;
    temp->arrival_time = 0;
    temp->burst_time = 0;
    temp->finish_time = 0;
    temp->remaining_time = 0;
    temp->response_time = 0;
    temp->start_time = -1;
    temp->turnaround_time = 0;
    temp->waiting_time = 0;
    return temp;
}

typedef Process* (*scheduler_func)(Queue *ready_queue, int now);

Process* schedule_fcfs(Queue *ready_queue, int now) {
    return dequeue(ready_queue);
}

Process* schedule_npsjf(Queue *ready_queue, int now) {
    if(is_empty(ready_queue)) return NULL;

    Node *cur = ready_queue->front;
    Node *best_prev = NULL, *prev = NULL, *best = cur;
    int min = cur->proc->burst_time;

    while (cur != NULL) {
        //找出時間為now時ready_queue中burst_time最小的Node
        if (cur->proc->burst_time < min) {
            min = cur->proc->burst_time;
            best = cur;
            best_prev = prev;
        }
        prev = cur;
        cur = cur->next;
    }

    // 把best 放到queue 中front 的位置
    //best不在queue裡front的位置
    if (best != ready_queue->front) {
        best_prev->next = best->next;
        if (ready_queue->rear == best) ready_queue->rear = best_prev;
        best->next = ready_queue->front;
        ready_queue->front = best;
    }
    return dequeue(ready_queue);
}

Process* schedule_rr(Queue *ready_queue, int now) {
    return dequeue(ready_queue);
}

void simulate(Queue *job_queue, Queue *ready_queue, scheduler_func scheduler) {
    
    int now = 0;
    int time_quantum = 20;//for rr scheduler
    Process *running = NULL;

    while (!is_empty(job_queue) || !is_empty(ready_queue) || running != NULL) {

        // 1. 有 arrival 的 process -> ready queue
        while (!is_empty(job_queue) && job_queue->front->proc->arrival_time <= now) {
            enqueue(ready_queue, dequeue(job_queue));
        }

        // 2. 若 CPU idle，從 scheduler 選一個 process
        if (running == NULL && !is_empty(ready_queue)) {
            running = scheduler(ready_queue, now);
            if(running->start_time == -1){
                running->start_time = now;
                running->waiting_time = now - running->arrival_time;
                ready_queue->memo->waiting_time = ready_queue->memo->waiting_time + running->waiting_time;
            }
        }

        // 3. 執行 1 單位時間
        if (running != NULL) {
            running->remaining_time--;
            if (running->remaining_time == 0) {
                running->finish_time = now + 1;
                time_quantum = 20;
                running = NULL; // process done
            }
            if(scheduler == schedule_rr && running !=NULL){
                time_quantum--;
                if(time_quantum==0){
                    Process* temp=running;
                    running = NULL;
                    enqueue(ready_queue,temp);
                    time_quantum=20;
                }
            }
        }
        now++;
    }
}

void load_processes_from_file(Queue *job){
    char file[10];
    FILE *fp = NULL;
    while(fp == NULL){
        printf("enter file name:");
        scanf("%s", file);
        fp = fopen(file, "r");
        if(fp == NULL){
            printf("can't opne the file, please enter again\n");
        }else{
            break;
        }
    }
     Process *n = createpro();
    int id = 1;

    while(fscanf(fp, "%d %d", &(n->arrival_time), &(n->burst_time)) != EOF){
        n->pid = id;
        n->remaining_time = n->burst_time;
        enqueue(job , n);
        id++;
        n = createpro();
    }
    return;
}

int main() {
    Queue *job_queue = malloc(sizeof(Queue));
    job_queue->front = NULL;
    job_queue->rear = NULL;
    job_queue->memo = NULL;

    Queue *ready_queue = malloc(sizeof(Queue));
    ready_queue->front = NULL;
    ready_queue->rear = NULL;
    ready_queue->memo = createpro();

    load_processes_from_file(job_queue);
    
    printf("Choose algorithm: 1=FCFS, 2=SJF, 3=RR\n");
    int option;
    scanf("%d", &option);

    scheduler_func scheduler;
    if (option == 1) scheduler = schedule_fcfs;
    else if (option == 2) scheduler = schedule_npsjf;
    else scheduler = schedule_rr;

    simulate(job_queue, ready_queue, scheduler);

    printf("toltal waiting time: %d",ready_queue->memo->waiting_time);
    return 0;
}
