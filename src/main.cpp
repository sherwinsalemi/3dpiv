#include <stdio.h>
#include <vector>

#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/mat.hpp>


int main()
{
    // cv::VideoCapture cap;
    // cap.open("res/vid.mp4");
    // if (!cap.isOpened())w
    // {
    //     printf("Failed to open file capture.");
    //     return 1;
    // }

    
    cv::VideoCapture cap("res/vid.mp4");
    cv::Mat previous{0};
    cv::Mat current{0};
    cv::Mat frame;

    cv::namedWindow("PIV", cv::WINDOW_NORMAL);

    cap >> frame;
    cv::cvtColor(frame, previous, cv::COLOR_RGB2GRAY);
    cv::cvtColor(frame, current, cv::COLOR_RGB2GRAY);

    std::vector<cv::Point2f> points;
    std::vector<cv::Point2f> nextPts;

    points[0] = cv::Point2f(200,200);

    while(true)
    {
        cap >> frame;
        current.copyTo(previous);
        cv::cvtColor(frame, current, cv::COLOR_RGB2GRAY);

        std::vector<unsigned char> status;
        std::vector<float> err;
      
        cv::calcOpticalFlowPyrLK(
            previous, current, points, nextPts, 
            status, err
        );
        
        

        cv::imshow("PIV", current);
        cv::waitKey(100);        
    }
    

    printf("Hello world\n");
    return 0;
}