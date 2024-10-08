#include <opencv2/opencv.hpp>
#include <map>
#include <string>

#define DECAY_RATE 4
int decay_rate = DECAY_RATE;  // Global variable for trackbar position
int thresh = 128;

#define EDGES 0
#define FILL 1
#define FILL_INV 2

int mode = EDGES;

std::map<int,std::string> modeText = {
    {EDGES, "EDGES"},
    {FILL, "FILL"},
    {FILL_INV, "FILL_INV"}
};


uchar kill(uchar pixel){
    pixel = 0;
    return pixel;
}

uchar birth(uchar pixel, int neighbours_mean){
    pixel = neighbours_mean;
    return pixel;
}

cv::Mat GoL(cv::Mat input, int iters){
    for(int iter=0; iter < iters; iter++){

        for (int i = 0; i < input.rows; i++) {
            for (int j = 0; j < input.cols; j++) {
                uchar &pixel = input.at<uchar>(i, j); 
                bool alive = pixel>(int)0;
                int num_alive_neighbours = 0;
                int neighbours_mean = 0;
                for(int k = -1; k < 2; k++){
                    for(int l = -1; l < 2; l++){
                        if((k==0 && l==0)||i==0||j==0||i==input.rows-1||j==input.cols-1) continue;
                        auto neighbour = input.at<uchar>(i+k,j+l);
                        if(neighbour > (int)0){
                            neighbours_mean += neighbour;
                            num_alive_neighbours+=1;
                        }
                    }
                }
                if(num_alive_neighbours>0){
                    neighbours_mean = neighbours_mean/num_alive_neighbours;
                }
                if(alive){
                    if(num_alive_neighbours < 2 || num_alive_neighbours > 3){
                        pixel = kill(pixel);
                    }
                    else{
                        pixel -= decay_rate;
                    }
                }
                else{
                    if(num_alive_neighbours == 3){
                        pixel = birth(pixel, neighbours_mean);
                    }
                }
            }
        }
    }
    return input;
}

void on_trackbar1(int, void*) {

}
void on_trackbar2(int, void*) {

}

void on_button_click(int, void*) {
    std::cout << "BOTON" << std::endl;
}

int main() {
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Could not open camera" << std::endl;
        return -1;
    }
    bool first = true;

    cv::namedWindow("Webcam");

    // Create a trackbar (slider)
    cv::createTrackbar("Decay rate", "Webcam", &decay_rate, 20, on_trackbar1);
    cv::createTrackbar("Threshold", "Webcam", &thresh, 255, on_trackbar2);

    cv::Mat frame, gray, binary, old_frame, edges, thick_edges, dst;
    cap >> frame;
    double scale_factor = 0.75;  // Scaling down to 25%
    cv::resize(frame, frame, cv::Size(), scale_factor, scale_factor);
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::Laplacian(gray, edges, CV_16S, 3);
    cv::convertScaleAbs(edges, edges);
    cv::threshold(gray, old_frame, thresh, 255, cv::THRESH_BINARY_INV);

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            break;
        }
        

        cv::resize(frame, frame, cv::Size(), scale_factor, scale_factor);
        cv::flip(frame, dst, 1);
        cv::cvtColor(dst, gray, cv::COLOR_BGR2GRAY);
        if(mode == EDGES){
            cv::Canny(gray, edges, 100, 200);
            cv::threshold(edges, binary, 1, 255, cv::THRESH_BINARY);
        }
        else if(mode==FILL){
            cv::threshold(gray, binary, thresh, 255, cv::THRESH_BINARY_INV);
        }
        else if(mode==FILL_INV){
            cv::threshold(gray, binary, thresh, 255, cv::THRESH_BINARY);

        }
        
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
        cv::dilate(binary, thick_edges, kernel);
        cv::putText(thick_edges, //target image
            modeText[mode], //text
            cv::Point(20, thick_edges.rows - 20), //top-left position
            cv::FONT_HERSHEY_DUPLEX,
            3.0,
            255,
            6);
        //cv::bitwise_not(thick_edges, thick_edges); 
        cv::Mat combined;
        
        cv::bitwise_or(thick_edges, old_frame, combined);
        //combined = binary;
         for(int n = 0; n<1;n++){
            //if (cv::waitKey(30) >= 0) break; 
            old_frame = GoL(combined,1);
            //old_frame = edges;
            //cv::Mat upscaled;
            //cv::resize(old_frame, upscaled, frame.size()); 
            if(first){
                first = false;
                break;
            }
            else{
                cv::imshow("Webcam", old_frame);
            }
            
        }

        //cv::imshow("Webcam", old_frame);
        int q = cv::waitKey(10);
        if(q == 27){
            break;
        }
        if((char)q == ' '){
            mode = (mode+1)%3;
        }
        else if(q>0){
            break;
        }
    }

    cap.release(); 
    cv::destroyAllWindows();
    return 0;
}