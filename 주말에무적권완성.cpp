#include <opencv2/opencv.hpp>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <deque>
#include <ctime>
#include <queue>
#include <string>
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
#define DISK_MAX 0
#define DISK_AVA 1
#define BY_MINUTE 0
#define BY_SECOND 1
#define VID_TIME time_t
#define FOL_TIME time_t
#define ll long long
using namespace std;
using namespace cv;
queue<string> folder_list;
queue<string> video_list;
const char path[100] = "/home/nvidia/DKblackbox/";

namespace patch 
{
	template < typename T > std::string to_string(const T& n)
	{
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}
}

string Init_Pipeline(int width, int height, double fps)  // 뭔진 모르지만 젯슨에서 카메라 활성화 해야 할 때 인자로 넘겨줘야함
{
	return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" +\
	       patch::to_string(width) + ", height=(int)" + patch::to_string(height)\
	       + ",format=(string)NV12, framerate=(fraction)" + patch::to_string(fps) +\
	       "/1 ! nvvidconv ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw,\
	       format=(string)BGR ! appsink";
}

// typedef struct DirNode{
//     char dirname[100];
//     struct DirNode* link;
// }DirNode;

// typedef struct Queue{
//     DirNode* front;
//     DirNode* rear;
// }Queue;

// extern Queue* Create_DirQueue()
// {
//     Queue* dirQ;
//     dirQ = (Queue*)malloc(sizeof(Queue));
//     dirQ ->front = NULL;
//     dirQ ->rear = NULL;

//     return dirQ;
// }

// extern void Push_DirName(Queue* dirQ, const char *dirname)
// {
//     DirNode* Newnode; 
//     Newnode = (DirNode*)malloc(sizeof(DirNode));
//     strcpy(Newnode->dirname, dirname);
//     Newnode->link = NULL;
//     string s = Newnode->dirname;
//     folder_list.push(s);

//     if(dirQ->front == NULL){
//         dirQ->front = Newnode;
//         dirQ->rear = Newnode;
//         Newnode ->link = NULL;
//     }
//     else{
//         dirQ->rear->link = Newnode;
//         dirQ->rear = Newnode;
//         Newnode->link = NULL;
//     }
// }

// extern void Pop_DirName(Queue* dirQ)
// {
//     DirNode *oldest;
//     oldest = dirQ->front;

//     if(oldest == NULL){
//         return;
//     }
//     else{
//         dirQ->front = dirQ->front->link;
//         oldest->link = NULL;
//     }
//     folder_list.pop();
//     free(oldest);
// }

// extern char* Get_Del_DirName(Queue* dirQ)
// {
//     DirNode* temp;
//     temp = dirQ->front;
//     char *ptr = temp->dirname;

//     if(temp->link==NULL){
//         return NULL;
//     }
//     else return ptr;
// }

// extern void Print_DirName(Queue* dirQ)
// {
//     DirNode* temp;
//     temp = dirQ->front;
//     if(dirQ->front == NULL){
//         return;
//     }
//     else{
//         while(temp){
//             printf("%s\n", temp->dirname);
//             temp = temp->link;
//         }
//     }
// }

extern ll Get_Disk_Size(int what_size)
{
    struct statfs stf;
    statfs("/", &stf);
    if(what_size == DISK_MAX) return stf.f_bsize * stf.f_blocks;
    else return stf.f_bsize * stf.f_bavail;
}

extern void TimetoString(time_t timer, char *str, int time_type)
{
    struct tm* tmptr;
    tmptr = localtime(&timer);

    if (time_type == BY_SECOND){
        sprintf(str, "%d%02d%02d_%02d%02d%02d",
                tmptr->tm_year + 1900, tmptr->tm_mon + 1, tmptr->tm_mday,
                tmptr->tm_hour, tmptr->tm_min, tmptr->tm_sec);
    }
    else if (time_type == BY_MINUTE){
        sprintf(str, "%d%02d%02d_%02d%02d",
                tmptr->tm_year + 1900, tmptr->tm_mon + 1, tmptr->tm_mday,
                tmptr->tm_hour, tmptr->tm_min);
    }
}

string Video_Date_Name()
{
    return "/home/nvidia/DKblackbox/" + folder_list.front() + "/" +
           video_list.front() + ".avi";
}

bool Is_Changed_Time(char **prev, char **cur)
{
    if(**prev != **cur) return true;
    else return false;
}

int Create_Folder(time_t timer)
{
    char prev_time[100];
    TimetoString(timer, prev_time, BY_SECOND);
    video_list.push(prev_time);
    TimetoString(timer, prev_time, BY_MINUTE);
    folder_list.push(prev_time);

    if (mkdir(prev_time, 0776) < 0 && errno != EEXIST){ //$$$$$$$$$$$$$
        perror("Create Folder Fail\n");
        return -1;
    }
    return 0;
}

