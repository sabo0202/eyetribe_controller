#include "ofApp.h"

// save compatibility
#if OF_VERSION_MINOR <= 8
    #define ofDrawLine ofLine
    #define ofDrawCircle ofCircle
#endif

void ofApp::setup()
{
    //ofBackground(0, 0, 0);
    ofSetVerticalSync(true);        //フレーム数と実際の描画回数を同期
    ofSetFrameRate(60);             //フレームレートの設定

    //スイッチ画像の読み込み
    button.setup();
    
    //USB camera init
    cam.setDeviceID(1);
    camSize.x = 800;
    camSize.y = 600;
    cam.setup(camSize.x, camSize.y);
    
    //ROS message topic init
    //chatter_pub = n.advertise<std_msgs::Int8>("cmd_dir", 20);
    //chatter_sub = n.subscribe("cmd_dir", 20, &ofApp::handle_chatter_callback, this);
    
    //テキスト出力．デバッグ用
    /*
    myTextFile.open("outPutGazeAngle.txt", ofFile::WriteOnly);
    
    fin.open("kiyohiko_nonmodel_rouka_outPutGazeAngle.txt", ofFile::ReadOnly);
    //ifstream fin("kiyohiko_joystic_rouka_outPutGazeAngle.txt");     //入力ファイルを開く
    if(fin.fail()){  // if(!fin)でもよい。
        cout << "入力ファイルをオープンできません" << endl;
        return 1;
    }
    */
 

    // open port.
    // NOTE: you have to start The EYE TRIBE TRACKER Server befor call open.
    tet.open();
    //tet.open(6555); //<---- if you want change device port

    /*
     API need your screen information, especially for calibration.
     please put in values of your screen parameters: screen index, resolution width/height in pixels, and phisical width/height in meters.
     This parameters can setting even from "EyeTribe UI". If you have already set the parameters via "EyeTrive UI", you do not need call this.
     */
    //tet.setScreen(0, ofGetScreenWidth(), ofGetScreenHeight(), 35.89, 24.71);
}

//ROS message topic subscribe callback
//void ofApp::handle_chatter_callback(const std_msgs::Int8& cmd_dir)
//{
//    dir = cmdDir;
//}

void ofApp::update()
{
    //キャリブレーション
    if (mfCalibrating)
    {
        updateCalibration();
    }
    //後方カメラ映像表示
    if(driveSwitch)
    {
        return;
    }
    cam.update();
}


