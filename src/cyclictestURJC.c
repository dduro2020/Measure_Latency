#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef DEBUG
    #define DEBUG_PRINTF(...) printf("DEBUG: "__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif

#define _GNU_SOURCE
#define MIN 60
#define PERIOD 0.005
#define NUMITERATIONS (MIN/PERIOD)
#define MILLION 1e9

struct measure {
    int med_lat;
    int max_lat;
    int lats[12000];
    int id_;
};

struct thread_data {
    int id;
};

//funcion de apoyo para calcular el tiempo transcurrido
double measure_time(double begin, double stop) {
    double time = (stop - begin);
    return time;
}

void *do_work(void *arg) {
    struct thread_data *data = (struct thread_data*)arg;
    int n_id = data->id;
    struct timespec begin, stop, needed, rest = {0, 0};
    volatile unsigned long long j;
    double ttime;
    int i, lat;
    struct measure *measures = malloc(sizeof(*measures));
    
    struct sched_param param;
    param.sched_priority = 99;

    cpu_set_t cpus;
    CPU_ZERO(&cpus);
    CPU_SET(n_id, &cpus);

    measures->id_ = n_id;
    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpus)) {
        err(EXIT_FAILURE, "error: cannot set affinity in thread %d", n_id);
    }
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &param)) {
        err(EXIT_FAILURE, "error: cannot set param in thread %d", n_id);
    }
    for (i = 0; i < NUMITERATIONS; i++) {
        clock_gettime(CLOCK_MONOTONIC, &begin);
        rest.tv_nsec = ((PERIOD)*MILLION);
        DEBUG_PRINTF("Tiempo de espera:  %f ns.\n",(PERIOD)*MILLION);
        //esperamos hasta completar el periodo
        if (nanosleep(&rest, &needed) < 0) {                         
            err(EXIT_FAILURE, "error: cannot execute nanosleep\n");
        } 
        clock_gettime(CLOCK_MONOTONIC, &stop);

        //calculamos el tiempo mandandole a la funcion la suma de segundos y nanosegundos
        ttime = measure_time(begin.tv_sec + (((double)begin.tv_nsec) / MILLION), stop.tv_sec + (((double)stop.tv_nsec) / MILLION));
        lat = (ttime - PERIOD) * MILLION;

        measures->lats[i] = lat;
        measures->med_lat = (lat + measures->med_lat);
        if (lat > measures->max_lat) {
            measures->max_lat = lat;
        }
    }
    measures->med_lat = (measures->med_lat / NUMITERATIONS);
    pthread_exit((void*)measures);
    free(measures);
}

int main() {

    int N_CORES = (int) sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t thread[N_CORES];
    int i, j, k, latency_target_fd, begin, stop; 
    int med_lat = 0;
    int max_lat = 0;
    int finish = 1;
    FILE *file;
    struct measure *measures = malloc(sizeof(*measures));
    
    int meds[N_CORES];
    int ids[N_CORES];
    int maxs[N_CORES];
    int lats[N_CORES][12000];

    /*executing linux with min latency*/
    static int32_t latency_target_value = 0;
    latency_target_fd = open("/dev/cpu_dma_latency", O_RDWR);
    write(latency_target_fd, &latency_target_value, 4);

    /*threads creation*/
    for (i = 0; i < N_CORES; i++)
    {   
        struct thread_data *thread_id = malloc(sizeof(*thread_id));
        thread_id->id = i;
        if (pthread_create(&thread[i], NULL, do_work, (void*)thread_id) != 0) {
            err(EXIT_FAILURE, "error: cannot create thread %d", i);
        }
        
    }
    
    /*joining threads*/
    for (i = 0; i < N_CORES; i++)
    {
        if (pthread_join(thread[i], (void**)&measures) != 0) {
            err(EXIT_FAILURE, "error: cannot join thread %d", i);
        }

        for (j = 0; j < NUMITERATIONS; j++) {
            lats[measures->id_][j] = measures->lats[j];
        }
        
        med_lat = med_lat + measures->med_lat;
        max_lat = max_lat + measures->max_lat;
        
        meds[i] = measures->med_lat;
        maxs[i] = measures->max_lat;
        ids[i] = measures->id_;
    }

    file = fopen("cyclictestURJC_idle_nrt.csv", "w");
    if (file == NULL) {
        err(EXIT_FAILURE, "error: cannot open file");
    }
    i = 0;
    j = 0;
    while(finish == 1) {
        if (i == ids[j]) {
            printf("[%d]\tlatencia media = %09d ns. | max = %09d ns\n", (ids[j]), meds[j], maxs[j]);
            
            for (k = 0; k < NUMITERATIONS; k++) {
                fprintf(file,"%d,%d,%f\n", i, k, lats[i][k]*1e-3);
            }
            
            j = 0;
            i++;
        }
        j++;

        if (i == N_CORES) {
            finish = 0;
        }
    }
    printf("\nTotal\tlatencia media = %09d ns. | max = %09d ns\n", med_lat/N_CORES, max_lat/N_CORES);
    
    free(measures);
    fclose(file);
}