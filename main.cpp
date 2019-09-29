#include "seam_carver.h"

int main( int argc, char** argv )
{
    char* imageName = argv[1];
    Mat image = imread( imageName, IMREAD_COLOR );
    if( argc != 2 || !image.data )
    {
        printf( " No image data \n " );
        return -1;
    }
    Mat img(300, 400, CV_8UC3, Scalar(0,0, 100));
    resize(image,img,img.size());
    image.release();
    compress_horizontal(img, image, 399);
    std::cout << image.size() << std::endl << std::endl;
    namedWindow( "compressed", WINDOW_AUTOSIZE );   
    imshow( "compressed", image );
    namedWindow( "original", WINDOW_AUTOSIZE );   
    imshow( "original", img );


    waitKey(0);
    //std::vector<int> v = verticle_seam(img);

    
    //std::vector<int> v = verticle_seam(img);
    /*
    Mat gray_image;
    Mat img(600, 800, CV_8UC3, Scalar(0,0, 100));
    std::cout << img.size().width << "\n";
    resize(image,img,img.size());
    cvtColor( img, gray_image, COLOR_BGR2GRAY );
    imwrite( "Gray_Image.jpg", gray_image );
    namedWindow( imageName, WINDOW_AUTOSIZE );
    namedWindow( "Gray image", WINDOW_AUTOSIZE );
    imshow( imageName, img );
    imshow( "Gray image", gray_image );
    waitKey(0);

    namedWindow( imageName, WINDOW_AUTOSIZE );
    
    imshow( imageName, image );
    waitKey(0);
    */
    
    //std::cout << "Size : " << v.size() << std::endl; 
    //for (auto it = v.begin(); it != v.end(); it++) 
    //    std::cout << *it << " " << std::endl;

    return 0;
}