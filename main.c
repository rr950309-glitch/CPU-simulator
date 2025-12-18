#include <stdio.h>
#include <stdlib.h>
#define MAX 50

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




void initqueue_Array(Queue_Array *q,int max){//請用此函式直接在這裡宣告陣列(^u^)

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

    /*if(is_empty_Array(q)){
        q->Data[q->front] = *p;
        q->count++;
    }
    else{
        if(q->rear != q->max-1){
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
    }*/
   if(q->count == q->max){
        printf("/n/n/Array is full. Can't not wrap-around to assign q->Data[0]!!!/n/n");
        return;
    }

    q->Data[q->rear] = *p;
    q->rear = (q->rear + 1) % q->max;
    q->count++;
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

Process* dequeue_Array(Queue_Array *q){
    if(is_empty_Array(q)) return NULL;
    
    Process *t = createpro();
    *t = q->Data[q->front];

    if(q->front == q->rear){
        q->front = q->rear = 0;
        return t;
    }
    else{
        q->front++;
        q->count--;
        return t;
    }
    

}

void struct_copy(Process *des, Process *sour){
    des->pid = sour->pid;
    des->arrival_time = sour->arrival_time;
    des->burst_time = sour->burst_time;
    des->finish_time = sour->finish_time;
    des->remaining_time = sour->remaining_time;
    des->response_time = sour->response_time;
    des->start_time = sour->start_time;
    des->turnaround_time = sour->turnaround_time;
    des->waiting_time = sour->waiting_time;
}

void swap_array(Process *a1, Process *a2){
    Process *temp = createpro();
    struct_copy(temp, a1);
    struct_copy(a1, a2);
    struct_copy(a2, temp);
}

Process* schedule_npsjf_Array(Queue_Array *ready_queue, int now) {
    if(is_empty_Array(ready_queue)) return NULL;
    //從front開始
    int index, cur = ready_queue->front;
    int best_index = cur;
    int mintime = ready_queue->Data[cur].burst_time;

    for(int c=1; c < ready_queue->count; c++) {
        //找出時間為now時ready_queue中burst_time最小的Node
        if((cur+1)%MAX == 0){
            cur = -1;
        }
        if (ready_queue->Data[cur+1].burst_time < mintime) {
            mintime = ready_queue->Data[cur+1].burst_time;
            best_index = cur+1;
        }
        cur++;
    }

    // 把best 放到queue 中front 的位置
    //當best不在queue裡front的位置
    if (best_index != ready_queue->front) {
        swap_array(&ready_queue->Data[best_index],&ready_queue->Data[ready_queue->front]);
    }
    return dequeue_Array(ready_queue);
}


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

Process* schedule_fcfs_Array(Queue_Array *ready_queue, int now){
    return dequeue_Array(ready_queue);
}

Process* schedule_rr_Array(Queue_Array *ready_queue, int now) {
    return dequeue_Array(ready_queue);
}

Process* schedule_psjf_Array(Queue_Array *ready_queue,int now){
    if(is_empty_Array(ready_queue)) return NULL;

    int cur_index = ready_queue->front;
    int best_index = cur_index;
    int min_remaining_time = ready_queue->Data[cur_index].remaining_time;

    //找出時間為now時ready_queue中remaining_time最小的Process
    for(int i = 1;i < ready_queue->count;i++){

        if((cur_index + 1) % MAX == 0)
            cur_index = -1;
        
        if(ready_queue->Data[cur_index + 1].remaining_time < min_remaining_time){
            min_remaining_time = ready_queue->Data[cur_index + 1].remaining_time;
            best_index = cur_index + 1;
        }

        cur_index++;
    
    }

    // 把best 放到queue 中front 的位置
    //best不在queue裡front的位置
    if(best_index != ready_queue->front)
        swap_array(&ready_queue->Data[best_index],&ready_queue->Data[ready_queue->front]);
    
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
            
            /*If the scheduler is Non-preemptive SJF, check if the remaining time of process that is running is shorter than the candidate.*/
            if(scheduler == schedule_psjf_LL && running != NULL && !is_empty_LL(ready_queue)){
                Process *candidate = scheduler(ready_queue, now);

                if (candidate != NULL && candidate->remaining_time < running->remaining_time) {
                    // preempt
                    enqueue_LL(ready_queue, running);
                    running = candidate;

                    if (running->start_time == -1) {
                        running->start_time = now;
                        running->response_time = now - running->arrival_time;
                    }
                    
                }
                else if(candidate != NULL){
                    // 不 preempt，把 candidate 放回去
                    enqueue_LL(ready_queue, candidate);
                }
            }

            running->remaining_time--;

            if (running->remaining_time == 0) {
                running->finish_time = now + 1;
                running->turnaround_time = running->finish_time - running->arrival_time;
                running->waiting_time = running->turnaround_time - running->burst_time;
                ready_queue->memo->waiting_time += running->waiting_time;
                ready_queue->memo->turnaround_time += running->turnaround_time;
    
                running = NULL; // process done
                time_quantum = 20;
            }
            
            if(scheduler == schedule_rr_LL && running !=NULL){
                time_quantum--;
                if(time_quantum==0){
                    Process* temp=running;
                    running = NULL;
                    enqueue_LL(ready_queue,temp);
                    time_quantum=20;
                }
            }

        }
        now++;
    }
}

