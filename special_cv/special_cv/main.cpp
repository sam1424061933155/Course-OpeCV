#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.hpp>
#include <fstream>
#include <string.h>

using namespace std;

#define BYTE unsigned char

using namespace cv;

Mat Image1 = imread("/Users/sam/Desktop/course/cv/hw/result/special/bunny/pic1.bmp", IMREAD_GRAYSCALE);
Mat Image2 = imread("/Users/sam/Desktop/course/cv/hw/result/special/bunny/pic2.bmp", IMREAD_GRAYSCALE);
Mat Image3 = imread("/Users/sam/Desktop/course/cv/hw/result/special/bunny/pic3.bmp", IMREAD_GRAYSCALE);
Mat Image4 = imread("/Users/sam/Desktop/course/cv/hw/result/special/bunny/pic4.bmp", IMREAD_GRAYSCALE);
Mat Image5 = imread("/Users/sam/Desktop/course/cv/hw/result/special/bunny/pic5.bmp", IMREAD_GRAYSCALE);
Mat Image6 = imread("/Users/sam/Desktop/course/cv/hw/result/special/bunny/pic6.bmp", IMREAD_GRAYSCALE);

Mat new_img1,new_img2,new_img3,new_img4,new_img5,new_img6;



Mat img_intensity (6,1,CV_32FC1);
Mat s_vector(6,3,CV_32FC1);
int light_info[6][3];
Mat n (3,1,CV_32FC1);

Mat n_sum (1,Image1.cols,CV_32FC1,Scalar(0));


fstream inputFile,outputFile1,outputFile2;


/*get the vector from surface to the light and normalize*/
void getVector(){
    
    for(int i=0;i<6;i++){
        int temp_x=(light_info[i][0]);
        int temp_y=(light_info[i][1]);
        int temp_z=(light_info[i][2]);
        float dis = sqrt((temp_x*temp_x)+(temp_y*temp_y)+(temp_z*temp_z));
        s_vector.at<float>(i,0) = (temp_x/dis);
        s_vector.at<float>(i,1) = (temp_y/dis);
        s_vector.at<float>(i,2) = (temp_z/dis);
    }
}

/*solve the matrix*/
void solve(){
    
    
    n=( ( ( ( (s_vector.t()) * (s_vector) ).inv() ) * (s_vector.t()) ) * img_intensity );
    
}

void surface_median(int row, int col){
    
    
    float temp_x,temp_y;
    
    if(n.at<float>(2.0)==0.0){
        temp_x=0.0;
        temp_y=0.0;
        cout << "in equal zero"<<endl;
    }else{
        
        temp_x =-(n.at<float>(0,0)) / (n.at<float>(2,0));
        temp_y =-( n.at<float>(1,0)) / (n.at<float>(2,0));
        
        if(row==0){
            if(col!=0){
                n_sum.at<float>(0,col) = n_sum.at<float>(0,col-1)+temp_x;
            }
        }
        else{
            n_sum.at<float>(0,col)=n_sum.at<float>(0,col)+temp_y;
        }
        
    }

    float z = n_sum.at<float>(0,col);
    outputFile2 << row<<" "<<col<<" "<<z<<" 255 255 255"<<endl;
    //cout << "z\n"<<z<<endl;
    
}

void surface_gaussian(int row, int col){
    
    
    float temp_x,temp_y;
    
    if(n.at<float>(2.0)==0.0){
        temp_x=0.0;
        temp_y=0.0;
        cout << "in equal zero"<<endl;
    }else{
        
        temp_x =-(n.at<float>(0,0)) / (n.at<float>(2,0));
        temp_y =-( n.at<float>(1,0)) / (n.at<float>(2,0));
        
        if(row==0){
            if(col!=0){
                n_sum.at<float>(0,col) = n_sum.at<float>(0,col-1)+temp_x;
            }
        }
        else{
            n_sum.at<float>(0,col)=n_sum.at<float>(0,col)+temp_y;
        }
        
    }
    
    float z = n_sum.at<float>(0,col);
    outputFile1 << row<<" "<<col<<" "<<z<<" 255 255 255"<<endl;
    //cout << "z\n"<<z<<endl;
    
}


