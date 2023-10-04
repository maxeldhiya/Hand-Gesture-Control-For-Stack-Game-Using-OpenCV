//Include Header
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>

using namespace std;
using namespace cv;

//Construct Random Color
RNG rng(12345);

//Declare Variable
Mat frame0, frame1, frameFlip, frame, frameHSV, frameGray, frameBlur, frameThreshold, frameCanny, frameInRange, frameErode1, frameDil1, frameDil2, frameErode2;

//Create HSV Trackbar
const int max_value_H = 180;
const int max_value = 255;
int low_H = 0, low_S = 50, low_V = 10;
int high_H = 25, high_S = max_value, high_V = max_value;

static void on_low_H_thresh_trackbar(int, void *)
{
    low_H = min(high_H-1, low_H);
    setTrackbarPos("Low H", "frame Erode2", low_H);
}

static void on_high_H_thresh_trackbar(int, void *)
{
    high_H = max(high_H, low_H+1);
    setTrackbarPos("High H", "frame Erode2", high_H);
}

static void on_low_S_thresh_trackbar(int, void *)
{
    low_S = min(high_S-1, low_S);
    setTrackbarPos("Low S", "frame Erode2", low_S);
}

static void on_high_S_thresh_trackbar(int, void *)
{
    high_S = max(high_S, low_S+1);
    setTrackbarPos("High S", "frame Erode2", high_S);
}

static void on_low_V_thresh_trackbar(int, void *)
{
    low_V = min(high_V-1, low_V);
    setTrackbarPos("Low V", "frame Erode2", low_V);
}

static void on_high_V_thresh_trackbar(int, void *)
{
    high_V = max(high_V, low_V+1);
    setTrackbarPos("High V", "frame Erode2", high_V);
}

void TrackbarHSV()
{
    createTrackbar("Low H", "frame Erode2", &low_H, max_value_H, on_low_H_thresh_trackbar);
    createTrackbar("High H", "frame Erode2", &high_H, max_value_H, on_high_H_thresh_trackbar);
    createTrackbar("Low S", "frame Erode2", &low_S, max_value, on_low_S_thresh_trackbar);
    createTrackbar("High S", "frame Erode2", &high_S, max_value, on_high_S_thresh_trackbar);
    createTrackbar("Low V", "frame Erode2", &low_V, max_value, on_low_V_thresh_trackbar);
    createTrackbar("High V", "frame Erode2", &high_V, max_value, on_high_V_thresh_trackbar);
}


//Create Region Of Interest Trackbar
const int max_value_xroi = 640;
const int max_value_yroi = 480;
int leftroi = 350, rightroi = 600 , uproi = 100, downroi = 300;

static void roi_left(int, void*)
{
    leftroi=min(leftroi,rightroi-1);
    setTrackbarPos("Left side of ROI", "frame Erode2", leftroi);
}

static void roi_right(int, void*)
{
    rightroi=max(leftroi+1,rightroi);
    setTrackbarPos("Right side of ROI", "frame Erode2", rightroi);
}

static void roi_up(int, void*)
{
    uproi=min(uproi,downroi-1);
    setTrackbarPos("Top side of ROI", "frame Erode2", uproi);
}

static void roi_down(int, void*)
{
    downroi=max(uproi+1,downroi);
    setTrackbarPos("Bottom side of ROI", "frame Erode2", downroi);
}

void TrackbarROI()
{
    createTrackbar("Left side of ROI", "frame Erode2", &leftroi, max_value_xroi, roi_left);
    createTrackbar("Right side of ROI", "frame Erode2", &rightroi, max_value_xroi, roi_right);
    createTrackbar("Top side of ROI", "frame Erode2", &uproi, max_value_yroi, roi_up);
    createTrackbar("Bottom side of ROI", "frame Erode2", &downroi, max_value_yroi, roi_down);
}



