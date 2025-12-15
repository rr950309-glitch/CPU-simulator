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

typedef struct node {
    Process *proc;
    struct node *next;
} Node;

typedef struct queue {
    Node *front;
    Process *memo;
    Node *rear;
} Queue_LL;

typedef struct queuearray{
    int count;
    int front;
    Process *memo;
    int rear;
    int max;
    Process *Data;
}Queue_Array;

int is_empty_LL(Queue_LL *q){

    if(q->front == NULL)
        return 1;

    else
        return 0;

}

void enqueue_LL(Queue_LL *q, Process *p){

    Node *node = (Node *)malloc(sizeof(Node));
    node->proc = p;
    node->next = NULL;

    if(is_empty_LL(q)){
        q->front = node;
        q->rear = node;
    }else{
        q->rear->next = node;
        q->rear = node;
    }
}

Process* dequeue_LL(Queue_LL *q){

    if(is_empty_LL(q)) return NULL;
    
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




void initArray(Queue_Array *q,int max){//請用此函式直接在這裡宣告陣列(^u^)

    q->count = 0;
    q->front = 0;
    q->rear = 0;
    q->max = max;
    q->Data = (Process *)malloc(sizeof(Process) * max);

}

int is_empty_Array(Queue_Array *q){

    if(q->count == 0)
        return 1;

    else
        return 0;

}

void enqueue_Array(Queue_Array *q, Process *p){

    if(is_empty_Array(q)){
        q->Data[q->front] = *p;
        q->count++;
    }
    else{
        if(q->rear != q->max){
            q->rear++;
            q->Data[q->rear] = *p;
            q->count++;
        }
        else{
            if(q->count == q->max)//如果陣列已滿：不能再塞元素到q->Data[0]
                printf("/n/n/Array is full. Can't not wrap-around to assign q->Data[0]!!!/n/n");
            else{//wrap-around
                q->rear = 0;
                q->Data[q->rear] = *p;
                q->count++;
            }
        }
    }
}

Process* dequeue_Array(Queue_Array *q){

    if(is_empty_Array(q)) return NULL;
    
    else{
        if(q->front == q->rear){
            Process *t;
            *t = q->Data[q->front];
            q->front = q->rear = 0;
            return t;
        }
        else{
            int temp;
            temp = q->front;
            q->front++;
            q->count--;
            Process *t;
            *t = q->Data[temp];
            return t;
        }
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

void arrcopy(Process *des, Process *sour){

}

/*
void shiftArray(Queue_Array *ready_queue, int target){
    Process temp;
    arrcopy(&temp, &(ready_queue->Data[target]));
    for(int i = target; i<=ready_queue->front; i++){
        arrcopy(&(ready_queue->Data[i]), &(ready_queue->Data[i-1]))
    }
    arrcopy()
}
*/

/*
Process* schedule_npsjf_Array(Queue_Array *ready_queue, int now) {
    if(is_empty_Array(ready_queue)) return NULL;
    int cur = ready_queue->front;

    int best_prev = 0, prev = 0, best = cur, c = 0;
    int min = ready_queue->Data[cur].burst_time;

    for(int c=0; c < ready_queue->count; c++) {
        //找出時間為now時ready_queue中burst_time最小的Node
        if (ready_queue->Data[cur].burst_time < min) {
            min = ready_queue->Data[cur].burst_time;
            best = cur;
            best_prev = prev;
        }
        prev = cur;
        cur++;
    }

    // 把best 放到queue 中front 的位置
    //best不在queue裡front的位置
    if (best != ready_queue->front) {
        best_prev->next = best->next;
        if (ready_queue->rear == best) ready_queue->rear = best_prev;
        best->next = ready_queue->front;
        ready_queue->front = best;
    }
    return dequeue_Array(ready_queue);
}*/


typedef Process* (*scheduler_func_LL)(Queue_LL *ready_queue, int now);
typedef Process* (*scheduler_func_Array)(Queue_Array *ready_queue, int now);

Process* schedule_fcfs_LL(Queue_LL *ready_queue, int now) {
    return dequeue_LL(ready_queue);
}

Process* schedule_npsjf_LL(Queue_LL *ready_queue, int now) {
    if(is_empty_LL(ready_queue)) return NULL;

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
    return dequeue_LL(ready_queue);
}

Process* schedule_rr_LL(Queue_LL *ready_queue, int now) {
    return dequeue_LL(ready_queue);
}

Process* schedule_psjf_LL(Queue_LL *ready_queue, int now){
    if(is_empty_LL(ready_queue)) return NULL;

    Node *cur = ready_queue->front;
    Node *best_prev = NULL, *prev = NULL, *best = cur;
    int min = cur->proc->remaining_time;

    while (cur != NULL) {
        //找出時間為now時ready_queue中remaining_time最小的Node
        if (cur->proc->remaining_time < min) {
            min = cur->proc->remaining_time;
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
        if (ready_queue->rear == best) {
            ready_queue->rear = best_prev;
        }
        best->next = ready_queue->front;
        ready_queue->front = best;
    }
    
    return dequeue_LL(ready_queue);
}

Process* schedule_rr_Array(Queue_Array *ready_queue, int now){
    return dequeue_Array(ready_queue);
}

void simulate_LL(Queue_LL *job_queue, Queue_LL *ready_queue, scheduler_func_LL scheduler) {
    
    int now = 0;
    int time_quantum = 20;//for rr scheduler
    Process *running = NULL;

    while (!is_empty_LL(job_queue) || !is_empty_LL(ready_queue) || running != NULL) {

        // 1. 有 arrival 的 process -> ready queue
        while (!is_empty_LL(job_queue) && job_queue->front->proc->arrival_time <= now) {
            enqueue_LL(ready_queue, dequeue_LL(job_queue));
        }

        // 2. 若 CPU idle，從 scheduler 選一個 process
        if (running == NULL && !is_empty_LL(ready_queue)) {
            running = scheduler(ready_queue, now);
            if(running->start_time == -1){
                running->start_time = now;
                running->response_time = running->start_time-running->arrival_time;
            }
        }

        // 3. 執行 1 單位時間
        if (running != NULL) {
            running->remaining_time--;
            if (running->remaining_time == 0) {
                running->finish_time = now + 1;
                running->turnaround_time = running->finish_time - running->arrival_time;
                running->waiting_time = running->turnaround_time - running->burst_time;
                ready_queue->memo->waiting_time += running->waiting_time;
    
                running = NULL; // process done
                time_quantum = 20;
            }
            if(scheduler == schedule_rr_LL && running !=NULL){
                time_quantum--;
                if(time_quantum==0){
                    running->arrival_time=now+1;
                    Process* temp=running;
                    running = NULL;
                    enqueue_LL(ready_queue,temp);
                    time_quantum=20;
                }
            }
            if(scheduler == schedule_psjf_LL && running != NULL && !is_empty_LL(ready_queue)){
                Process *candidate = ready_queue->front->proc;

                if (candidate->remaining_time < running->remaining_time) {
                    // preempt
                    enqueue_LL(ready_queue, running);
                    running = scheduler(ready_queue, now);

                    if (running->start_time == -1) {
                        running->start_time = now;
                        running->response_time = now - running->arrival_time;
                    }
                    else {
                        // 不 preempt，把 candidate 放回去
                        enqueue_LL(ready_queue, candidate);
                    }
                }
            }
        }
        now++;
    }
}
/*
void simulate_Array(Queue_Array *job_queue, Queue_Array *ready_queue, scheduler_func_LL scheduler) {
    
    int now = 0;
    int time_quantum = 20;//for rr scheduler
    Process *running = NULL;

    while (!is_empty_Array(job_queue) || !is_empty_Array(ready_queue) || running != NULL) {

        // 1. 有 arrival 的 process -> ready queue
        while (!is_empty_Array(job_queue) && job_queue->front->arrival_time <= now) {
            enqueue_Array(ready_queue, dequeue_Array(job_queue));
        }

        // 2. 若 CPU idle，從 scheduler 選一個 process
        if (running == NULL && !is_empty_Array(ready_queue)) {
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
            if(scheduler == schedule_rr_Array && running !=NULL){
                time_quantum--;
                if(time_quantum==0){
                    Process* temp=running;
                    running = NULL;
                    enqueue_Array(ready_queue,temp);
                    time_quantum=20;
                }
            }
        }
        now++;
    }
}  
    */

void load_processes_from_file(Queue_LL *job){
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
        enqueue_LL(job , n);
        id++;
        n = createpro();
    }
    fclose(fp);
    return;
}

int main() {
    Queue_LL *job_queue = malloc(sizeof(Queue_LL));
    job_queue->front = NULL;
    job_queue->rear = NULL;
    job_queue->memo = NULL;

    Queue_LL *ready_queue = malloc(sizeof(Queue_LL));
    ready_queue->front = NULL;
    ready_queue->rear = NULL;
    ready_queue->memo = createpro();

    load_processes_from_file(job_queue);
    
    printf("Choose algorithm: 1=FCFS, 2=SJF, 3=RR, 4=Preemptive SJF\n");
    int option;
    scanf("%d", &option);

    scheduler_func_LL scheduler_LL;
    scheduler_func_Array scheduler_Array;
    if (option == 1) scheduler_LL = schedule_fcfs_LL;
    else if (option == 2) scheduler_LL = schedule_npsjf_LL;
    else if (option == 3) scheduler_LL = schedule_rr_LL;
    else if (option == 4) scheduler_LL = schedule_psjf_LL;
    else if (option == 7) scheduler_Array = schedule_rr_Array;

    simulate_LL(job_queue, ready_queue, scheduler_LL);

    printf("total waiting time: %d\n",ready_queue->memo->waiting_time);
    return 0;
}