void writePlyheader(){
    
    /*outputFile1 << "ply\n";
    outputFile1 << "format ascii 1.0\n";
    outputFile1 << "comment alpha=1.0\n";
    outputFile1 << "element vertex 14400\n";
    outputFile1 << "property float x\n";
    outputFile1 << "property float y\n";
    outputFile1 << "property float z\n";
    outputFile1 << "property uchar red\n";
    outputFile1 << "property uchar green\n";
    outputFile1 << "property uchar blue z\n";
    outputFile1 << "end_header\n";*/
    
    outputFile2 << "ply\n";
    outputFile2 << "format ascii 1.0\n";
    outputFile2 << "comment alpha=1.0\n";
    outputFile2 << "element vertex 14400\n";
    outputFile2 << "property float x\n";
    outputFile2 << "property float y\n";
    outputFile2 << "property float z\n";
    outputFile2 << "property uchar red\n";
    outputFile2 << "property uchar green\n";
    outputFile2 << "property uchar blue z\n";
    outputFile2 << "end_header\n";
    
}
float filter(Mat image,int row, int col){
    
  
    float sum=0;
    
    if(col-1>=0 && row-1>=0 && col+1<image.cols && row+1<image.rows){
        cout <<"each value"<<endl;
        cout <<(float)image.at<uchar>(row,col-1)<<" "<<(float)image.at<uchar>(row,col+1)<<" "<<(float)image.at<uchar>(row+1,col)<<" "<<(float)image.at<uchar>(row,col)<<endl;
        float temp_top=(float)image.at<uchar>(row,col-1)*0.125;
        float temp_down=(float)image.at<uchar>(row,col+1)*0.125;
        float temp_left=(float)image.at<uchar>(row-1,col)*0.125;
        float temp_right=(float)image.at<uchar>(row+1,col)*0.125;
        float temp_center=(float)image.at<uchar>(row,col)*0.5;
        cout <<"top = "<<temp_top<<" down = "<<temp_down<<" left = "<<temp_left<<" right = "<<temp_right<<endl;
        sum = temp_top+temp_down+temp_left+temp_right+temp_center;
        if(sum>255){
            sum=255;
        }
        if(sum<0){
            sum=0;
        }
    }else{
        sum = image.at<uchar>(row,col);
    }
    cout <<"the point value is = "<<sum<<endl;
    
    return sum;
}


/*get the intensity of each pixel from each image*/
void getIntensity_median(int row, int col){
    
    
    /*img_intensity.at<float>(0,0) = filter(Image1,row,col);
    img_intensity.at<float>(1,0) = filter(Image2,row,col);
    img_intensity.at<float>(2,0) = filter(Image3,row,col);
    img_intensity.at<float>(3,0) = filter(Image4,row,col);
    img_intensity.at<float>(4,0) = filter(Image5,row,col);
    img_intensity.at<float>(5,0) = filter(Image6,row,col);*/
    
     img_intensity.at<float>(0,0) = new_img1.at<uchar>(row,col);
     img_intensity.at<float>(1,0) = new_img2.at<uchar>(row,col);
     img_intensity.at<float>(2,0) = new_img3.at<uchar>(row,col);
     img_intensity.at<float>(3,0) = new_img4.at<uchar>(row,col);
     img_intensity.at<float>(4,0) = new_img5.at<uchar>(row,col);
     img_intensity.at<float>(5,0) = new_img6.at<uchar>(row,col);
    
    //cout << "img intensity = "<<img_intensity<<endl;
}
void getIntensity_gaussian(int row, int col){
    
    
     img_intensity.at<float>(0,0) = filter(Image1,row,col);
     img_intensity.at<float>(1,0) = filter(Image2,row,col);
     img_intensity.at<float>(2,0) = filter(Image3,row,col);
     img_intensity.at<float>(3,0) = filter(Image4,row,col);
     img_intensity.at<float>(4,0) = filter(Image5,row,col);
     img_intensity.at<float>(5,0) = filter(Image6,row,col);
    
}



int main() {
    
    inputFile.open("/Users/sam/Desktop/course/cv/hw/result/special/bunny/LightSource.txt" , ios::in );
    //outputFile1.open("/Users/sam/Desktop/course/cv/hw/result/special/bunny/bunny-surface-gaussian.ply" , ios::out );
    outputFile2.open("/Users/sam/Desktop/course/cv/hw/result/special/bunny/bunny-surface-median.ply" , ios::out );

    string line;
    
    /*read the LightSource and store in the light_info*/
    int line_row=0;
    int line_col=0;
    while(getline(inputFile,line)){
        int start=7;
        line_col=0;
        for(int i=7;i<line.size()-1;i++){
            if( line[i]==',' || line[i]==')'){
                string num=line.substr(start,i-start);
                cout << num<<endl;
                start=i+1;
                light_info[line_row][line_col]=atoi(num.c_str());
                line_col++;
            }
        }
        line_row++;
    }
    
    
   

    writePlyheader();
    /* only need to calculate once */
    getVector();
    
    /*for (int rowIndex = 0; rowIndex < Image1.rows; rowIndex++) {
        for (int colIndex = 0; colIndex < Image1.cols; colIndex++) {
            
            cout << rowIndex<<" "<<colIndex<<endl;
            cout <<"--------------------------"<<endl;
            getIntensity_gaussian(rowIndex,colIndex);
            solve();
            surface_gaussian(rowIndex,colIndex);
        }
    }*/

    
    
    //mediablur
    medianBlur(Image1, new_img1, 3);
    medianBlur(Image2, new_img2, 3);
    medianBlur(Image3, new_img3, 3);
    medianBlur(Image4, new_img4, 3);
    medianBlur(Image5, new_img5, 3);
    medianBlur(Image6, new_img6, 3);
    
    
    /*Normal Estimation : loop each pixel,each loop compute the intensity of each image at pixel (x,y)*/
    for (int rowIndex = 0; rowIndex < Image1.rows; rowIndex++) {
        for (int colIndex = 0; colIndex < Image1.cols; colIndex++) {
            
            cout << rowIndex<<" "<<colIndex<<endl;
            cout <<"--------------------------"<<endl;
            getIntensity_median(rowIndex,colIndex);
            solve();
            surface_median(rowIndex,colIndex);
        }
    }
    
    
    inputFile.close();
    //outputFile1.close();
    outputFile2.close();
    
    cout <<"end"<<endl;
    
    return 0;
}
