#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <sstream>
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

string get_tegra_pipline(int width, int height, double fps)
{
        return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" +\
               patch::to_string(width) + ", height=(int)" + patch::to_string(height)\
               + ",format=(string)NV12, framerate=(fraction)" + patch::to_string(fps) +\
               "/1 ! nvvidconv ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw,\
               format=(string)BGR ! appsink";
}

int main()
{
    Mat img_color;
        
    //비디오 캡쳐 초기화        
    string init = get_tegra_pipline(720,480,30);
    VideoCapture cap(0);
    if (!cap.isOpened()){
        cerr << "cap opened is fail";
        return -1;
    }

    // 동영상 파일을 저장하기 위한 준비
    Size size = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
                     (int)cap.get(CAP_PROP_FRAME_HEIGHT));

    VideoWriter writer;
    double fps = 30.0;
    writer.open("output.avi", VideoWriter::fourcc('D', 'I', 'V', 'X'), fps, size, true);
    if (!writer.isOpened()){
        cout << "동영상을 저장하기 위한 초기화 작업 중 에러 발생" << endl;
        return 1;
    }

    while (1){
        cap.read(img_color);
        if (img_color.empty())
        {
            cerr << "빈 영상이 캡쳐되었습니다.\n";
            break;
        }

        //동영상 파일에 한 프레임을 저장함.
        writer.write(img_color);

        imshow("Color", img_color);

        if (waitKey(25) >= 0)
            break;
    }

    return 0;
}


