/*
현재 1분간격으로 폴더 생성은 성공
그러나 용량이 리미트에 도달했을 때 삭제 될지 의문....
*/

#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <queue>
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
#include <libgen.h>
#include <grp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#define BUFSIZE 1024
#define TRUE 1 
#define FALSE 0
#define ll long long
using namespace std;
queue<string>vs;

ll Get_Disk_Size()                                   // 디스크 용량구하기 함수
{
    ll size = 0;
    struct statfs stf;
    statfs("/", &stf);
    size = stf.f_bsize * stf.f_bavail;
    
    return size;
}

typedef struct DirNode{                              // 폴더이름 넣을 노드
    char dirname[100];
    struct DirNode* link;
}DirNode;

typedef struct Queue{                                // 폴더이름 가리키는 큐
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
    Newnode->link = NULL;
    string s = Newnode->dirname;
    vs.push(s);                                        // 굳이 직접 안만들고 vector<string>으로도 하면 된다....

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

extern void Pop_DirName(Queue* dirQ)                  // Pop() 구현 팝하면서 이름을 반환하고 싶었으나 단일 자료형이 아니라 문자열이기 때문에
{                                                     // 포인터를 반환하고 싶으나 메모리 할당해제하면서 포인터의 값이 사라지기 때문에 안됨                
    DirNode *oldest;
    oldest = dirQ->front;

    if(oldest == NULL){
        return;
    }
    else{
        dirQ->front = dirQ->front->link;
        oldest->link = NULL;
    }
    vs.pop();
    free(oldest);
}

extern char* Get_Del_DirName(Queue* dirQ)              // 그래서 만든게 이함수 ㅜ 삭제할 디렉토리 이름 반환
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

extern void TimetoString(time_t timer, char *str)           // 시간을 입력받아 년원일시분초로 반환해주는 함수
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

    DIR* dir;
    struct dirent *dp;
    struct statfs stfs;
    struct stat st;

    Queue* dir_list = Create_DirQueue();
    char path[] = "/home/dokyun/VScodedk/BlackBox";
    char prev_time[100];
    char cur_time[100];
    char *prev_hour;                              // 전시간을 가리키는 포인터
    char *cur_hour;                               // 현재시간을 가리키는 포인터
    char *del_dir_name;

    char *loot = (char *)malloc(sizeof(char) * 22);
    strcpy(loot,"./");

    // dir = opendir(path);

    time_t timer1;
    timer1 = time(NULL);                         // 현재 시각을 초 단위로 얻기
    
    TimetoString(timer1,prev_time);
    Push_DirName(dir_list, prev_time);
    prev_hour = strstr(prev_time, "_");         // "_"의 주소 반환
    prev_hour += 4;                             // + 2만큼 포인터 연산해서 시간을 가리킴

    if (mkdir(prev_time, 0776) < 0 && errno != EEXIST){
        perror("Create Folder Fail\n");
        return -1;
    }

    printf("%s",prev_time); // pass pass pass pass

    while (1){
        
        const ll limit = 42950000000;
        ll avail = Get_Disk_Size();

        // printf("prev_time is %s\n", prev_time);
        sleep(1);
        time_t timer2;
        timer2 = time(NULL);     
        TimetoString(timer2, cur_time);
        // printf("cur_time is %s\n", cur_time);

        cur_hour = strstr(cur_time, "_");
        cur_hour += 4;

        if (*cur_hour != *prev_hour){
            if (mkdir(cur_time, 0776) < 0 && errno != EEXIST){
                perror("Create Folder Fail\n");
                return -1;
            }
            Push_DirName(dir_list, cur_time);
            strcpy(prev_time, cur_time);
        }
        if(limit > avail){
            del_dir_name = Get_Del_DirName(dir_list);
            if(remove(strcat(loot, del_dir_name)) == -1){
                perror("remove fail\n");
                return -1;
            }
            if(remove(del_dir_name) == -1){
                perror("remove rail\n");
                return -1;
            }
            Pop_DirName(dir_list);
            vs.pop();
        }
    }
    free(dir_list);

    return 0;
}
