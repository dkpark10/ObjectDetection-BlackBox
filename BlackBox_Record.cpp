/*
1분마다 영상녹화 저장
g++ `pkg-config --cflags opencv` -o Video_Rec Video_Rec.cpp `pkg-config --libs opencv` 
compile code
*/
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <time.h>
#include <algorithm>
#include <deque>
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
#include <libgen.h>
#include <grp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
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
	return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + \
		patch::to_string(width) + ", height=(int)" + patch::to_string(height)\
		+ ",format=(string)NV12, framerate=(fraction)" + patch::to_string(fps) + \
		"/1 ! nvvidconv ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw,\
	       format=(string)BGR ! appsink";
}

ll Get_Disk_Size()
{
	ll size = 0; 
	struct statfs stf;
	statfs("/",&stf);
	size = stf.f_bsize * stf.f_bavail;

	return size;
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
	VideoCapture cap(Init_Pipeline(640, 480, 30.0));
	if (!cap.isOpened()) {
		cerr << "cap opened is fail" << endl;;
		return -1;
	}

	// 동영상 파일을 저장하기 위한 준비
	Size size = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CAP_PROP_FRAME_HEIGHT));

	VideoWriter writer;
	double fps = 30.0;
	int fourcc = VideoWriter::fourcc('D', 'I', 'V', 'X');

	while (1) {

		time_t init = time(NULL);
		ll minute = (ll)init + 60;
		writer.open(ds.front(), fourcc, fps, size, true);

		if (!writer.isOpened()) {
			cout << "error!! uu uu so sad " << endl;
			return 1;
		}

		while (1) {
			time_t timer = time(NULL);
			cap.read(img);

			if (img.empty()) {
				cerr << "empty video" << endl;
				break;
			}

			//동영상 파일에 한 프레임을 저장함.
			writer.write(img);

			imshow("Color", img);
			printf("cur disk size is %lld\n",Get_Disk_Size());
			sleep(1);
			waitKey(15);
			if (timer == minute){
				ds.pop_front();
				break;
			}
		}
	}

	return 0;
}

