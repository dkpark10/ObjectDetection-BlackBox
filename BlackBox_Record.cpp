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
        
        time_t init = time(NULL);       // 초기 프로그램 시작시간
        ll minute = (ll)init + 60;      // 1분 맞추기
        writer.open(ds.front(), fourcc, fps, size, true); // 비디오라이터를 오픈
        
        if (!writer.isOpened()){                          // 비디오 라이터 열기 실패할 때 에러 날리는 메세지
            cout << "error!! uu uu so sad " << endl;
            return 1;
        }
        
        while (1){
            time_t timer = time(NULL);                    // 영상녹화 돌리는 시간
            cap.read(img);                                               

            if (img.empty()){                             // 이미지 없을 때
                cerr << "empty video" << endl;
                break;
            }
            writer.write(img);                            // 동영상 파일에 한 프레임을 저장함.

            imshow("Color", img);
            waitKey(15);
            if (timer == minute)                          // 1분이 되면 나감
            {
                ds.pop_front();                           // 맨 처음 패스 팝
                break;
            }
        }
    }

    return 0;
}