void simulate_Array(Queue_Array *job_queue, Queue_Array *ready_queue, scheduler_func_Array scheduler) {
    
    int now = 0;
    int time_quantum = 20;//for rr scheduler
    Process *running = NULL;

    while (!is_empty_Array(job_queue) || !is_empty_Array(ready_queue) || running != NULL) {

        // 1. 有 arrival 的 process -> ready queue
        while (!is_empty_Array(job_queue) && job_queue->Data[job_queue->front].arrival_time <= now) {
            enqueue_Array(ready_queue, dequeue_Array(job_queue));
        }

        // 2. 若 CPU idle，從 scheduler 選一個 process
        if (running == NULL && !is_empty_Array(ready_queue)) {
            running = scheduler(ready_queue, now);
            if(running->start_time == -1){
                running->start_time = now;
                running->response_time = running->start_time-running->arrival_time;
            }
        }

        // 3. 執行 1 單位時間
        if (running != NULL) {
            
            /*If the scheduler is Non-preemptive SJF, check if the remaining time of process that is running is shorter than the candidate.*/
            if(scheduler == schedule_psjf_Array && running != NULL && !is_empty_Array(ready_queue)){
                Process *candidate = scheduler(ready_queue, now);

                if (candidate != NULL && candidate->remaining_time < running->remaining_time) {
                    // preempt
                    enqueue_Array(ready_queue, running);
                    running = candidate;

                    if (running->start_time == -1) {
                        running->start_time = now;
                        running->response_time = now - running->arrival_time;
                    }
                    
                }
                else if(candidate != NULL){
                    // 不 preempt，把 candidate 放回去
                    enqueue_Array(ready_queue, candidate);
                }
            }

            running->remaining_time--;

            if (running->remaining_time == 0) {
                running->finish_time = now + 1;
                running->turnaround_time = running->finish_time - running->arrival_time;
                running->waiting_time = running->turnaround_time - running->burst_time;
                ready_queue->memo->waiting_time += running->waiting_time;
                ready_queue->memo->turnaround_time += running->turnaround_time;
    
                running = NULL; // process done
                time_quantum = 20;
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

void load_processes_from_file_LL(Queue_LL *job){
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

void load_processes_from_file_Array(Queue_Array *job){
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
        enqueue_Array(job , n);
        id++;
        n = createpro();
    }
    fclose(fp);
    return;
}

int main() {
    Queue_LL *job_queue_LL = malloc(sizeof(Queue_LL));
    job_queue_LL->front = NULL;
    job_queue_LL->rear = NULL;
    job_queue_LL->memo = NULL;

    Queue_LL *ready_queue_LL = malloc(sizeof(Queue_LL));
    ready_queue_LL->front = NULL;
    ready_queue_LL->rear = NULL;
    ready_queue_LL->memo = createpro();

    Queue_Array *job_queue_array = malloc(sizeof(Queue_Array));
    initqueue_Array(job_queue_array, MAX);
    job_queue_array->memo = NULL;

    Queue_Array *ready_queue_array = malloc(sizeof(Queue_Array));
    initqueue_Array(ready_queue_array, MAX);
    ready_queue_array->memo = createpro();

    
    printf("Choose algorithm: 1=FCFS_LL, 2=SJF_LL, 3=RR_LL, 4=Preemptive SJF_LL ; 5=FCFS_Array, 6=SJF_Array, 7=RR_Array, 8=Preemptive SJF_Array\n");
    int option;
    scanf("%d", &option);

    scheduler_func_LL scheduler_LL;
    scheduler_func_Array scheduler_Array;
    if (option == 1) scheduler_LL = schedule_fcfs_LL;
    else if (option == 2) scheduler_LL = schedule_npsjf_LL;
    else if (option == 3) scheduler_LL = schedule_rr_LL;
    else if (option == 4) scheduler_LL = schedule_psjf_LL;
    else if (option == 5) scheduler_Array = schedule_fcfs_Array;
    else if (option == 6) scheduler_Array = schedule_npsjf_Array;
    else if (option == 7) scheduler_Array = schedule_rr_Array;
    else if (option == 8) scheduler_Array = schedule_psjf_Array;

    if(0 < option && option < 5){
        load_processes_from_file_LL(job_queue_LL);
        simulate_LL(job_queue_LL, ready_queue_LL, scheduler_LL);
        printf("total waiting time: %d\n",ready_queue_LL->memo->waiting_time);
        printf("total turnaround time: %d\n", ready_queue_LL->memo->turnaround_time);
    }
    else{ 
        load_processes_from_file_Array(job_queue_array);
        simulate_Array(job_queue_array, ready_queue_array, scheduler_Array);
        printf("total waiting time: %d\n",ready_queue_array->memo->waiting_time);
        printf("total turnaround time: %d\n", ready_queue_array->memo->turnaround_time);
    }
    
    return 0;
}