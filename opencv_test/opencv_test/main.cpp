#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.hpp>
#include <fstream>
#include <string.h>



using namespace std;

#define BYTE unsigned char

using namespace cv;

Mat Image1 = imread("/Users/sam/Desktop/course/cv/hw/result/bunny/pic1.bmp", IMREAD_GRAYSCALE);
Mat Image2 = imread("/Users/sam/Desktop/course/cv/hw/result/bunny/pic2.bmp", IMREAD_GRAYSCALE);
Mat Image3 = imread("/Users/sam/Desktop/course/cv/hw/result/bunny/pic3.bmp", IMREAD_GRAYSCALE);
Mat Image4 = imread("/Users/sam/Desktop/course/cv/hw/result/bunny/pic4.bmp", IMREAD_GRAYSCALE);
Mat Image5 = imread("/Users/sam/Desktop/course/cv/hw/result/bunny/pic5.bmp", IMREAD_GRAYSCALE);
Mat Image6 = imread("/Users/sam/Desktop/course/cv/hw/result/bunny/pic6.bmp", IMREAD_GRAYSCALE);

Mat img_intensity (6,1,CV_32FC1);     //i(x,y) : intensity of th mth image
Mat s_vector(6,3,CV_32FC1);           //s_vector: vector point from surface to light
int light_info[6][3];                //(light_px,light_py,light_intensity)
Mat n (3,1,CV_32FC1);                //  n is normals
Mat w (6,6,CV_32FC1);
float past_x=0.0;
float past_y=0.0;
Mat n_sum (1,Image1.cols,CV_32FC1,Scalar(0));
Mat record_y (1,Image1.cols,CV_32FC1,Scalar(0));


fstream inputFile,outputFile1,outputFile2,outputFile3;

/*use box filter */
float boxfilter(Mat image, int row, int col){
    
    float sum=0;
    
    if(col-1>=0 && row-1>=0 && col+1<image.cols && row+1<image.rows){
        for(int i=row-1;i<row+2;i++){
            for(int j=col-1;j<col+2;j++){
                sum=sum+image.at<uchar>(i,j);
            }
        }
        sum = sum/9;
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
void getIntensity_box(int row, int col){
    
    img_intensity.at<float>(0,0) = boxfilter(Image1, row, col);
    img_intensity.at<float>(1,0) = boxfilter(Image2, row, col);
    img_intensity.at<float>(2,0) = boxfilter(Image3, row, col);
    img_intensity.at<float>(3,0) = boxfilter(Image4, row, col);
    img_intensity.at<float>(4,0) = boxfilter(Image5, row, col);
    img_intensity.at<float>(5,0) = boxfilter(Image6, row, col);
}

/*get the intensity of each pixel from each image*/
void getIntensity(int row, int col){
    
    img_intensity.at<float>(0,0) = Image1.at<uchar>(row, col);
    img_intensity.at<float>(1,0) = Image2.at<uchar>(row, col);
    img_intensity.at<float>(2,0) = Image3.at<uchar>(row, col);
    img_intensity.at<float>(3,0) = Image4.at<uchar>(row, col);
    img_intensity.at<float>(4,0) = Image5.at<uchar>(row, col);
    img_intensity.at<float>(5,0) = Image6.at<uchar>(row, col);
}

/*get the vector from surface to the light and normalize*/
void getVector(){
    
    for(int i=0;i<6;i++){
        
        int temp_x=(light_info[i][0]);
        int temp_y=(light_info[i][1]);
        int temp_z=(light_info[i][2]);
        float dis = sqrt((temp_x*temp_x)+(temp_y*temp_y)+(temp_z*temp_z));
        //cout << "dis= "<<dis<<endl;
        s_vector.at<float>(i,0) = (temp_x/dis);
        s_vector.at<float>(i,1) = (temp_y/dis);
        s_vector.at<float>(i,2) = (temp_z/dis);

    }
}

/*solve the matrix*/
void solve(){
    
    n=( ( ( ( (s_vector.t()) * (s_vector) ).inv() ) * (s_vector.t()) ) * img_intensity );
    
}

void solve2(){
    
    img_intensity = w* img_intensity;
    s_vector = w* s_vector;
    n=( ( ( ( (s_vector.t()) * (s_vector) ).inv() ) * (s_vector.t()) ) * img_intensity );
    cout << "n \n"<< n<<endl;
    
}

void surface1(int row, int col){
    
    
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
                n_sum.at<float>(0,col) = n_sum.at<float>(0,col-1)+past_x;
            }
        }
        else{
            n_sum.at<float>(0,col)=n_sum.at<float>(0,col)+record_y.at<float>(0,col);
        }
        past_x=temp_x;
        record_y.at<float>(0,col)=temp_y;
        
    }
   

    //float z = sum_x+sum_y;
    float z = n_sum.at<float>(0,col);
    outputFile1 << row<<" "<<col<<" "<<z<<" 255 255 255"<<endl;
    cout << "z\n"<<z<<endl;
}

