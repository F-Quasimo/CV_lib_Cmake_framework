#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include "yolox.h"
#include "time.h"
int main(int argc, char* argv[]){
   
    std::cout << "OpenCV version : " << CV_VERSION << std::endl;
    std::cout << "Major version : " << CV_MAJOR_VERSION << std::endl;
    std::cout << "Minor version : " << CV_MINOR_VERSION << std::endl;
    std::cout << "Subminor version : " << CV_SUBMINOR_VERSION << std::endl;
    cv::VideoCapture cap(std::stoi(argv[1])+cv::CAP_DSHOW);

    cap.open(std::stoi(argv[1]));
    cap.set(cv::CAP_PROP_FOURCC,cv::VideoWriter::fourcc('M','J','P','G'));
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    cap.set(cv::CAP_PROP_FPS, 30);
    if(!cap.isOpened())
	{
		std::cout << "open camera failed. " << std::endl;
		return -1;
	}
    cv::Mat frame;
    YoloX yolox(0.25);
    while(true)
	{
		cap >> frame;  
		if(!frame.empty())
		{
            std::vector<Object> objects;
            yolox.detect_yolox(frame, objects);
            yolox.draw_objects(frame, objects);
			cv::imshow("camera", frame);
		}
		
		if(cv::waitKey(1) > 0)	
		{
			break;
		}
	}

    return 0;
}
