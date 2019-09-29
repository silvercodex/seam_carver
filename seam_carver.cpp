#include "seam_carver.h"
#include <cstdlib>
#include <algorithm>
#include <string>


std::string type2str(int type) {
  std::string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

void verticle_seam(Mat image_old, std::vector<int>& path)
{
    Mat image,y,x, gradient,M;
    //std::vector<int> path; 
    path.empty();
    image = Mat(image_old.size(), CV_8UC1);
    
    image_old.convertTo(image_old,CV_8UC3);
    std::cout << path.size() << std::endl << std::endl;
    //std::cout << type2str(image.type()) << std::endl << std::endl;
    //std::cout << type2str(image_old.type()) << std::endl << std::endl;
    //std::cout << image_old.type() << std::endl;
    cvtColor( image_old, image, COLOR_BGR2GRAY);
    //std::cout << image.type() << std::endl;
    //creating matrices to calculate th gradient
    gradient = Mat(image.rows,image.cols,CV_32F, Scalar(0));

    copyMakeBorder(image,y,1,0,0,0,BORDER_CONSTANT,Scalar(0));
    copyMakeBorder(image,x,0,0,1,0,BORDER_CONSTANT,Scalar(0));

    for( int i = 0; i < image.rows; i++ ) {
        for( int j = 0; j < image.cols; j++ ) {
            gradient.at<float>(i,j) = abs(y.at<uchar>(i+1,j) - y.at<uchar>(i,j)) + abs(x.at<uchar>(i,j+1) - x.at<uchar>(i,j)) ;
        }
    }
    M = Mat(gradient);

    //calculating the dynamic programming matric for optimum seam carving
    for( int i = 1; i < image.rows; i++ ) {
        for( int j = 0; j < image.cols; j++ ) {
            if(j+1 >= image.cols){
                M.at<float>(i,j) =  gradient.at<float>(i,j) + min(M.at<float>(i-1,j-1), M.at<float>(i-1,j));
            } else if(j-1 <=0){
                M.at<float>(i,j) =  gradient.at<float>(i,j) + min(M.at<float>(i-1,j), M.at<float>(i-1,j+1));
            } else{
                M.at<float>(i,j) =  gradient.at<float>(i,j) + min(min(M.at<float>(i-1,j-1), M.at<float>(i-1,j)),M.at<float>(i-1,j+1));
            }
        }
    }


    Mat m(M.row(M.rows-1));
    std::vector<float> last(m.begin<float>(), m.begin<float>() + m.cols);
    float MIN = *std::min_element(last.begin(), last.end());
    
    
    std::vector<std::vector<int>> end;
    for(int i = 0; i < last.size();i++){
        if(last[i] == MIN){
            std::vector<int> v;
            v.push_back(i);
            end.push_back(v);
        }
    }
    last.clear();
    if(end.size() == 1){
        path.push_back(end[0][0]);
    } else {
        std::cout << end.size() << std::endl;
        bool broke = false;
        for( int i = image.rows - 2; i >=0; i--){
            for(int j = 0; j < end.size();j++){
                MIN = 1000000;
                int column = end[j].back();
                int current = column;
                if(column +1 < image.cols){
                    if (M.at<float>(i,column+1) < MIN){
                        current = column+1;
                        MIN = M.at<float>(i,column+1);
                    }
                }
                if(column -1 > 0){
                    if (M.at<float>(i,column-1) < MIN){
                        current = column-1;
                        MIN = M.at<float>(i,column-1);
                    }
                }
                if (M.at<float>(i,column) < MIN){
                        current = column;
                        MIN = M.at<float>(i,column);
                    }
                end[j].push_back(current);
            }
            
            std::vector<int> drop;

            MIN = 10000000;

            for(int j = 0; j < end.size();j++){
                if (end[j].back() < MIN) MIN = end[j].back();
            }
            for(int j = 0; j < end.size();j++){
                if(end[j].back() != MIN) drop.push_back(j);
            }
            for(int j = drop.size()-1; j >0;j--){
                end.erase(end.begin() + drop[j]);
            }
            if(end.size() == 1){
                path.push_back(end[0][0]);
                broke = true;
                break;
            }

        }
        if(!broke) path.push_back(end[0][0]);

    }
    
    for( int i = image.rows - 2; i >=0; i--){

        MIN = 1000000;
        int column = path.back();
        int current = column;
        if(column +1 < image.cols){
            if (M.at<float>(i,column+1) < MIN){
                current = column+1;
                MIN = M.at<float>(i,column+1);
            }
        }
        if(column -1 > 0){
            if (M.at<float>(i,column-1) < MIN){
                current = column-1;
                MIN = M.at<float>(i,column-1);
            }
        }
        if (M.at<float>(i,column) < MIN){
                current = column;
                MIN = M.at<float>(i,column);
            }
        path.push_back(current);
    }

    /*for(auto j = path.begin(); j != path.end(); j++){
        std::cout << *j << std::endl;
    }*/
    end.clear();m.release();last.clear();
    gradient.release();image.release();y.release();x.release(); gradient.release();M.release();
    return;
}


void compress_horizontal(Mat image_old,Mat& image, int target_width){
    image.release();
    image = Mat(image_old);

    Mat image_temp;
    std::vector<int> path;
    
    while(image.cols > target_width){
        image.convertTo(image,CV_8UC3);
        std::cout << path.size() << std::endl << std::endl;
        verticle_seam(image, path);    
        std::reverse(path.begin(),path.end());
        image_temp = Mat::zeros(image.rows-1,image.cols-1, image.type());
        for(int c = 0; c < 3; c++){
            //std::cout << c << std::endl;
            for(int i = 0; i< image.rows; i++){
                for (int j = 0; j < image.cols; j++){
                    if(j < path[i]){
                        image_temp.at<Vec3b>(i,j)[c] = image.at<Vec3b>(i,j)[c];
                    } else if(j > path[i]){
                        image_temp.at<Vec3b>(i,j)[c] = image.at<Vec3b>(i+1,j)[c];
                    }
                    
                }
            }    
        }
        std::cout << image_temp.size() << std::endl << std::endl;
        image.release();
        image = Mat(image_temp);
        image_temp.release();
        path.empty();
        return;

        
               
    }




    return;
}
