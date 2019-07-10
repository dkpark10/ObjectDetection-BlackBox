/*
현재시간으로 된 폴더 생성 시간까지
ex)20190625_105548
폴더 있는지 없는지 검사
*/

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#define BUFSIZE 1024
#define TRUE 1 
#define FALSE 0
#define ll long long
using namespace std;
vector<string>vs;

typedef struct DirNode{
    char dirname[100];
    struct DirNode* link;
}DirNode;

typedef struct Queue{
    DirNode* front;
    DirNode* rear;
}Queue;

extern Queue* Create_DirQueue()
{
    Queue* dirQ;
    dirQ = (Queue*)malloc(sizeof(Queue));
    dirQ ->front = NULL;
    dirQ ->rear = NULL;

    return dirQ;
}

extern void Push_DirName(Queue* dirQ, const char *dirname)
{
    DirNode* Newnode; 
    Newnode = (DirNode*)malloc(sizeof(DirNode));
    strcpy(Newnode->dirname, dirname);
    string s = Newnode->dirname;
    vs.push_back(s);
    Newnode->link = NULL;

    if(dirQ->front == NULL){
        dirQ->front = Newnode;
        dirQ->rear = Newnode;
        Newnode ->link = NULL;
    }
    else{
        dirQ->rear->link = Newnode;
        dirQ->rear = Newnode;
        Newnode->link = NULL;
    }
}

extern void Pop_DirName(Queue* dirQ)
{
    DirNode *oldest;
    oldest = dirQ->front;

    if(oldest == NULL){
        return;
    }
    else{
        dirQ->front = dirQ->front->link;
        oldest->link = NULL;
    }
    free(oldest);
}

extern char* Get_Del_DirName(Queue* dirQ)
{
    DirNode* temp;
    temp = dirQ->front;
    char *ptr = temp->dirname;

    if(temp->link==NULL){
        return NULL;
    }
    else return ptr;
}

extern void Print_DirName(Queue* dirQ)
{
    DirNode* temp;
    temp = dirQ->front;
    if(dirQ->front == NULL){
        return;
    }
    else{
        while(temp){
            printf("%s\n", temp->dirname);
            temp = temp->link;
        }
    }
}

extern void TimetoString(time_t timer, char *str)
{
    struct tm* tmptr;
    tmptr = localtime(&timer);

    sprintf(str, "%d%02d%02d_%02d%02d%02d",
            tmptr->tm_year + 1900, tmptr->tm_mon + 1, tmptr->tm_mday,
            tmptr->tm_hour, tmptr->tm_min, tmptr->tm_sec);
}

int main(int argc, char *argv[])
{
    // struct timeval curtime;
    // gettimeofday(&curtime, NULL);
    // printf("%ld: %ld\n", curtime.tv_sec, curtime.tv_usec);

    //
    // DIR* dir;
    // struct dirent *dp;
    // Queue* dir_list = Create_DirQueue();
    char path[] = "/home/dokyun/VScodedk/BlackBox";
    // char prev_time[100];
    // char cur_time[100];
    // char *prev_hour;
    // char *cur_hour;
    // char *del_dir_name;

    // dir = opendir(path);

    // time_t timer;
    // timer = time(NULL);                         // 현재 시각을 초 단위로 얻기
    
    // TimetoString(timer,prev_time);
    // Push_DirName(dir_list, prev_time);
    // prev_hour = strstr(prev_time, "_");         // "_"의 주소 반환
    // prev_hour += 2;                             // + 2만큼 포인터 연산해서 시간을 가리킴

    // if (mkdir(prev_time, 0776) < 0 && errno != EEXIST){
    //     perror("Create Folder Fail\n");
    //     return -1;
    // }

    // printf("%s",prev_time); // pass pass pass pass

    // while (1){
        
    //     TimetoString(timer, cur_time);
    //     cur_hour = strstr(cur_time, "_");
    //     cur_hour += 2;

    //     if (*cur_hour != *prev_hour){
    //         if (mkdir(cur_time, 0776) < 0 && errno != EEXIST){
    //             perror("Create Folder Fail\n");
    //             return -1;
    //         }
    //         Push_DirName(dir_list, cur_time);
    //         strcpy(prev_time, cur_time);
    //     }
    //     sleep(5);
    // }
    // free(dir_list);

    DIR *pDir;
    struct dirent *pNode;
    struct stat buf;

    if ((pDir = opendir(path)) == NULL)
    {
        perror("Failed to open directory");
        return -1;
    }

    while((pNode = readdir(pDir)) != NULL){
        lstat(path, &buf);
        if(S_ISREG(buf.st_mode)){
            printf("file is %s\n", pNode->d_name);
            printf("file size is %ld\n",(long)buf.st_size);
        }
    }

    return 0;
}
