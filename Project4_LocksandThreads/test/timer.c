#include "allh.h"
#include "list.h"
#include "hash.h"
#include "counter.h"
#define MAX_THREAD_NUM 25
#define TEST_TIMES 15

struct timeval t1,t2;
char* lock_name[5] = {"_","SPINLOCK","MUTEX","TWOPHASE","PTHREAD"};

int i,j,thread_num,type,rc;
//int tmp[OPERATION_TIMES + 10];
double thread_time[10000];
double total_time;
list_t list;
hash_t hash;
counter_t counter;
pthread_t* p;

double getms(struct timeval t1, struct timeval t2){ return (t2.tv_sec - t1.tv_sec) *1000.0 + (t2.tv_usec - t1.tv_usec)/1000.0; }

void *list_thread_test(void *t)
{
	//printf("%s: begin\n", (char*)arg);
	int OPERATION_TIMES = (int)1e3;
	int i;
	int type = *(int *)t;
	if (type == 1){
        for (i = 0; i < OPERATION_TIMES ; i++) {
            list_insert(&list, (unsigned int)i);
        }
        for (i = 0; i < OPERATION_TIMES ; i++) {
            list_delete(&list, (unsigned int)i);
        }
        //printf("%s: done\n", (char*)arg);
        return NULL;
    }
    if (type == 2){
        for (i = 0; i < OPERATION_TIMES ; i++) {
            list_insert(&list, (unsigned int)i);
        }
        for (i = OPERATION_TIMES-1; i >= 0 ; i--) {
            list_delete(&list, (unsigned int)i);
        }
        //printf("%s: done\n", (char*)arg);
        return NULL;
    }
    if (type ==3) {
        for (i=0;i<OPERATION_TIMES ;i++){
            list_insert(&list, (unsigned int)i);
            list_delete(&list, (unsigned int)i);
        }
        return NULL;
    }
    if (type ==4){
        for (i=0;i<OPERATION_TIMES ;i++)
            list_insert(&list, (unsigned int)i);
        for (i=0;i<OPERATION_TIMES ;i++){
            int ran = rand() % OPERATION_TIMES;
            list_delete(&list, (unsigned int)ran);
        }
        return NULL;
    }
}

void *counter_thread_test(void *t)
{
	//printf("%s: begin\n", (char*)arg);
	struct timeval start_t;struct timeval end_t;
	gettimeofday(&start_t,NULL);
	int OPERATION_TIMES = (int)1e6;
	int i;
	int thread_number = *(int *)t;
	for (i=0; i<OPERATION_TIMES; i++){
		counter_increment(&counter);
	}
	gettimeofday(&end_t,NULL);

	thread_time[thread_number] = getms(start_t,end_t);
	return NULL;
}

void *hash_thread_test(void *t)
{
	int i;
	int OPERATION_TIMES = 1000;

    for(i = 0;i<OPERATION_TIMES; i++){
        hash_insert(&hash, i*rand());
    }
    for(i = 0;i<OPERATION_TIMES; i++){
        hash_delete(&hash, i*rand());
    }
}

void list_test(){
    for (thread_num=1;thread_num<=MAX_THREAD_NUM;thread_num += 1){
        for (type=1;type<=4;type++){
            total_time = 0;
            for(i=0;i<2*TEST_TIMES;i++){

                p = malloc(sizeof(pthread_t) * thread_num);

                gettimeofday(&t1,NULL);

                //start timing
                list_init(&list);
                //printf("main: begin(counter = %d)\n", counter_get_value(&counter));
                for(j=0;j<thread_num;j++){
                        rc = pthread_create(&p[j], NULL, list_thread_test, &type);
                }
                for(j=0;j<thread_num;j++){
                    rc = pthread_join(p[j], NULL);
                }

                //printf("main: done with both(counter = %d)\n", counter_get_value(&counter));
                //end timing

                gettimeofday(&t2,NULL);
                total_time += getms(t1,t2);
                free(p);
            }
            //printf("LOCK = %s THREADS = %d TYPE = %d AVG = %.3lf ms\n",lock_name[LOCK_TYPE],thread_num,type,total_time/(0.0+test_times));
            printf("{\"LOCK\": \"%s\",\"THREADS\":%d,\"TEST_TYPE\" : %d,\"AVG\" : %.3lf }\n",lock_name[LOCK_TYPE],thread_num,type,total_time/(2.0*TEST_TIMES));
        }
    }
}