void ofApp::draw()
{
    timer.launch();    //処理時間計測開始
    
    if (mfCalibrating)
    {
        // draw calibration view (calibration process/calibration result)
        //-----------------------------------------------------------------------------
        drawCalibration();
        
    }
    else
    {
        // get gaze data and server state
        //-----------------------------------------------------------------------------
        const gtl::GazeData GazeData = tet.getGazeData();
        const gtl::ServerState& s = tet.getServerState();
        const gtl::Screen& screen = tet.getScreen();
        const gtl::CalibResult& cr = tet.getCalibResult();
        
        if (driveSwitch)
        {
            // draw background
            //-----------------------------------------------------------------------------
            if (distribution)
            {
                ofBackgroundGradient(ofColor(90), ofColor(0));
                ofDisableAntiAliasing();
            }
            else
            {
                ofBackground(0, 0, 0);
                ofDisableAntiAliasing();
            }
        }
        else
        {
            //draw back monitor
            //-----------------------------------------------------------------------------
            cam.draw(0, 0, ofGetWidth(), ofGetHeight());
        }
        button.draw(isStart, driveSwitch);      //スイッチ画像の表示
        
        /*
        double a, b;
        if (fin >> a >> b){ // ファイルの最後まで読み込んだら終了
            coodinate_buf.x = a; coodinate_buf.y = b;
            cout << coodinate_buf.x << " " << coodinate_buf.y << endl;
            if ((coodinate_buf.x != 0) && (coodinate_buf.y != 0)) {
                coodinate = coodinate_buf;
            }
            driving(coodinate);
            fin >> coodinate_buf.x >> coodinate_buf.y; // 最後に読み込む
            myTextFile << coodinate.x << " " << coodinate.y << endl;
        }
         */
        
        
        /*
        //両眼のデータが正常に取れているとき，注視座標を取得
        if (GazeData.state & gtl::GazeData::GD_STATE_TRACKING_PRESENCE) {
            coodinate_buf = tet.getPoint2dAvg();
            if ((coodinate_buf.x != 0) && (coodinate_buf.y != 0)) {
                coodinate = coodinate_buf;
            }
            driving(coodinate);
        }
        else
        {
            coodinate.x = 640;
            coodinate.y = 400;
            driving(coodinate);
            //cmdDir = 0;     //Stop
            //topic publish
            //chatter_pub.publish(cmd_dir);
            //ros::spinOnce();
        }
         */
        
        
        //マウスで動かすとき
        coodinate_buf.x = mouseX;
        coodinate_buf.y = mouseY;
        if ((coodinate_buf.x != 0) && (coodinate_buf.y != 0)) {
            coodinate = coodinate_buf;
        }
        driving(coodinate);
         

        //注視座標をテキストに保存
        myTextFile << coodinate.x << " " << coodinate.y << endl;
        //myTextFile << "diffTime: " << rosHist.diffTime << endl;

        // draw gaze data
        //-----------------------------------------------------------------------------
        ofFill();
        
        // red circle is raw gaze point
        ofSetColor(ofColor::red);
        ofDrawCircle(tet.getPoint2dRaw(), 5);
        
        // green dot is smoothed gaze point
        ofSetColor(ofColor::green);
        ofDrawCircle(tet.getPoint2dAvg(), 10);
        
        // when fixated is show orenge circle
        if (tet.isFix())
        {
            ofSetColor(ofColor::orange, 100);
            ofDrawCircle(tet.getPoint2dAvg(), 40);
        }
        
        // and draw data from each eyes
        ofNoFill();
        ofSetColor(ofColor::mediumAquaMarine);
        ofDrawCircle(tet.getLeftEyeRaw(), 5);
        ofDrawCircle(tet.getRightEyeRaw(), 5);
        
        ofSetColor(ofColor::purple);
        ofDrawCircle(tet.getLeftEyeAvg(), 5);
        ofDrawCircle(tet.getRightEyeAvg(), 5);
        
        ofSetColor(ofColor::yellow);
        ofDrawCircle(tet.getLeftEyePcenter().x * ofGetWidth(), tet.getLeftEyePcenter().y * ofGetHeight(), 20);
        ofDrawCircle(tet.getRightEyePcenter().x * ofGetWidth(), tet.getRightEyePcenter().y * ofGetHeight(), 20);
        
        // draw the line
        
        if(driveSwitch)
        {
            ofSetColor(ofColor::green);
            ofLine(276, 2, 1004, 2);

            ofSetColor(ofColor::blue);
            ofLine(2, 2, 276, 2);
            ofLine(1004, 2, 1278, 2);
            
            ofSetColor(ofColor::orange);
            ofLine(0, 2, 2, 2);
            ofLine(1278, 2, 1280, 2);
            ofLine(2, 0, 2, 50);
            ofLine(1279, 0, 1279, 50);
            
            ofSetColor(ofColor::purple);
            ofLine(2, 50, 2, 800);
            ofLine(1279, 50, 1279, 800);
        }
        else
        {
            ofSetColor(ofColor::green);
            ofLine(250, 400, 1040, 400);
            
            ofSetColor(ofColor::purple);
            ofLine(250, 0, 250, 800);
            ofLine(1040, 0, 1040, 800);
        }
        
        //デバック用
        stringstream ss;
        ss << "isStart: " << (isStart ? "TRUE" : "FALSE") << endl;
        ss << "driveSwitch: " << (driveSwitch ? "TRUE" : "FALSE") << endl;
        ss << "Elapsed Time: " << duringTime << endl;
        ss << "fps: " << ofGetFrameRate() << endl;
        ss << "Traking_State: " << GazeData.state << endl;
        ss << endl;
        ss << "[ API ]" << endl;
        ss << "timestamp: " << tet.getTimestamp() << endl;
        ss << endl;
        ss << "[ GAZE DATA ]" << endl;
        ss << "is fixed: " << (tet.isFix() ? "TRUE" : "FALSE") << endl;
        ss << "point 2d raw: " << tet.getPoint2dRaw() << endl;
        ss << "point 2d avg: " << tet.getPoint2dAvg() << endl;
        //ss << "smoothedCoordinates: " << smoothedCoordinates.x << " " << smoothedCoordinates.y << endl;
        //ss << "point 2d avg smoothed: " << gazePoint.x << " " << gazePoint.y << endl;
        //ss << "point 2d deg: " << gazeDeg << endl;
        ss << "left eye raw: " << tet.getLeftEyeRaw() << endl;
        ss << "left eye avg: " << tet.getLeftEyeAvg() << endl;
        ss << "left eye pupil size: " << tet.getLeftEyePupilSize() << endl;
        ss << "left eye pupil coordinates normalized: " << tet.getLeftEyePcenter() << endl;
        ss << "right eye raw: " << tet.getRightEyeRaw() << endl;
        ss << "right eye avg: " << tet.getRightEyeAvg() << endl;
        ss << "right eye pupil size: " << tet.getRightEyePupilSize() << endl;
        ss << "right eye pupil coordinates normalized: " << tet.getRightEyePcenter() << endl;
        ss << endl;
        ss << "[ SERVER STATE ]" << endl;
        ss << "framerate: " << s.framerate << endl;
        ss << "is calibrated: " << s.iscalibrated << endl;
        ss << "is calibrating: " << s.iscalibrating << endl;
        ss << "tracker state: " << s.trackerstate << endl;
        ss << "version: " << s.version << endl;
        ss << endl;
        ss << "[ SCREEN ]" << endl;
        ss << "screen index: " << screen.screenindex << endl;
        ss << "screen resolution width in pixels  : " << screen.screenresw << endl;
        ss << "screen resolution height in pixels : " << screen.screenresh << endl;
        ss << "screen physical width in meters  : " << screen.screenpsyw << endl;
        ss << "screen physical height in meters : " << screen.screenpsyh << endl;
        ss << endl;
        ss << "[ CALIBRATION ]" << endl;
        ss << "result: " << (cr.result ? "succeed" : "failed") << endl;
        ss << "average error in degrees: " << cr.deg << endl;
        ss << "average error in degs, left eye: " << cr.degl << endl;
        ss << "average error in degs, right eye: " << cr.degr << endl;
        ss << "number of calibration points: " << cr.calibpoints.size() << endl;
        ss << endl;
        ss << "[ KEY ]" << endl;
        ss << "f: " << "toggle fulscreen" << endl;
        ss << "o: " << "open(connect)" << endl;
        ss << "c: " << "close(disconnect)" << endl;
        ss << "s: " << "starting server" << endl;
        ss << "SPACE: " << "start calibration process" << endl;
        ss << endl;
        ss << "ROS_MASSAGE: " << cmdDir << endl;
        ss << "f_ratio: " << rosHist.f_ratio << endl;
        ss << "diffTime: " << duringTime << endl;
        
        ofSetColor(255, 255, 255);
        ofDrawBitmapString(ss.str(), 20, 20);

    }
    duringTime = timer.elapsed();   //処理時間計測終了
}

