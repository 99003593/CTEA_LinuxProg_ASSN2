#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <sys/sysinfo.h>

#define ARR_SIZ (1000000000)
#define SEED    (1033u)

typedef struct
{
    int *arr;
    int len;
} work_data_t;

typedef struct
{
    long sum;
} work_result_t;

typedef struct
{
    work_data_t wd;
    work_result_t wr;
} work_state_t;


int get_optimal_thread_count()
{
    return get_nprocs();
}

void init_work_data(work_data_t *p_wd, int len)
{
    unsigned int seed = SEED;
    p_wd->len = len;
    p_wd->arr = (int*)malloc(sizeof(int)*p_wd->len);
    for(int i = 0; i < p_wd->len; i++)
    {
        p_wd->arr[i] = rand_r(&seed);
    }
}

work_state_t* parallize_work_state(work_state_t *p_ws, int *units)
{
    work_state_t *pa_wsp;
    int n, quanta, tlen = p_ws->wd.len;

    if (units != NULL)
        n = *units;

    if (n == 0)
        n = get_optimal_thread_count();

    if (units != NULL)
        *units = n;

    // Allocate parallel work_state array
    pa_wsp = (work_state_t*)malloc(sizeof(work_state_t)*n);
    memset(pa_wsp, 0, sizeof(work_state_t)*n);

    // Remap array segements to sub work data structures
    quanta = tlen / n;
    for(int i = 0; i < n - 1; i++)
    {
        pa_wsp[i].wd.len = quanta;
        pa_wsp[i].wd.arr = &p_ws->wd.arr[i*quanta];
        tlen -= quanta;
    }
    pa_wsp[n-1].wd.len = tlen;
    pa_wsp[n-1].wd.arr = &p_ws->wd.arr[p_ws->wd.len - tlen];

    return  pa_wsp;
}

void deparallize_work_state(work_state_t *p_ws, work_state_t *pa_wsp, int *units)
{
    int n = *units;
    long sum = 0;

#ifdef DEBUG
    printf("Thread Level:\n");
#endif
    // Accumulate results
    for(int i = 0; i < n; i++)
    {
#ifdef DEBUG
        printf("\t%d\t%d\t%p\t%ld\n", i, pa_wsp[i].wd.len, pa_wsp[i].wd.arr, pa_wsp[i].wr.sum);
#endif
        sum += pa_wsp[i].wr.sum;
    }

    // Save Results
    p_ws->wr.sum = sum;

    // free parallel work_state array
    free(pa_wsp);
}

void sum(work_state_t *p_ws)
{
    long sum = 0;

    // Calculate sum
    for(int i = 0; i < p_ws->wd.len; i++)
    {
        sum += p_ws->wd.arr[i];
    }

    // Save results
    p_ws->wr.sum = sum;
}

void* sum_thread(void* arg)
{
    work_state_t *p_ws = (work_state_t *)arg;
    sum(p_ws);
    // pthread_exit(NULL);
}

void parallel_sum(work_state_t *p_ws, int *work_units)
{
    work_state_t *pa_wsp = parallize_work_state(p_ws, work_units);
    // Allocate thread handles
    pthread_t *thandles = (pthread_t*)malloc(sizeof(pthread_t)*(*work_units));
    // Create threads
    for(int i = 0; i < *work_units; i++)
        pthread_create(&thandles[i], NULL, sum_thread, &pa_wsp[i]);
    // Wait for threads to complete
    for(int i = 0; i < *work_units; i++)
        pthread_join(thandles[i], NULL);
    // Accumulate results
    deparallize_work_state(p_ws, pa_wsp, work_units);
    // Free thread handles
    free(thandles);
}

int main(int argc, char const **argv)
{
    work_state_t ws;
    int threads_count = 0;

    // Initialize Sample data
    init_work_data(&ws.wd, ARR_SIZ);
    printf("Finished Initialization\n");

    // Calculate sum parallely
    parallel_sum(&ws, &threads_count);

    // Print result
    printf("Total:\n\t%d\t%d\t%p\t%ld\n", threads_count, ws.wd.len, ws.wd.arr, ws.wr.sum);

    // Deinitialize Sample data
    free(ws.wd.arr);
    ws.wd.len = 0;

    return 0;
}
