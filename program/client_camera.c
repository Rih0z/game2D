#include <stdio.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>
#include "common.h"
#include "client_func.h"
#include <time.h>
#define CV_DISP_WIDTH  50
#define CV_DISP_HEIGHT 50

CharaInfo   *gChara;    /* キャラデータ */
CameraData   *gCam;    /* キャラデータ */
int Camera_r(int pos ,int camflag)
{
  static int x,y;
  static int chara;
  int endflagc= 1 ; //cameraを終了するフラグ 
  static uchar p[3];
  CvCapture *capture = 0;
  IplImage *frame = 0;
  double w = CV_DISP_WIDTH, h = CV_DISP_HEIGHT;
  //double w = WD_Width * MAP_ChipSize ; 
  //double h = WD_Height * MAP_ChipSize ; 

  int ckey;
  if(camflag == 0){
    // カメラのオープン(/dev/video0)
    capture = cvCreateCameraCapture(0);
    if (capture == NULL) {
      fprintf(stderr, "*Error* cannot open /dev/video0\n");
      return -1;
    }

    // キャプチャサイズの設定
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, w);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, h);
    //  cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);

    static double red ,green , blue ;
    static double red_ave ,green_ave , blue_ave ; 
    static float col_all,col_red,col_green,col_blue;
    static double red_c,blue_c ,green_c;
    static int count_cam ;
    // カメラからフレームをキャプチャ
    while (1) {
      int i ;
      for(i =  0 ; i< 3 ; i++)
        p[i] = 0 ;
      frame = cvQueryFrame(capture);
      //150*150の領域になるように条件を定義
      for (y = 0 ; y < h ; y++) {
        for (x = 0 ;  x < w ; x++) {
          //アクセスするピクセルのbgr値を取得
          p[0] = frame->imageData[frame->widthStep * y + x * 3];          // B
          p[1] = frame->imageData[frame->widthStep * y + x * 3 + 1];      // G
          p[2] = frame->imageData[frame->widthStep * y + x * 3 + 2];      // B
          blue = p[0] - '0';
          green = p[1] - '0';
          red = p[2] - '0'; 
          red_c += red ; 
          green_c += green ; 
          blue_c += blue ; 
          count_cam++;
        }
      }
      cvShowImage("Capture", frame);
      ckey = cvWaitKey(10);
      printf("Camera_Butten%d",ckey);
      printf("Camera_Butten%d",(ckey & 0xff));
      /*    
            if(Time_r(2000000)){
            break;
            }*/
            //add20 entar 
      if ((ckey & 0xff) == '\x1b' ||( ckey&0xff) == 10) {
        //  DestroyCWindow();
        printf("Exiting ...\n");
        cvDestroyAllWindows();
        cvDestroyWindow("Capture");

        break; // エスケープキーで終了
        //  endflagc = DestroyCWindow(&capture) ;

      }
    }
    printf("calc...\n");
    red_ave = (red_c /count_cam); 
    green_ave = (green_c /count_cam); 
    blue_ave = (blue_c /count_cam); 
    col_all = red_ave + green_ave + blue_ave;
    printf("end!!!\n");
    //hp 100 speed1-5 1/20 a attack 10 1/10
    //server command のcamsttsみたいな関数でせっと
    gCam[pos].red = (int)(( red_ave/col_all) *CAMERA_PER );
    gCam[pos].green = (int)((blue_ave/col_all)*CAMERA_PER);
    gCam[pos].blue = (int)((green_ave /col_all)*CAMERA_PER);

    //CAMERA_PER *col_red/100


    printf("R ; %d \n G ; %d \n B : %d \n" , gCam[pos].red, gCam[pos].green , gCam[pos].blue);
    printf("TIME IS COME GAME START\n");
    cvReleaseCapture(&capture);
    cvDestroyWindow("Capture");
    //  printf("WIBDOWWWEFFIFJ\n");
    SendCamCommand();
    cvDestroyAllWindows();
    //add19
    return 1 ; 
  }else{
    cvDestroyAllWindows();
    return 0 ;
  }
}

int Time_r(int time)
{
  static clock_t t1,t2;
  static int count = 0;
  if(count == 0 )
  {
    t1 = clock();
    count = 1; 
  }
  t2 = clock();
  if(t2 - t1 > time)
  {
    t1 = t2;
    return 1; 
  }
  return 0;
}