int main() {

//Capture Video From Camera/Source
    VideoCapture cap(0);
    if( !cap.isOpened() ){
        cout << "Camera error" << endl;
        return -1;
    }
    

//Declare window
    namedWindow("frame0", WINDOW_NORMAL);
    namedWindow("frame", WINDOW_NORMAL);
    namedWindow("frame HSV", WINDOW_NORMAL);
    namedWindow("frame In Range", WINDOW_NORMAL);
    namedWindow("frame Erode1", WINDOW_NORMAL);
    namedWindow("frame Dil1", WINDOW_NORMAL);
    namedWindow("frame Dil2", WINDOW_NORMAL);
    namedWindow("frame Erode2", WINDOW_NORMAL);
    namedWindow("contour", WINDOW_NORMAL);


    while(true) {
//Read Video Input from Camera

        cap.read(frame0);

        if(frame0.empty()) {
            cout << "Can't load video" << endl;
            break;
        }

        flip(frame0, frame1, 1);
        
        
/*
//Read Video Input from Source
        cap.read(frame1);

        if(frame1.empty()) {
            cout << "Can't load video" << endl;
            break;
        }
        */
//Create Region Of Interest

        Rect roi(leftroi, uproi, rightroi-leftroi, downroi-uproi);
        frame = frame1(roi);
        

//Draw Region Of Interest on Video Camera
        rectangle( frame1, Point( leftroi, uproi ), Point( rightroi, downroi), Scalar( 0, 255, 255 ));

//Create HSV Frame
        cvtColor(frame, frameHSV, COLOR_BGR2HSV);

//Create Trackbar
        TrackbarHSV();
        TrackbarROI();

//Thresholding
        inRange(frameHSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frameInRange);

//Reducing noise using erode and dilation
        GaussianBlur( frameInRange, frameInRange, Size(3, 3), 0, 0);
        Mat kernel = getStructuringElement(2, Size(10,10));
        erode(frameInRange, frameErode1, kernel);
        dilate(frameErode1, frameDil1, kernel);
        dilate(frameDil1, frameDil2, kernel);
        erode(frameDil2, frameErode2, kernel);

        imshow("frame1", frame1);
        imshow("frame Erode2", frameErode2);

        char key = (char) waitKey(50);
        if(key == 's') {
            break;
        }
    }

    sleep(3);

//Camera opened
    while(true) 
    {
//Read Video Input from Camera

        cap.read(frame0);

        if(frame0.empty()) {
            cout << "Can't load video" << endl;
            break;
        }

        flip(frame0, frame1, 1);
        
        
/*
//Read Video Input from Source
        cap.read(frame1);

        if(frame1.empty()) {
            cout << "Can't load video" << endl;
            break;
        }
        */

//Create Region Of Interest

        Rect roi(leftroi, uproi, rightroi-leftroi, downroi-uproi);
        frame = frame1(roi);
        

//Draw Region Of Interest on Video Camera
        rectangle( frame1, Point( leftroi, uproi ), Point( rightroi, downroi), Scalar( 0, 255, 255 ));

//Create HSV Frame
        cvtColor(frame, frameHSV, COLOR_BGR2HSV);

//Create Trackbar
        TrackbarHSV();
        TrackbarROI();

//Thresholding
        inRange(frameHSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frameInRange);

//Reducing noise using erode and dilation
        Mat kernel = getStructuringElement(2, Size(10,10));
        erode(frameInRange, frameErode1, kernel);
        dilate(frameErode1, frameDil1, kernel);
        dilate(frameDil1, frameDil2, kernel);
        erode(frameDil2, frameErode2, kernel);

//Finding contour on frameErode2 in dynamic array of array
        vector<vector<Point> > contours;
        findContours( frameErode2, contours, RETR_TREE, CHAIN_APPROX_SIMPLE );

//Finding Convex Hull
        vector<vector<Point> >hull( contours.size() );

        for(size_t i = 0; i < contours.size(); i++) {   
            convexHull(contours[i], hull[i]);
        }

//Draw hand contour and convex hull. Checking if gesture is open hand or closed hand
        Mat drawing = Mat::zeros( frameErode2.size(), CV_8UC3 );
        float largestCont = 0;
        float largestHull = 0;
        

        for(size_t i = 0; i < contours.size(); i++) {

            if(contourArea(contours[i]) > 30000) {

                Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
                drawContours( drawing, contours, (int)i, color );
                drawContours( drawing, hull, (int)i, color );

                if(contourArea(contours[i]) > largestCont) {
                    largestCont = contourArea(contours[i]);
                }
                if(contourArea(hull[i]) > largestHull) {
                    largestHull = contourArea(hull[i]);
                }
                

            }
            

        }

//Closed hand gesture send input to the mouse

        if ( largestCont/largestHull > 0.85) {
            system("sleep 0.1 && xdotool click 1");
            chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
            int ms=0;
            while (ms<300)
            {   
                waitKey(30);
                cap >> src;
                process();
                chrono::time_point<chrono::system_clock> fin = chrono::system_clock::now();
                auto milliseconds = chrono::duration_cast<chrono::milliseconds>(fin - start);
                ms = milliseconds.count();
                cout<<ms<<" ms"<<endl;
            }
            
        }
        


//Showing frame window
        imshow("frame1", frame1);
        //imshow("frame", frame);
        //imshow("frame HSV", frameHSV);
        //imshow("frame In Range", frameInRange); 
        //imshow("frame Erode1", frameInRange); 
        //imshow("frame Dil1", frameInRange); 
        //imshow("frame Dil2", frameDil2);
        imshow("frame Erode2", frameErode2);
        imshow("contour", drawing);

        char c = (char) waitKey(25);
        if(c==27){
            break;
        }
    }

    return 0;
}