void surface2(int row, int col){
    
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
                n_sum.at<float>(0,col) = n_sum.at<float>(0,col-1)+past_x;
            }
        }
        else{
            n_sum.at<float>(0,col)=n_sum.at<float>(0,col)+record_y.at<float>(0,col);
        }
        past_x=temp_x;
        record_y.at<float>(0,col)=temp_y;
        
    }
    
    
    //float z = sum_x+sum_y;
    float z = n_sum.at<float>(0,col);
    outputFile2 << row<<" "<<col<<" "<<z<<" 255 255 255"<<endl;
    cout << "z\n"<<z<<endl;
    
}

void surface3(int row, int col){
    
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
                n_sum.at<float>(0,col) = n_sum.at<float>(0,col-1)+past_x;
            }
        }
        else{
            n_sum.at<float>(0,col)=n_sum.at<float>(0,col)+record_y.at<float>(0,col);
        }
        past_x=temp_x;
        record_y.at<float>(0,col)=temp_y;
        
    }
    
    
    //float z = sum_x+sum_y;
    float z = n_sum.at<float>(0,col);
    outputFile3 << row<<" "<<col<<" "<<z<<" 255 255 255"<<endl;
    cout << "z\n"<<z<<endl;
    
}

void writePlyheader(){
    
    outputFile1 << "ply\n";
    outputFile1 << "format ascii 1.0\n";
    outputFile1 << "comment alpha=1.0\n";
    outputFile1 << "element vertex 14400\n";
    outputFile1 << "property float x\n";
    outputFile1 << "property float y\n";
    outputFile1 << "property float z\n";
    outputFile1 << "property uchar red\n";
    outputFile1 << "property uchar green\n";
    outputFile1 << "property uchar blue z\n";
    outputFile1 << "end_header\n";
    
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
    
    outputFile3 << "ply\n";
    outputFile3 << "format ascii 1.0\n";
    outputFile3 << "comment alpha=1.0\n";
    outputFile3 << "element vertex 14400\n";
    outputFile3 << "property float x\n";
    outputFile3 << "property float y\n";
    outputFile3 << "property float z\n";
    outputFile3 << "property uchar red\n";
    outputFile3 << "property uchar green\n";
    outputFile3 << "property uchar blue z\n";
    outputFile3 << "end_header\n";

}

void randomWeight(){
    
    srand((unsigned)time(NULL));
    float random[6];
    float sum_random=0.0;
    for(int i=0;i<6;i++){
        random[i]=(float)rand()/RAND_MAX;
        sum_random=sum_random+random[i];
    }
    
    for(int i=0;i<6;i++){
        random[i]=random[i]/sum_random;
    }
    
    for(int i=0;i<6;i++){
        for(int j=0;j<6;j++){
            if(i!=j){
                w.at<float>(i,j)=0.0;
            }else{
                w.at<float>(i,j)=random[i];
                cout <<"random\n"<<w.at<float>(i,j)<<endl;
            }
        }
    }
}

int main() {
    
    inputFile.open( "/Users/sam/Desktop/course/cv/hw/result/bunny/LightSource.txt" , ios::in );
    outputFile1.open( "/Users/sam/Desktop/course/cv/hw/result/bunny/bunny-surface1-box.ply" , ios::out );
    outputFile2.open( "/Users/sam/Desktop/course/cv/hw/result/bunny/bunny-surface-weight.ply" , ios::out );
    outputFile3.open( "/Users/sam/Desktop/course/cv/hw/result/bunny/bunny-surface-box.ply" , ios::out );

    string line;
    
    /*read the LightSource and store in the light_info*/
    int line_row=0;
    int line_col=0;
    while(getline(inputFile,line)){
        //cout << line.size()<<endl;
        int start=7;
        line_col=0;
        for(int i=7;i<line.size()-1;i++){
            if( line[i]==',' || line[i]==')'){
                string num=line.substr(start,i-start);
                //cout << num<<endl;
                start=i+1;
                light_info[line_row][line_col]=atoi(num.c_str());
                line_col++;
            }
        }
        line_row++;
    }
    
    /*compute the weight*/
    randomWeight();
  
    writePlyheader();
    
    /* only need to calculate once */
    getVector();


    /*Normal Estimation : loop each pixel,each loop compute the intensity of each image at pixel (x,y)*/
    for (int rowIndex = 0; rowIndex < Image1.rows; rowIndex++) {
        for (int colIndex = 0; colIndex < Image1.cols; colIndex++) {
            
            cout << rowIndex<<" "<<colIndex<<endl;
            getIntensity(rowIndex,colIndex);
            solve();
            surface1(rowIndex,colIndex);
        }
    }
    
    
    /*for wieghted least square*/
    for (int rowIndex = 0; rowIndex < Image1.rows; rowIndex++) {
        for (int colIndex = 0; colIndex < Image1.cols; colIndex++) {
            
            cout << rowIndex<<" "<<colIndex<<endl;
            getIntensity(rowIndex,colIndex);
            solve2();
            surface2(rowIndex,colIndex);
        }
    }
    
    /*for box filter*/
    for (int rowIndex = 0; rowIndex < Image1.rows; rowIndex++) {
        for (int colIndex = 0; colIndex < Image1.cols; colIndex++) {
            
            cout << rowIndex<<" "<<colIndex<<endl;
            getIntensity_box(rowIndex,colIndex);
            solve();
            surface3(rowIndex,colIndex);
        }
    }

    
    inputFile.close();
    outputFile1.close();
    outputFile2.close();
    outputFile3.close();

    cout <<"end"<<endl;
    
    return 0;
}
