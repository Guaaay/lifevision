#include <opencv2/opencv.hpp>

uchar kill(uchar pixel){
    pixel = 0;
    return pixel;
}

uchar birth(uchar pixel){
    pixel = 255;
    return pixel;
}

cv::Mat GoL(cv::Mat input, int iters){
    for(int iter=0; iter < iters; iter++){

        for (int i = 0; i < input.rows; i++) {
            for (int j = 0; j < input.cols; j++) {
                uchar &pixel = input.at<uchar>(i, j);  // For 3-channel (BGR) images
                bool alive = pixel==(int)255;
                int num_alive_neighbours = 0;
                for(int k = -1; k < 2; k++){
                    for(int l = -1; l < 2; l++){
                        if((k==0 && l==0)||i==0||j==0||i==input.rows-1||j==input.cols-1) continue;
                        auto neighbour = input.at<uchar>(i+k,j+l);
                        if(neighbour == (int)255){
                            num_alive_neighbours+=1;
                        }
                    }
                }
                if(alive){
                    if(num_alive_neighbours < 2 || num_alive_neighbours > 3){
                        pixel = kill(pixel);
                    }
                }
                else{
                    if(num_alive_neighbours == 3){
                        pixel = birth(pixel);
                    }
                }
            }
        }
    }
    return input;
}

int main() {
    cv::VideoCapture cap(0); // Open the default camera
    if (!cap.isOpened()) { // Check if we succeeded
        std::cerr << "Could not open camera" << std::endl;
        return -1;
    }
    bool first = true;

    cv::Mat frame, gray, binary, old_frame, edges, thick_edges;
    cap >> frame;
    double scale_factor = 0.8;  // Scaling down to 25%
    cv::resize(frame, frame, cv::Size(), scale_factor, scale_factor);
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    cv::Laplacian(gray, edges, CV_16S, 3);  // Apply Laplacian with kernel size 3
    cv::convertScaleAbs(edges, edges);      // Convert back to 8-bit format
    cv::threshold(gray, old_frame, 156, 255, cv::THRESH_BINARY_INV);

    while (true) {
        cap >> frame;

        if (frame.empty()) {
            break; // End of video stream
        }

        cv::resize(frame, frame, cv::Size(), scale_factor, scale_factor);
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            // Apply the Laplacian edge detection
        cv::Canny(gray, edges, 100, 200);
        //cv::convertScaleAbs(edges, edges);      // Convert back to 8-bit format

        cv::threshold(edges, binary, 100, 255, cv::THRESH_BINARY);
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(10, 10));

        // Apply dilation to make the edges thicker
         cv::dilate(binary, thick_edges, kernel);
        cv::bitwise_not(thick_edges, thick_edges); 
        cv::Mat combined;
        
        cv::add(thick_edges, old_frame, combined);
        //combined = binary;
         for(int n = 0; n<1;n++){
            //if (cv::waitKey(30) >= 0) break; // Wait for a keystroke in the window or 30ms
            old_frame = GoL(combined,1);
            //old_frame = edges;
            //cv::Mat upscaled;
            //cv::resize(old_frame, upscaled, frame.size());  // Resize back to the original size
            if(first){
                first = false;
                break;
            }
            else{
                cv::imshow("Webcam", old_frame);
            }
            
        }

        //cv::imshow("Webcam", old_frame);
        if (cv::waitKey(10) >= 0) break; // Wait for a keystroke in the window or 30ms
    }

    cap.release(); // Release the VideoCapture object
    cv::destroyAllWindows(); // Close all OpenCV windows
    return 0;
}