#include "my_CV.h"
//get outline using canny
//debugcode
Mat drawLines2(Mat frame, Point &ptl1, Point &ptl2)
{
    Mat line_img(frame.rows,frame.cols,frame.type(), cv::Scalar(0));
    line(line_img, ptl1, ptl2, Scalar(0, 0, 255),3); // draw line in raw image
    //line(line_img, ptr1, ptr2, Scalar(0, 0, 255),3); // draw line in raw image
    addWeighted(frame, 1.0, line_img, 1.0, 0.0, frame);
    return frame;
}
//debug end
Mat myCanny(Mat &frame)
{
	///***********************************************************************
	//original Canny
	
	Mat grayImage, edgeImage;
    cvtColor(frame,grayImage,COLOR_BGR2GRAY); // convert color image
	GaussianBlur(grayImage, grayImage, Size(5, 5), 0.5);
    Canny(grayImage,edgeImage,40,350,3); // using canny get outline
    return edgeImage;
	
	//************************************************************************/
	//gpu canny
	/**
	Mat grayImage;
  cvtColor(frame,grayImage,COLOR_BGR2GRAY);
  GaussianBlur(grayImage, grayImage, Size(5, 5), 0.5);
  
  
  
  
	  cv::cuda::GpuMat d_src(grayImage);
	cv::cuda::GpuMat d_dst;

	cv::cuda::bilateralFilter(d_src, d_dst, -1, 50, 7);
	Ptr<cuda::CannyEdgeDetector> canny = cuda::createCannyEdgeDetector( 35.0, 200.0);
	canny->detect(d_src, d_dst);
	
	Mat edgeImage(d_dst);
	return edgeImage;
	*/
}
Mat getROI(Mat& origImag, Point2f topRight, Point2f topLeft, Point2f botLeft, Point2f botRight){
    Mat black(origImag.rows, origImag.cols, origImag.type(), cv::Scalar::all(0));
    Mat mask(origImag.rows, origImag.cols, CV_8UC1, cv::Scalar(0));
    vector< vector<Point> >  co_ordinates;
    co_ordinates.push_back(vector<Point>());
    co_ordinates[0].push_back(topRight);
	co_ordinates[0].push_back(topLeft);
    co_ordinates[0].push_back(botLeft);
    co_ordinates[0].push_back(botRight);
    drawContours( mask,co_ordinates,0, Scalar(255),CV_FILLED, 8 );

    origImag.copyTo(black,mask);
    return black;
}
//hough transform
void myHough(Mat edgeImage, vector<Vec4i> &lines)
{
	/*********************************************************************************************
	//original hough
    Vec4i params;
    int x1,y1,x2,y2;
    HoughLinesP(edgeImage,lines, 1, CV_PI/180.0, 30, 40, 25);// get lines using hough transform
    return;
	************************************************************************************************/
	cv::cuda::GpuMat d_src(edgeImage);
	cv::cuda::GpuMat d_lines;

	Ptr<cuda::HoughSegmentDetector> hough = cuda::createHoughSegmentDetector(1.0f, (float)(CV_PI / 180.0f), 50, 5);
	hough -> detect(d_src, d_lines);

	if(!d_lines.empty())
	{
		lines.resize(d_lines.cols);
		Mat h_lines(1, d_lines.cols, CV_32SC4, &lines[0]);
		d_lines.download(h_lines);
	}
	return;
}
void groupLines(vector<Vec4i> &lines, vector<int> &left_line_x, vector<int> &left_line_y, vector<int> &right_line_x, vector<int> &right_line_y,Mat &image)
{
    Vec4i params;
    int x1,y1,x2,y2;
    double slope;
    int lineLen = lines.size();
    for(int k=0;k<lineLen;k++)
    {
        //grouping!!
        //convert r,theta coordinate or
        //clustring
        params = lines[k]; // straght line information insert param
        x1 = params[0];
        y1 = params[1];
        x2 = params[2];
        y2 = params[3];
        
        
        //***DongHyeonCode****///
        Point pt1(x1,y1);
        Point pt2(x2,y2);
        
        int dis_x=abs(x1-x2);//----- distance
        int dis_y=abs(y1-y2);// l dist
        
        
        //************************
       ////if((y1<=730)||(y2<=730))
		////   continue;
        
        
        
        
        if(x2 != x1)
        {
			
			 //******************************DongHeyonCode******************************************//
            if((dis_x>dis_y)&&(dis_x>50))//I want to get a horizental line (-----) line
            {
				
				if(dis_y<100)// I want to get a horizental line (-----) line
				{
				      //***********DongHeyonCode**********************
				
					slope = ((double)y2 - y1)/(x2-x1); //get slope
					//cout<<slope<<endl;//testcode
					if(fabs(slope)>20 || fabs(slope)<0.03)
						continue;
					else if(slope<0)    // leftLine
					{
						//debug 
						//cout<<"left"<<fabs(slope)<<"\n";
						//
						left_line_x.push_back(x1);
						left_line_x.push_back(x2);
						left_line_y.push_back(y1);
						left_line_y.push_back(y2);
					//DebugCode
				//		line(image, pt1, pt2, Scalar(255, 0, 0),3); // draw line in raw image
				//
					}
					else    //rightLine
					{
						//debug
						//cout<<"right"<<fabs(slope)<<"\n";
						//
						right_line_x.push_back(x1);
						right_line_x.push_back(x2);
						right_line_y.push_back(y1);
						right_line_y.push_back(y2);
						//debugcode
				//	    line(image, pt1, pt2, Scalar(255, 0, 0),3); // draw line in raw image
						//
					}
				}
			}
		}
        else
        {
            //slope is infinit
        }
    }
}
void myransac(vector<int> x,vector<int> y,Point &pt1, Point &pt2,int imageCols)
{
    //gradient -> using r, theta
    double gradient, distance, yIC;
    double resultn = log(1-0.99)/log(1-(0.5*0.5));//store n
    int r1,r2;
    int dataNum = x.size();
    int threshold = 5;
    int inlier = 0, inMax = 0;
    int x1,y1,x2,y2;
    int px1,py1 = 2000,px2,py2 = 2000;
    if(x.empty() || y.empty())
	{
		//debug
		cout<<"1st dump"<<'\n';
		//
       if(imageCols == 300)
	   {
		 	  pt1 = Point(0, 1080);
			  pt2 = Point(0, 780);
	    }
		else
		{
			   pt1 = Point(1920,780);
			   pt2 = Point(1920,1080);
		}
         return;
      }
	  for(int i=0;i<resultn;i++)
      {
		int k=0;
		do{
			k++;
            r1 = rand()%(dataNum);
            r2 = ((rand()%(dataNum))+r1)%(dataNum);            
        	if(k>100)
			{
				cout<<"using break"<<endl;
				break;
			}
		}while(r1 == r2 || x[r1] == x[r2] || (x[r1]>500 && x[r1] < 1300) || (x[r2]>500&&x[r2]<1300));
        //while(r1 == r2 || x[r1] == x[r2])
        gradient = ((double)(y[r2]-y[r1]))/(x[r2]-x[r1]);
		yIC = y[r1] - (gradient * x[r1]);
		for(int k=0;k<dataNum;k++)
        {
            if(k!=r1 && k!=r2)
            {
                distance = abs(-(x[k]*gradient) - yIC + y[k])/sqrt((gradient*gradient)+1);
                if(distance < threshold)
                {
                    inlier++;
                }
            }
        }
		if(inlier>inMax)
        {
          inMax = inlier;
          px1 = x[r1];
          py1 = y[r1];
          px2 = x[r2];
          py2 = y[r2];
			    /*
            int y1 = imageRows;
            int x1 = (int)((y1-yIC)/gradient);
            int y2 = (imageRows*3)/4;
            int x2 = (int)((y2-yIC)/gradient);
			    */
          /*      
			    int x1 = imageCols - 300;
			    int y1 = (int)((gradient*x1)+yIC);
			    int x2 = imageCols;
			    int y2 = (int)((gradient*x2)+yIC);
          if(y1 < 1081 && y2 <1081)
			    {
			    	pt1 = Point(x1, y1);
            pt2 = Point(x2, y2);
			    }
			    else
			    {
				      if(imageCols == 300)
				      {
				    	  pt1 = Point(0, 1080);
					      pt2 = Point(0, 780);
				      }
				      else
				      {
				      	pt1 = Point(1920,780);
				      	pt2 = Point(1920,1080);
				      }
			    }
           */
		    	//cout<<"select point : (x1 = "<< x[r1] <<", y1 = "<<y[r1]<<")   ( x2 = "<<x[r2]<<", y2 = "<<y[r2]<<" )"<<endl; 
			    //cout<<"gradient : "<<gradient<<" yIC : "<<yIC<<endl;//debug
			    //cout<<"ransac line : "<<x1<<", "<<y1<<"    "<<x2<<", "<<y2<<endl;//debug code
       }
       inlier = 0;
   }
   if(py1 < 1081 && py2 < 1081)
   {
  	  pt1 = Point(px1, py1);
      pt2 = Point(px2, py2);
	  //debug
	  //cout<<"point1 = ("<<px1<<", "<<py1<<'\n';
	  //cout<<"point2 = ("<<px2<<", "<<py2<<'\n';
   }
   else
   {
	   //debug
	   cout<<"2nd dump"<<'\n';
	   cout<<"py1 = "<<py1 <<", py2 = "<<py2<<"\n";
	   //
       if(imageCols == 300)
	   {
		    pt1 = Point(0, 1080);
		    pt2 = Point(0, 780);
		}
		else
		{
		   	pt1 = Point(1920,780);
		   	pt2 = Point(1920,1080);
        }
   }
}
double getControl(Point &ptl1, Point &ptl2, Point &ptr1, Point &ptr2)
{
    //bp -> ptl ap -> ptr
    int ox,oy;
    double steeringInfo;
	int a1=ptl1.x ,b1=ptl1.y ,a2=ptl2.x ,b2=ptl2.y;
	int n1=ptr1.x ,m1=ptr1.y ,n2=ptr2.x ,m2=ptr2.y;

	if((a1==a2)&&(n1==n2))
	{
		steeringInfo = 90;
	}
	else if(a1==a2)
	{
		steeringInfo = 180;
	}
	else if(n1==n2)
	{
		steeringInfo = 0;
	}
	else
	{
		ox = ((a1-a2)*(n1-n2)*(m2-b2) - (a1-a2)*(m1-m2)*n2 + (n1-n2)*(b1-b2)*a2)/((n1-n2)*(b1-b2) - (a1-a2)*(m1-m2));
		oy = ( (b1-b2)*ox+(a1-a2)*b2-(b1-b2)*a2 )/(a1-a2);
    	steeringInfo = atan2((gLength - oy), (ox - (gWidth/2))) * 180.0/PI;
	}
	/*
    static double current_angle;
    static double PID_control;
    double mid1 = (ptl1.x + ptr1.x)/2.0;
    double mid2 = (ptl2.x + ptr2.x)/2.0;
    current_angle = atan2((ptl1.y - ptl2.y), (mid2 - mid1)) * 180.0/PI;
	  PID_control = current_angle;
	  
    if(control_cnt) // first_time
    {
        PID_control = desired_angle - current_angle;
        time(&pre_time);
        pre_error = desired_angle - current_angle;
        control_cnt = 0;
    }
    else //first time
    {
        time(&cur_time);
        del_time = difftime(cur_time,pre_time);
        time(&pre_time);

        error = desired_angle - current_angle;
        P_control = Kp * error;
        I_control += Ki * error * del_time;
        D_control = Kd*(error - pre_error)/del_time;
        PID_control = P_control + I_control + D_control;
        pre_error = error;
    }
	  **/
    return steeringInfo;
}
Mat drawLines(Mat frame, Point &ptl1, Point &ptl2, Point &ptr1, Point &ptr2)
{
    Mat line_img(frame.rows,frame.cols,frame.type(), cv::Scalar(0));
    line(line_img, ptl1, ptl2, Scalar(0, 0, 255),3); // draw line in raw image
    line(line_img, ptr1, ptr2, Scalar(0, 0, 255),3); // draw line in raw image
    addWeighted(frame, 0.8, line_img, 1.0, 0.0, frame);
    return frame;
}
int imageProcess(Mat frame)
{
    vector<Vec4i> lines; // get line information in vector
    vector<int> left_line_x;
    vector<int> left_line_y;
    vector<int> right_line_x;
    vector<int> right_line_y;
    Point ptl1, ptl2, ptr1, ptr2;
    Mat roiImage, edgeImage, linedImage;
    gWidth = frame.cols, gLength = frame.rows;
    /*//hsv start
    Scalar hsv_black_min = cvScalar(41, 0, 0);
    Scalar hsv_black_max = cvScalar(230, 255, 115);
    cvtColor(frame, hsvImage, COLOR_BGR2HSV); // frame -> hsvImage�� HSV��
    inRange(hsvImage, hsv_black_min, hsv_black_max, hsvImage);//hsv->dstImage �÷�����
    //hsv end*/
	edgeImage = myCanny(frame);
    roiImage = getROI(edgeImage,Point2f(frame.cols, frame.rows*3/5),Point2f(0, frame.rows*3/5),Point2f(0,frame.rows),Point2f(frame.cols,frame.rows));
    myHough(roiImage, lines);
	//debug
	/*
		Mat houghImage = roiImage;
		for(int i=0;i<lines.size();i++)
		{
			Vec4i param = lines[i];
			const int x1= param[0], y1=param[1], x2=param[2], y2=param[3];
			Point p1(x1,y1), p2(x2,y2);
			houghImage = drawLines2(houghImage,p1,p2);
		}
	//end */
    //debug code
	groupLines(lines, left_line_x, left_line_y, right_line_x, right_line_y,frame);
	//cout<<"geoup line size : "<<left_line_x.size()<<'\n';

	//end */
	//RANSAC
    myransac(left_line_x,left_line_y,ptl1, ptl2, 300);
    myransac(right_line_x,right_line_y,ptr2, ptr1, frame.cols);
	//cout<<"ptl1 = ("<<ptl1.x<<", "<<ptl1.y<<"),  ptl2 = ("<<ptl2.x<<", "<<ptl2.y<<")\n";
	//cout<<"ptr1 = ("<<ptr1.x<<", "<<ptr1.y<<"),  ptr2 = ("<<ptr2.x<<", "<<ptr2.y<<")\n";
    /////linedImage = drawLines(frame, ptl1, ptl2, ptr1, ptr2);
	/*******testcode**************************************
	Mat halfedge, halfroi, halfline;
	resize(edgeImage,halfedge,Size(edgeImage.cols/2,edgeImage.rows/2));
	resize(roiImage,halfroi,Size(roiImage.cols/2,roiImage.rows/2));
	resize(linedImage,halfline,Size(linedImage.cols/2,linedImage.rows/2));
	imshow("edge",halfedge);
	imshow("roi",halfroi);
	imshow("line",halfline);

	// *******testcode**************************************/
	//cout<<"image's width = "<<linedImage.cols<<"  image's rows = "<<linedImage.rows <<"\n";
    //imshow("edge",edgeImage);
	//////imshow("roi",roiImage);
	///////imshow("frame",frame);
	//imshow("line",linedImage);
	//imshow("group",groupImage);
    double steeringInfo = getControl(ptl1, ptl2, ptr1, ptr2);
	waitKey(1);
    return (int)steeringInfo;
}