void ofApp::driving(const ofPoint coodinate)
{
    int direction;          //移動方向
    rosHist = hist.normalDist(coodinate);   //注視量ヒストグラムを計算
    
    if(rosHist.f_ratio >= 90)
    {
        //timer.launch();    //処理時間計測開始
    }
    if(rosHist.f_ratio <= 10)
    {
        //rosHist.diffTime = timer.elapsed();   //処理時間計測終了
    }
    //myTextFile << "f_ratio: " << rosHist.f_ratio << endl;
    //myTextFile << "---------------------------------------" << endl;
    
    //電動車いすon/off切替
    if (rosHist.start_ratio >= rosHist.threshold)       isStart = true;
    else if (rosHist.stop_ratio >= rosHist.threshold)   isStart = false;

    //走行方向切替
    if (rosHist.front_ratio >= rosHist.threshold)       driveSwitch = true;
    else if (rosHist.back_ratio >= rosHist.threshold)   driveSwitch = false;

    //startスイッチがオン
    if (isStart)
    {
        switch (distribution) {
            case true:  //注意推定モデルを使用する
                //前進
                if (driveSwitch) {
                    //注視量ヒストグラム値に応じて方向を決定 & ROSにトピックをパブリッシュ
                    //画面上部
                    //今注視量が 90％以上
                    if ((rosHist.f_ratio >= rosHist.threshold))
                    {
                        if ((rosHist.f_y > 276) && (rosHist.f_y <= 1004))                   cmdDir = 1;      //Forward
                        else if ((rosHist.f_y > 2) && (rosHist.f_y <= 276))                 cmdDir = 2;      //Forward_Left
                        else if ((rosHist.f_y > 1004) && (rosHist.f_y <= 1278))             cmdDir = 3;      //Forward_Right
                        else if ((rosHist.f_y >= 0) && (rosHist.f_y <= 2))                  cmdDir = 4;      //Diagonal_Left
                        else if ((rosHist.f_y > 1278) && (rosHist.f_y <= 1280))             cmdDir = 5;      //Diagonal_Right
                    }
                    //今注視量が 90％未満 &　前注視量が 10%以上
                    else if ((rosHist.f_ratio < rosHist.threshold) && (rosHist.f_sub_ratio >= rosHist.sub_threshold))
                    {
                        if ((rosHist.f_y > 276) && (rosHist.f_y <= 1004))
                        {
                            if (cmdDir == 1)                   cmdDir = 1;      //Forward
                            else if (cmdDir == 2)              cmdDir = 9;      //Forward_Left_Neutral
                            else if (cmdDir == 3)              cmdDir = 10;     //Forward_Right_Neutral
                            else if (cmdDir == 4)              cmdDir = 11;     //Diagonal_Left_Neutral
                            else if (cmdDir == 5)              cmdDir = 12;     //Diagonal_Right_Neutral
                        }
                        else if ((rosHist.f_y > 2) && (rosHist.f_y <= 276))
                        {
                            if (cmdDir == 1)                   cmdDir = 8;      //Forward_Neutral
                            else if (cmdDir == 2)              cmdDir = 2;      //Forward_Left
                            else if (cmdDir == 3)              cmdDir = 10;     //Forward_Right_Neutral
                            else if (cmdDir == 4)              cmdDir = 11;     //Diagonal_Left_Neutral
                            else if (cmdDir == 5)              cmdDir = 12;     //Diagonal_Right_Neutral
                        }
                        else if ((rosHist.f_y > 1004) && (rosHist.f_y <= 1278))
                        {
                            if (cmdDir == 1)                   cmdDir = 8;      //Forward_Neutral
                            else if (cmdDir == 2)              cmdDir = 9;      //Forward_Left_Neutral
                            else if (cmdDir == 3)              cmdDir = 3;      //Forward_Right
                            else if (cmdDir == 4)              cmdDir = 11;     //Diagonal_Left_Neutral
                            else if (cmdDir == 5)              cmdDir = 12;     //Diagonal_Right_Neutral
                        }
                        else if ((rosHist.f_y >= 0) && (rosHist.f_y <= 2))
                        {
                            if (cmdDir == 1)                   cmdDir = 8;      //Forward_Neutral
                            else if (cmdDir == 2)              cmdDir = 9;      //Forward_Left_Neutral
                            else if (cmdDir == 3)              cmdDir = 10;     //Forward_Right_Neutral
                            else if (cmdDir == 4)              cmdDir = 4;      //Diagonal_Left
                            else if (cmdDir == 5)              cmdDir = 12;     //Diagonal_Right_Neutral
                        }
                        else if ((rosHist.f_y > 1278) && (rosHist.f_y <= 1280))
                        {
                            if (cmdDir == 1)                   cmdDir = 8;      //Forward_Neutral
                            else if (cmdDir == 2)              cmdDir = 9;      //Forward_Left_Neutral
                            else if (cmdDir == 3)              cmdDir = 10;     //Forward_Right_Neutral
                            else if (cmdDir == 4)              cmdDir = 11;     //Diagonal_Left_Neutral
                            else if (cmdDir == 5)              cmdDir = 5;      //Diagonal_Right
                        }
                    }
                    
                    //画面右側
                    //今注視量が 90％以上
                    else if ((rosHist.r_ratio >= rosHist.threshold))
                    {
                        if ((rosHist.r_y >= 0) && (rosHist.r_y <= 50))                    cmdDir = 5;      //Diagonal_Right
                        else if ((rosHist.r_y > 50) && (rosHist.r_y <= 800))              cmdDir = 6;      //Turn_Right
                    }
                    //今注視量が 90％未満 &　前注視量が 10%以上
                    else if ((rosHist.r_ratio < rosHist.threshold) && (rosHist.r_sub_ratio >= rosHist.sub_threshold))
                    {
                        
                        if ((rosHist.r_y >= 0) && (rosHist.r_y <= 50))
                        {
                            if (cmdDir == 5)                    cmdDir = 5;     //Diagonal_Right
                            else if (cmdDir == 6)               cmdDir = 13;    //Turn_right_Neutral
                            
                            cmdDir = 12;     //Diagonal_Right_Neutral
                        }
                        else if ((rosHist.r_y > 50) && (rosHist.r_y <= 800))
                        {
                            if (cmdDir == 5)                    cmdDir = 12;     //Diagonal_Right_Neutral
                            else if (cmdDir == 6)               cmdDir = 6;      //Turn_right
                        }
                    }
                    
                    //画面左側
                    //今注視量が 90％以上
                    else if ((rosHist.l_ratio >= rosHist.threshold))
                    {
                        if ((rosHist.l_y >= 0) && (rosHist.l_y <= 50))                    cmdDir = 4;      //Diagonal_Left
                        else if ((rosHist.l_y > 50) && (rosHist.l_y <= 800))              cmdDir = 7;      //Turn_Left
                    }
                    //今注視量が 90％未満 &　前注視量が 10%以上
                    else if ((rosHist.l_ratio < rosHist.threshold) && (rosHist.l_sub_ratio >= rosHist.sub_threshold))
                    {
                        if ((rosHist.l_y >= 0) && (rosHist.l_y <= 50))
                        {
                            if (cmdDir == 4)                    cmdDir = 4;      //Diagonal_Left
                            else if (cmdDir == 7)               cmdDir = 14;     //Turn_Left_Neutral
                        }
                        else if ((rosHist.l_y > 50) && (rosHist.l_y <= 800))
                        {
                            if (cmdDir == 4)                    cmdDir = 11;     //Diagonal_Left_Neutral
                            else if (cmdDir == 7)               cmdDir = 7;      //Turn_Left
                        }
                    }
                    
                    /*
                    //前注視量が 10%未満
                    else if ((rosHist.f_sub_ratio < rosHist.sub_threshold) || (rosHist.r_sub_ratio < rosHist.sub_threshold) || (rosHist.l_sub_ratio < rosHist.sub_threshold))
                    {
                        cmdDir = 0;     //Stop
                    }
                     */
                    
                    //今注視量が 90％未満 & 前注視量が 10%未満
                    else if (((rosHist.f_ratio < rosHist.threshold) && (rosHist.f_sub_ratio < rosHist.sub_threshold)) || ((rosHist.r_ratio < rosHist.threshold) && (rosHist.r_sub_ratio < rosHist.sub_threshold)) || ((rosHist.l_ratio < rosHist.threshold) && (rosHist.l_sub_ratio < rosHist.sub_threshold)))
                    {
                        cmdDir = 0;     //Stop
                    }
                 
                }
                else  //後進
                {
                    //注視方向に応じて進行方向を決定 & ROSにトピックをパブリッシュ
                    //画面上部
                    //今注視量が 90％以上
                    if (coodinate.y <= 400)
                    {
                        if ((coodinate.x >= 0) && (coodinate.x <= 1280))            cmdDir = 15;      //Back
                    }
                    
                    //画面右側
                    //今注視量が 90％以上
                    if (coodinate.x >= 1040)
                    {
                        if ((coodinate.y > 0) && (coodinate.y <= 800))              cmdDir = 6;      //Turn_Right
                    }
                    
                    //画面左側
                    //今注視量が 90％以上
                    if (coodinate.x <= 250)
                    {
                        if ((coodinate.y > 0) && (coodinate.y <= 800))              cmdDir = 7;      //Turn_Left
                    }
                    
                    //画面内を見ているとき
                    if ((coodinate.y > 400) && (coodinate.x < 1040) && (coodinate.x > 250)) {
                        cmdDir = 0;     //Stop
                    }
                    /*
                    //注視量ヒストグラム値に応じて方向を決定 & ROSにトピックをパブリッシュ
                    //画面上部
                    //今注視量が 90％以上
                    if ((rosHist.f_ratio >= rosHist.threshold))
                    {
                        if ((rosHist.f_y >= 0) && (rosHist.f_y <= 1280))                    cmdDir = 9;      //Back
                    }
                    //今注視量が 90％未満 &　前注視量が 10%以上
                    else if ((rosHist.f_ratio < rosHist.threshold) && (rosHist.f_sub_ratio >= rosHist.sub_threshold))
                    {
                        cmdDir = 10;  //Back_Neutral
                    }
                    //画面右側
                    //今注視量が 90％以上
                    else if ((rosHist.r_ratio >= rosHist.threshold))
                    {
                        if ((rosHist.r_y > 0) && (rosHist.r_y <= 800))              cmdDir = 6;      //Turn_Right
                    }
                    //今注視量が 90％未満 &　前注視量が 10%以上
                    else if ((rosHist.r_ratio < rosHist.threshold) && (rosHist.r_sub_ratio >= rosHist.sub_threshold)){
                        cmdDir = 10;  //Neutral
                    }
                    //画面左側
                    //今注視量が 90％以上
                    else if ((rosHist.l_ratio >= rosHist.threshold))
                    {
                        if ((rosHist.l_y > 0) && (rosHist.l_y <= 800))              cmdDir = 7;      //Turn_Left
                    }
                    //今注視量が 90％未満 &　前注視量が 10%以上
                    else if ((rosHist.l_ratio < rosHist.threshold) && (rosHist.l_sub_ratio >= rosHist.sub_threshold))
                    {
                        cmdDir = 10;  //Neutral
                    }
                    //今注視量が 90％未満 &　前注視量が 10%未満
                    else if (((rosHist.f_ratio < rosHist.threshold) && (rosHist.f_sub_ratio < rosHist.sub_threshold)) || ((rosHist.r_ratio < rosHist.threshold) && (rosHist.r_sub_ratio < rosHist.sub_threshold)) || ((rosHist.l_ratio < rosHist.threshold) && (rosHist.l_sub_ratio < rosHist.sub_threshold)))
                    {
                        cmdDir = 0;     //Stop
                    }
                     */
                }
                break;
                
            case false:     //注意推定モデルを使用しない
                //前進
                if (driveSwitch) {
                    //注視方向に応じて進行方向を決定 & ROSにトピックをパブリッシュ
                    //画面上部
                    //今注視量が 90％以上
                    if (coodinate.y <= 50)
                    {
                        if ((coodinate.x > 276) && (coodinate.x <= 1004))                  cmdDir = 1;      //Forward
                        else if ((coodinate.x > 2) && (coodinate.x <= 276))                cmdDir = 2;      //Forward_Left
                        else if ((coodinate.x > 1004) && (coodinate.x <= 1278))            cmdDir = 3;      //Forward_Right
                        else if ((coodinate.x >= 0) && (coodinate.x <= 2))                 cmdDir = 4;      //Diagonal_Left
                        else if ((coodinate.x > 1278) && (coodinate.x <= 1280))            cmdDir = 5;      //Diagonal_Right
                    }

                    //画面右側
                    //今注視量が 90％以上
                    else if (coodinate.x >= 1080)
                    {
                        if ((coodinate.y >= 0) && (coodinate.y <= 50))                    cmdDir = 5;      //Diagonal_Right
                        else if ((coodinate.y > 50) && (coodinate.y <= 800))              cmdDir = 6;      //Turn_Right
                    }

                    //画面左側
                    //今注視量が 90％以上
                    else if (coodinate.x <= 200)
                    {
                        if ((coodinate.y >= 0) && (coodinate.y <= 50))                    cmdDir = 4;      //Diagonal_Left
                        else if ((coodinate.y > 50) && (coodinate.y <= 800))              cmdDir = 7;      //Turn_Left
                    }

                    //画面内を見ているとき
                    else {
                        cmdDir = 0;     //Stop
                    }
                }
                else  //後進
                {
                    //注視方向に応じて進行方向を決定 & ROSにトピックをパブリッシュ
                    //画面上部
                    //今注視量が 90％以上
                    if (coodinate.y <= 400)
                    {
                        if ((coodinate.x >= 0) && (coodinate.x <= 1280))            cmdDir = 15;      //Back
                    }
                    
                    //画面右側
                    //今注視量が 90％以上
                    if (coodinate.x >= 1040)
                    {
                        if ((coodinate.y > 0) && (coodinate.y <= 800))              cmdDir = 6;      //Turn_Right
                    }
                    
                    //画面左側
                    //今注視量が 90％以上
                    if (coodinate.x <= 250)
                    {
                        if ((coodinate.y > 0) && (coodinate.y <= 800))              cmdDir = 7;      //Turn_Left
                    }
                    
                    //画面内を見ているとき
                    if ((coodinate.y > 400) && (coodinate.x < 1040) && (coodinate.x > 250)) {
                        cmdDir = 0;     //Stop
                    }
                }
                break;
        }
    }
    //startスイッチがオフ
    else
    {
        cmdDir = 0;     //Stop
    }

    //topic publish
    //chatter_pub.publish(cmd_dir);
    //ros::spinOnce();
}

