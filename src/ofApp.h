#pragma once

#include "gazeapi.h"
#include "ofxEyeTribe.h"
#include "ofxTimerClass.h"
#include "ofxNormalDistribution.h"
#include "ofxDrawImage.h"

//#include <ros/ros.h>
//#include <std_msgs/Int8.h>

#include <string>

class ofApp : public ofBaseApp
{
public:
    bool driveSwitch = true;        //走行モード切替フラグ
    
    //for USB camera
    bool    isStart = false;
    bool    distribution = true;
    ofPoint camSize;
    ofPoint coodinate;      //注視座標
    ofPoint coodinate_buf;      //注視座標
    
    //for Timer
    double duringTime;
    
    int count = 0;
    
    ofVideoGrabber        cam;          //Video Class
    ofxEyeTribe           tet;          //Eyetribe  Class
    ofxTimer              timer;        //Timer Class
    ofxNormalDistribution hist;         //NormalDistribution Class
    ofxDrawImage          button;       //ButtonImage Class
    ofFile                myTextFile;   //TextFile Class
    ofFile                fin;   //TextFile Class

    //for ROS
    int             argc;
    char            **argv;
    Histgram        rosHist;
    //ros::NodeHandle n;
    //ros::Publisher  chatter_pub;
    //ros::Subscriber chatter_sub;
    //std_msgs::Int8  cmd_dir;
    //std_msgs::Int8  msg_sub;
    int cmdDir;
    int dir;
    
    //for OpenFrameWorks & ROS callback function
    void setup();
    void update();
    void draw();
    void driving(const ofPoint coodinate);
    void keyPressed(int key);
    //void handle_chatter_callback(const std_msgs::Int8& msg);
    
    
    //------------------------------------------------------------------------------------------
    //                               Calibration Example
    //------------------------------------------------------------------------------------------
    /*
        Calibration example.
        ofxEyeTribe has functions fo calibration: calibrationStart(int), calibrationPointStart(), calibrationPointEnd() and calibrationAbort()
        First of all, call calibrationStart(int) and begin calibration mode. You can set the number of points to use for calibration in augument.
        You should draw a point for guide to user's gaze, and guide user's gaze to first/next calibration point.
        Call calibrationPointStart() and begin calibration of gaze point. Wait for few second...
        Call calibrationPointEnd(), and guide user's gaze to next calibration point.
        Repeat N time: N is specified number of points to use for calibration.
        Check consol log, if the message "calibrate succeed" was outputted, your calibration sequence was succeed.
        Try make your functions and animation for calibration sequence.
     */
    
    // values for calibration
    vector<ofPoint> mCalibPoints;
    int             mCurrentCalibIndex;
    bool            mfCalibrating;
    float           mTick;
    float           mDuration;
    enum            { CALIB_STAND_BY, CALIB_START, CALIB_FOLLOW_POINT, CALIB_POINT };
    int             mCalibState;
    float           mCalibPointTime;
    float           mCalibFollowPointTime;
    float           mCalibPointSize;
    
    /**
     *  Begin easily calibration sequence, you have to call updateCalibration() and drawCalibration() after called this.
     *  if you want cancel calibration, call stopCalibrationProcess.
     *
     *  @param numCalibrationPoints The number of points to use for calibration. (only 9/12/16)
     *  @param time of each gaze point calibration.
     *  @param followPointTime Time (sec.) to following for each calibration point (minimum = 1.0)
     *  @param calibPointSize Size (pixel) of each calibration point.
     *  @param random shuffle order of follow points.
     *  @return indication of the request processed okay.
     *  @sa stopCalibrationProcess.
     */
    bool startCalibrationSequence(const int numCalibrationPoints,
                                  const float calibPointTime,
                                  const float followPointTime,
                                  const float calibPointSize,
                                  bool random);
    
    /**
     *  Stop calibration sequence.
     *  @sa startCalibrationSequence.
     */
    void stopCalibrationSequence();
    
    void updateCalibration();
    void drawCalibration();
};

