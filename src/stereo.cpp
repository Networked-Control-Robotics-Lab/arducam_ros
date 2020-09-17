//https://www.itread01.com/content/1547744238.html
#include <ros/ros.h>
#include <image_transport/image_transport.h>
//#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <stdio.h>

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

//static const std::string INPUT = "Input"; //定義輸入視窗名稱
//static const std::string OUTPUT_LEFT = "OUTPUT_LEFT"; //定義輸出視窗名稱
//static const std::string OUTPUT_RIGHT = "OUTPUT_RIGHT"; //定義輸出視窗名稱
class shuangmu2LeftRight
{
private:
  ros::NodeHandle nh; 
  sensor_msgs::ImagePtr msg ;
  image_transport::ImageTransport it;
  image_transport::Publisher pub_left;
  image_transport::Publisher pub_right;
  image_transport::Subscriber shuangmu_image_sub_;
public:
  shuangmu2LeftRight()
  :it(nh) 
  {

    pub_left = it.advertise("/arducam/image_left", 1);
    pub_right = it.advertise("/arducam/image_right", 1);
    shuangmu_image_sub_ = it.subscribe("/arducam/camera/image_raw", 1, &shuangmu2LeftRight::convert_callback,this);

    //cv::namedWindow(INPUT);
    //cv::namedWindow(OUTPUT_LEFT);
    //cv::namedWindow(OUTPUT_RIGHT);
    //publish_test_img();
  }
  ~shuangmu2LeftRight() 
  {
    //cv::destroyWindow(INPUT);
    //cv::destroyWindow(OUTPUT_LEFT);
    //cv::destroyWindow(OUTPUT_RIGHT);
  }
  void publish_test_img(){
    cv::Mat image = cv::imread("/home/zzz/20170415095236604.png", CV_LOAD_IMAGE_COLOR);
    if(image.empty()){
     printf("open error\n");
    }else{
    msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", image).toImageMsg();
    pub_left.publish(msg);
    ros::spinOnce();
    }
  }

  /*這是一個ROS和OpenCV的格式轉換回調函式，將圖象格式從sensor_msgs/Image  --->  cv::Mat*/
  void convert_callback(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
        cv_ptr =  cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8); //將ROS訊息中的圖象資訊提取，生成新cv型別的圖象，複製給CvImage指標
    }
    catch(cv_bridge::Exception& e)  //異常處理
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    image_process(cv_ptr->image); //得到了cv::Mat型別的圖象，在CvImage指標的image中，將結果傳送給處理函式
  }

  cv::Mat image_split(cv::Mat img,CvRect rect){
    int crop_x1 = cv::max(0,rect.x);
    int crop_y1 = cv::max(0,rect.y);
    int crop_x2 = cv::min(img.cols - 1, rect.x + rect.width - 1); 
    int crop_y2 = cv::min(img.rows - 1, rect.y + rect.height - 1);

    return img(cv::Range(crop_y1,crop_y2 + 1),cv::Range(crop_x1,crop_x2 + 1));
  }
  void image_process(cv::Mat img)
  {
    CvRect rect;
    rect.x        = 0;
    rect.y        = 0;
    rect.width    = img.cols/2;
    rect.height = img.rows;

    //cv::Mat I = cv::Mat::zeros(rect.height, rect.width, 0);
    cv::Mat roi_img=image_split(img,rect);
    msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", roi_img).toImageMsg();
    pub_left.publish(msg);
    //cv::imshow(INPUT, img);
    //cv::imshow(OUTPUT_LEFT, roi_img);
    rect.x        = img.cols/2;
    roi_img=image_split(img,rect);
    msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", roi_img).toImageMsg();
    pub_right.publish(msg);
    //cv::imshow(OUTPUT_RIGHT, roi_img);
    //cv::waitKey(5);

  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "rosopencv");
  shuangmu2LeftRight obj;
  ros::spin();

}