void ofApp::keyPressed(int key)
{
    if (key == 'p' || key == 'P') cam.videoSettings();
    if (key == 'r') isStart = !isStart;
    if (key == 'x') distribution = !distribution;
    if (key == 'f') ofToggleFullscreen();
    if (key == 'o') tet.open();
    if (key == 'c') tet.close();
    if (key == 's') tet.startServer();
    
    if (key == ' ')
    {
        if (mfCalibrating)
        {
            stopCalibrationSequence();
        }
        else {
            bool calibration = startCalibrationSequence(9, 1.0, 2.0, 25.0, true);
            
            if (calibration)
                ofLogNotice("ofxEyeTribe", "start calibration sequence");
            else
                ofLogError("ofxEyeTribe", "calibration could not start");
        }
    }
}



//------------------------------------------------------------------------------------------
//                               Calibration Example
//------------------------------------------------------------------------------------------

static const float CALIB_STAND_BY_TIME  = 3.0;


static float QuintEaseIn(const float t)
{
    return t * t * t * t * t;
}

static float QuintEaseOut(const float t)
{
    return 1. - QuintEaseIn(1. - t);
}

static float ElasticEaseOut(const float t)
{
    float s = 1 - t;
    return 1 - powf(s, 8) + sinf(t * t * 6 * PI) * s * s;
}

