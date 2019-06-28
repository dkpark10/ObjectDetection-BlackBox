#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int glob_var = 6;


void *t_function(void *data)
{
    int id;
    pthread_t t_id;
    id = *((int *)data);
    t_id=pthread_self();
    printf("pid=%d, t_id=%lu, id=%d, glob_var=%d\n",getpid(),t_id,id,glob_var);
    return (void*)(id*id);
}

int main(void)
{
    pthread_t p_thread[2];
    int status;
    int a=1;
    int b=2;
    int err;

    printf("before pthread_create() pid = %d, glob_var=%d\n",getpid(), glob_var);

    //create thread1 
    if((err = pthread_create(&p_thread[0], NULL, t_function, (void*)&a)) <0)
    {
        perror("thread create error : ");
        exit(1);
    }

    //create thread2 
    if((err = pthread_create(&p_thread[1], NULL, t_function, (void*)&b)) <0)
    {
        perror("thread create error : ");
        exit(2);
    }

    pthread_join(p_thread[0], (void**)&status);
    printf("thread join : %d\n", status);

    pthread_join(p_thread[1], (void**)&status);
    printf("thread join : %d\n", status);

    printf("after pthread_create() glob_var=%d\n", glob_var);

    return 0;

}