void hash_test(){
    int start_thread = 1;
    int maxsize;
// 变量：　锁类型，线程数　　固定：MAX_SIZE
    if (HASH_FIX_THREADS == 0){
        maxsize = 100;
        for (thread_num = 1;thread_num <= MAX_THREAD_NUM; thread_num++){
            total_time = 0;
            for(i = 0;i < 2*TEST_TIMES; i++) {
                p = malloc(sizeof(pthread_t) * thread_num);
                gettimeofday(&t1,NULL);
                hash_init(&hash,maxsize);


                for(j=0;j<thread_num;j++)
                    pthread_create(&p[j], NULL, hash_thread_test, &type);
                for (j=0;j<thread_num;j++)
                    rc = pthread_join(p[j],NULL);

                gettimeofday(&t2,NULL);
                total_time += getms(t1,t2);
                free(p);
            }
            printf("{\"LOCK\": \"%s\",\"MAX_SIZE\":%d, \"THREADS\":%d,\"TEST_TYPE\" : %d,\"AVG\" : %.3lf }\n",lock_name[LOCK_TYPE],maxsize, thread_num,1,total_time/(2.0*TEST_TIMES));
        }
    }
// 变量：SIZE，锁类型 　　固定：线程数
    else {

        thread_num = MAX_THREAD_NUM;
        for (maxsize = 10;maxsize <=10000; maxsize += 100){
            for(i = 0;i < 2*TEST_TIMES; i++) {
                p = malloc(sizeof(pthread_t) * thread_num);
                gettimeofday(&t1,NULL);
                hash_init(&hash, maxsize);


                for(j=0;j<thread_num;j++)
                    pthread_create(&p[j], NULL, hash_thread_test, &type);
                for (j=0;j<thread_num;j++)
                    rc = pthread_join(p[j],NULL);

                gettimeofday(&t2,NULL);
                total_time += getms(t1,t2);
                free(p);
            }
            printf("{\"LOCK\": \"%s\",\"MAX_SIZE\":%d, \"THREADS\":%d,\"TEST_TYPE\" : %d,\"AVG\" : %.3lf }\n",lock_name[LOCK_TYPE],maxsize, thread_num,2,total_time/(2.0*TEST_TIMES));
        }
    }
}

void twophase_test(){
        for (thread_num = 1;thread_num <= 20; thread_num += 4){
		total_time = 0;
		for (i=0;i<TEST_TIMES;i++){
		    p = malloc(sizeof(pthread_t) * thread_num);

		    gettimeofday(&t1,NULL);

		    //start timing
		    counter_init(&counter,0);
		    for(j=0;j<thread_num;j++){
		        rc = pthread_create(&p[j], NULL, counter_thread_test, &type);
		    }
		    for(j=0;j<thread_num;j++){
		        rc = pthread_join(p[j], NULL);
		    }
		    gettimeofday(&t2,NULL);
		    total_time += getms(t1,t2);
		    free(p);
		}
		printf("{\"LOCK\": \"%s\",\"THREADS\":%d,\"SPIN_TIMES\" : %d,\"AVG\" : %.3lf }\n",lock_name		[LOCK_TYPE],thread_num,SPIN_TIMES,total_time/(TEST_TIMES));
	}
}

void counter_test(){
  for (thread_num=1;thread_num<=MAX_THREAD_NUM;thread_num += 1){
        total_time = 0;
        for(i=0;i<2*TEST_TIMES;i++){
            p = malloc(sizeof(pthread_t) * thread_num);

            gettimeofday(&t1,NULL);

            //start timing
            counter_init(&counter,0);
            //printf("main: begin(counter = %d)\n", counter_get_value(&counter));
            for(j=0;j<thread_num;j++){
                    rc = pthread_create(&p[j], NULL, counter_thread_test, &type);
            }
            for(j=0;j<thread_num;j++){
                rc = pthread_join(p[j], NULL);
            }

            //printf("main: done with both(counter = %d)\n", counter_get_value(&counter));
            //end timing

            gettimeofday(&t2,NULL);
            total_time += getms(t1,t2);
            free(p);
        }
        //printf("LOCK = %s THREADS = %d TYPE = %d AVG = %.3lf ms\n",lock_name[LOCK_TYPE],thread_num,type,total_time/(0.0+test_times));
        printf("{\"LOCK\": \"%s\",\"THREADS\":%d,\"TEST_TYPE\" : %d,\"AVG\" : %.3lf }\n",lock_name[LOCK_TYPE],thread_num,1,total_time/(2.0*TEST_TIMES));
    }
}

void fairness_test(){
  for (thread_num=1;thread_num<=15;thread_num += 1){
        total_time = 0;
        double avr = 0;
        for(i=0;i<3;i++){
            p = malloc(sizeof(pthread_t) * thread_num);
            counter_init(&counter,0);
            //printf("main: begin(counter = %d)\n", counter_get_value(&counter));
            for(j=0;j<thread_num;j++){
                    rc = pthread_create(&p[j], NULL, counter_thread_test, &j);
            }
            for(j=0;j<thread_num;j++){
                rc = pthread_join(p[j], NULL);
            }
            free(p);
            //for (j=0;j<thread_num;j++)
                //printf("%.3lf ",thread_time[j]);printf("\n");
            double biaozhuncha = 0;
            double sum = 0;
            for(j=0;j<thread_num;j++)
                sum += thread_time[j];
            sum /= thread_num;
            for (j=0;j<thread_num;j++)
                biaozhuncha += (thread_time[j] - sum)*(thread_time[j]-sum);
            biaozhuncha /= thread_num;
            avr += sqrt(biaozhuncha);
        }
        //printf("LOCK = %s THREADS = %d TYPE = %d AVG = %.3lf ms\n",lock_name[LOCK_TYPE],thread_num,type,total_time/(0.0+test_times));
        printf("{\"LOCK\": \"%s\",\"THREADS\":%d,\"FAIRNESS\" : %.3lf}\n",lock_name[LOCK_TYPE],thread_num,avr/3);
    }
}

int main(int argc,char* argv[]){
    if (strcmp(argv[1],"list") == 0) list_test();
    if (strcmp(argv[1],"hash") == 0) hash_test();
    if (strcmp(argv[1],"counter") == 0) counter_test();
    if (strcmp(argv[1],"twophase") == 0) twophase_test();
    if (strcmp(argv[1],"fairness") == 0) fairness_test();
    return 0;
}