static float ElasticEaseIn(const float t)
{
    return 1.0 - ElasticEaseOut(1.0 - t);
}

bool ofApp::startCalibrationSequence(const int numCalibrationPoints,
                                     const float calibPointTime,
                                     const float followPointTime,
                                     const float calibPointSize,
                                     bool random)
{
    // start calibration
    bool calibrating = tet.calibrationStart(numCalibrationPoints);
    
    if (calibrating)
    {
        // initialize
        mCalibPoints.clear();
        mCurrentCalibIndex = 0;
        
        // get screen size from api, if you dont set yet? see setup()
        const int width = tet.getScreen().screenresw;
        const int height = tet.getScreen().screenresh;
        
        // set calibration points
        int nx = 0;
        int ny = 0;
        switch (numCalibrationPoints)
        {
            case 9  : nx = 3; ny = 3; break;
            case 12 : nx = 4; ny = 3; break;
            case 16 : nx = 4; ny = 4; break;
            default : ofLogError("ofxEyeTribe", "illegal number of points"); return false;
        }
        for (int i = 0; i < ny; ++i)
        {
            for (int j = 0; j < nx; ++j)
            {
                float x = width / (nx * 2) * (j * 2 + 1);
                float y = height / (ny * 2) * (i * 2 + 1);
                mCalibPoints.push_back(ofPoint(x, y));
            }
        }
        
        // shuffle order of calibration points
        if (random)
        {
            ofRandomize(mCalibPoints);
        }
        
        // set animation parameters
        mCalibPointSize = abs(calibPointSize);
        mCalibPointTime = abs(calibPointTime);
        mCalibFollowPointTime = followPointTime;
        if (mCalibFollowPointTime < mCalibPointTime * 2)
        {
            ofLogWarning("ofxEyeTribe", "follow point time must more than " + ofToString(mCalibPointTime * 2));
            mCalibFollowPointTime = mCalibPointTime * 2;
        }
        mCalibState = CALIB_START;
        mTick = 0;
        mDuration = CALIB_STAND_BY_TIME;
        mfCalibrating = true;
        return true;
    }
    else {
        mfCalibrating = false;
        return false;
    }
}

