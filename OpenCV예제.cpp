#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
    Mat img_color;

    // 재생할 동영상 파일을 지정해줍니다.
    VideoCapture cap("res.avi");
    if (!cap.isOpened()){
        cerr << "에러 - 카메라를 열 수 없습니다.\n";
        return -1;
    }

    while (1)
    {

        cap.read(img_color);
        if (img_color.empty()){
            cerr << "빈 영상입니다.\n";
            break;
        }

        imshow("Color", img_color);

        if (waitKey(25) >= 0)
            break;
    }
    return 0;
}