int Video_Record()
{
    Mat img;

    //비디오 캡쳐 초기화
    VideoCapture cap(Init_Pipeline(640,480,30.0));
    if (!cap.isOpened()){
        cerr << "cap opened is fail"<< endl;
        return -1;
    }

    // 동영상 파일을 저장하기 위한 준비
    Size size = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
                     (int)cap.get(CAP_PROP_FRAME_HEIGHT));

    VideoWriter writer;
    int fourcc = VideoWriter::fourcc('D', 'I', 'V', 'X');

    FOL_TIME prev_fol = time(NULL);
    ll folder_hour = (ll)prev_fol + 3600;
    Create_Folder(prev_fol);

    /////////////////////// REC START!! ///////////////////
    while (1){


        string video_date_name = Video_Date_Name();

        VID_TIME begin_time = time(NULL);
        ll video_minute = (ll)begin_time + 60;
        writer.open(video_date_name, fourcc, 30.0, size, true);

        if (!writer.isOpened()){
            cout << "error!! uu uu so sad " << endl;
            return 1;
        }
        
        while (1){

            VID_TIME end_video_time = time(NULL);
            FOL_TIME end_folder_time = time(NULL);
            char new_video_time_name[100];
            cap.read(img);

            if (img.empty()){
                cerr << "empty video uu terrible >.<" << endl;
                break;
            }

            writer.write(img);      //동영상 파일에 한 프레임을 저장함.

            imshow("Color", img);
            printf("cur disk size is %lld\n", Get_Disk_Size(DISK_AVA));
            waitKey(15);

            // if(end_folder_time == folder_hour){

            // }

            if (end_video_time == video_minute){
                TimetoString(end_video_time, new_video_time_name, BY_SECOND);
                video_list.pop();
                video_list.push(new_video_time_name);
                break;
            }
        }
    }
    writer.release();
    cap.release();

    return 0;
}

int main(int argc, char *argv[])
{
    // struct timeval curtime;
    // gettimeofday(&curtime, NULL);
    // printf("%ld: %ld\n", curtime.tv_sec, curtime.tv_usec);

    // DIR* dir;
    // struct dirent *dp;
    // struct statfs stfs;
    // struct stat st;
    // dir = opendir(path);

    // Queue* dir_list = Create_DirQueue();
    char prev_time[100];
    char cur_time[100];
    char *prev_folder_hour;
    char *cur_folder_hour;
    char *del_dir_name;
    char *loot = (char *)malloc(sizeof(char) * 22);
    strcpy(loot,"./");

    time_t timer1 = time(NULL);                 // 현재 시각을 초 단위로 얻기

    TimetoString(timer1, prev_time, BY_SECOND);
    video_list.push(prev_time);
    TimetoString(timer1, prev_time, BY_MINUTE);
    folder_list.push(prev_time);
    // Push_DirName(dir_list, prev_time);

    prev_folder_hour = strstr(prev_time, "_");         // "_"의 주소 반환
    prev_folder_hour += 2;                             // + 2만큼 포인터 연산해서 시간을 가리킴

    if (mkdir(prev_time, 0776) < 0 && errno != EEXIST){    //$$$$$$$$$$$$$
        perror("Create Folder Fail\n");
        return -1;
    }
    Video_Record();

    // printf("%s",prev_time); // pass pass pass pass

    while (1){
        
        ll max_size = Get_Disk_Size(DISK_MAX);
        ll ava_size = Get_Disk_Size(DISK_AVA);
        ll limit = max_size * 0.45;

        // printf("prev_time is %s\n", prev_time);
        time_t timer2 = time(NULL);
        TimetoString(timer2, cur_time, BY_MINUTE);
        // printf("cur_time is %s\n", cur_time);

        cur_folder_hour = strstr(cur_time, "_");
        cur_folder_hour += 2;

        if (Is_Changed_Time(&prev_folder_hour, &cur_folder_hour)){
            if (mkdir(cur_time, 0776) < 0 && errno != EEXIST){  //$$$$$$$$$$$$$
                perror("Create Folder Fail\n");
                return -1;
            }
            // Push_DirName(dir_list, cur_time);
            folder_list.push(cur_time);
            strcpy(prev_time, cur_time);
        }
        if(limit > ava_size){
            // del_dir_name = Get_Del_DirName(dir_list);  //$$$$$$$$$$$$$
            if(remove(strcat(loot, del_dir_name)) == -1){
                perror("Remove file fail\n");
                return -1;
            }
            if(remove(del_dir_name) == -1){
                perror("Remove folder rail\n");
                return -1;
            }
            // Pop_DirName(dir_list);
            folder_list.pop();
        }
    }
    // free(dir_list);

    return 0;
}








////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <time.h>
#include <algorithm>
#include <deque>
#include <vector>
#define ll long long
using namespace cv;
using namespace std;

namespace patch
{
	template < typename T > std::string to_string(const T& n)
	{
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}
}

string Init_Pipeline(int width, int height, double fps) 
{
	return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" +\
	       patch::to_string(width) + ", height=(int)" + patch::to_string(height)\
	       + ",format=(string)NV12, framerate=(fraction)" + patch::to_string(fps) +\
	       "/1 ! nvvidconv ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw,\
	       format=(string)BGR ! appsink";
}

int main()
{
    Mat img;
    
    deque<string> ds;
    ds.push_back("123.avi");
    ds.push_back("456.avi");
    ds.push_back("789.avi");
    ds.push_back("abc.avi");
    ds.push_back("xyz.avi");

    //비디오 캡쳐 초기화
    VideoCapture cap(Init_Pipeline(640,480,30.0));
    if (!cap.isOpened()){
        cerr << "cap opened is fail"<< endl;;
        return -1;
    }

    // 동영상 파일을 저장하기 위한 준비
    Size size = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
                     (int)cap.get(CAP_PROP_FRAME_HEIGHT));

    VideoWriter writer;
    double fps = 30.0;
    int fourcc = VideoWriter::fourcc('D', 'I', 'V', 'X');

    while (1){
        
        time_t init = time(NULL);
        ll minute = (ll)init + 60;
        writer.open(ds.front(), fourcc, fps, size, true);
        
        if (!writer.isOpened()){
            cout << "error!! uu uu so sad " << endl;
            return 1;
        }
        
        while (1){
            time_t timer = time(NULL);
            cap.read(img);

            if (img.empty()){
                cerr << "empty video" << endl;
                break;
            }

            //동영상 파일에 한 프레임을 저장함.
            writer.write(img);

            imshow("Color", img);
            waitKey(15);
            if (timer == minute)
            {
                ds.pop_front();
                break;
            }
        }
    }

    return 0;
}