void ofApp::stopCalibrationSequence()
{
    // calibration abort
    tet.calibrationAbort();
    mfCalibrating = false;
    mCalibState = CALIB_STAND_BY;
}

void ofApp::updateCalibration()
{
    if (mfCalibrating)
    {
        // update animation
        
        if (mCalibState == CALIB_START)
        {
            if (mTick > mDuration)
            {
                mTick = 0;
                mDuration = mCalibFollowPointTime - mCalibPointTime;
                mCalibState = CALIB_FOLLOW_POINT;
                // -> goto next state
            }
        }
        
        if (mCalibState == CALIB_FOLLOW_POINT)
        {
            if (mTick > mDuration)
            {
                mTick = 0;
                mDuration = mCalibPointTime;
                mCalibState = CALIB_POINT;
                // begin new calib pts
                tet.calibrationPointStart(mCalibPoints[mCurrentCalibIndex]);
                // -> goto next state
            }
        }
        
        if (mCalibState == CALIB_POINT)
        {
            if (mTick > mDuration)
            {
                // end current calib pts
                tet.calibrationPointEnd();
                
                mCurrentCalibIndex++;
                
                if (mCurrentCalibIndex == mCalibPoints.size())
                {
                    mCalibState = CALIB_STAND_BY;
                    mfCalibrating = false;
                    // -> goto end
                }
                else {
                    mCalibState = CALIB_FOLLOW_POINT;
                    mDuration = mCalibFollowPointTime - mCalibPointTime;
                    // -> back to previous state
                }
                mTick = 0;
            }
        }
        
        // count up animation timeline
        mTick += ofGetLastFrameTime();
    }
}

void ofApp::drawCalibration()
{
    ofPushStyle();
    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofSetRectMode(OF_RECTMODE_CORNER);
    ofSetCircleResolution(90);
    
    // get screen size from api
    const int width  = tet.getScreen().screenresw;
    const int height = tet.getScreen().screenresh;
    const float timeInterp = ofClamp(1.0 - (mTick / mDuration), 0.0, 1.0);
    
    // draw animation
    
    if (mCalibState == CALIB_START)
    {
        unsigned char a = QuintEaseOut(timeInterp) * 255;
        ofDrawBitmapStringHighlight("FOLLOW THE CIRCLE", width / 2 - 70, height / 2, ofColor(0, a), ofColor(255, a));
    }
    
    if (mCalibState == CALIB_FOLLOW_POINT)
    {
        ofFill();
        ofSetColor(255, 255, 255);
        const ofPoint& p = mCalibPoints[mCurrentCalibIndex];
        float size = ElasticEaseIn(timeInterp) * (mCalibPointSize * 1.5) + mCalibPointSize;
        ofDrawCircle(p, size);
    }
    
    if (mCalibState == CALIB_POINT)
    {
        ofFill();
        ofSetColor(255, 255, 255);
        const ofPoint& p = mCalibPoints[mCurrentCalibIndex];
        float size = QuintEaseIn(timeInterp) * mCalibPointSize;
        ofDrawCircle(p, size);
    }
    
    if (mCalibState == CALIB_STAND_BY)
    {
        ofFill();
        for (const auto& e : tet.getCalibResult().calibpoints)
        {
            ofFill();
            ofSetColor(ofColor::blue);
            ofDrawCircle(e.cp.x, e.cp.y, 5);
            ofNoFill();
            ofSetColor(ofColor::skyBlue);
            ofDrawCircle(e.mecp.x, e.mecp.y, 5);
        }
    }
    
    ofPopStyle();
}
