#include <vector>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>

#include "pose.hpp"
#include "poseEstimation.hpp"

#include <algorithm>

namespace fs = std::filesystem;

// check output folder
void create_output_folder(const std::string& folder_path) {
    if (!fs::exists(folder_path)) {
        fs::create_directories(folder_path);
    }
}

std::string get_frame_filename(const std::string& folder_path, int frame_number) {
    std::ostringstream filename;
    filename << folder_path << "/frame_" << std::setw(6) << std::setfill('0') << frame_number << ".jpg";
    return filename.str();
}

std::vector<std::string> get_frame_filenames(const std::string& folder_path) {
    std::vector<std::string> filenames;
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        filenames.push_back(entry.path().string());
    }
    std::sort(filenames.begin(), filenames.end());  // Ensure frames are sorted correctly
    return filenames;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <model_path> <input_path> <type>" << std::endl;
        return -1;
    }

    std::string model_path = argv[1];
    std::string input_path = argv[2];
    std::string type = argv[3];

    std::cout << "Model path: " << model_path << std::endl;
    std::cout << "Input path: " << input_path << std::endl;
    std::cout << "Type: " << type << std::endl;

    // Initialize pose estimation and pose tracker
    poseEstimation::poseEstimation pe(model_path);
    poseEstimation::poseTracker pt;

    if (type == "video") {
        // Open the video
        cv::VideoCapture cap(1);  // access to camera index 1

        if (!cap.isOpened()) {
            std::cerr << "Error: Could not open video." << std::endl;
            return -1;
        }

        // Set frames per second (Please change as you want)
        double desired_fps = 32.0;
        double video_fps = cap.get(cv::CAP_PROP_FPS);
        std::cout << "Video FPS: " << video_fps << std::endl;  

        if (video_fps <= 0) {
            std::cerr << "Error: Could not retrieve video FPS." << std::endl;
            return -1;
        }

        // Calculate frame skip, ensuring it is at least 1
        int frame_skip = static_cast<int>(video_fps / desired_fps);
        if (frame_skip < 1) {
            frame_skip = 1; 
        }
        std::cout << "Frame Skip: " << frame_skip << std::endl; 

        // Create output folder 
        std::string output_folder = "frames";
        create_output_folder(output_folder);

        int frame_number = 0;
        int processed_frame_number = 0;
        cv::Mat frame;

        while (cap.read(frame)) {
          
            std::cout << "Reading frame " << frame_number << std::endl;

            // Skip frames to achieve the desired fps
            if (frame_number % frame_skip != 0) {
                frame_number++;
                continue;
            }

            // Create a white background image to draw pose estimation
            cv::Mat white_background(frame.size(), frame.type(), cv::Scalar(255, 255, 255));

            // Run pose estimation
            std::vector<poseEstimation::Pose> poses = pe.run(frame);
            pt.track(poses);

            // Draw the poses on the white background image
            for (int i = 0; i < poses.size(); i++) {
                poses[i].draw(white_background, true);
            }

 
            std::string output_filename = get_frame_filename(output_folder, processed_frame_number);
            cv::imwrite(output_filename, white_background);

            processed_frame_number++;
            frame_number++;
        }

        cap.release();
        std::cout << "Processing completed. Total frames processed: " << processed_frame_number << std::endl;

   
        std::vector<std::string> frame_filenames = get_frame_filenames(output_folder);
        if (frame_filenames.empty()) {
            std::cerr << "Error: No frames found for video creation." << std::endl;
            return -1;
        }

        cv::Mat first_frame = cv::imread(frame_filenames[0]);
        if (first_frame.empty()) {
            std::cerr << "Error: Could not read first frame." << std::endl;
            return -1;
        }

    
        cv::VideoWriter video_writer("poses_video.avi",
            cv::VideoWriter::fourcc('X', 'V', 'I', 'D'),  // Use a different codec
            desired_fps,
            first_frame.size());

        if (!video_writer.isOpened()) {
            std::cerr << "Error: Could not open video writer." << std::endl;
            return -1;
        }

 
        for (const auto& filename : frame_filenames) {
            cv::Mat frame = cv::imread(filename);
            if (frame.empty()) {
                std::cerr << "Error: Could not read frame from " << filename << std::endl;
                continue;
            }
            video_writer.write(frame);
        }

        video_writer.release();
        std::cout << "Video creation completed." << std::endl;

    }
    else if (type == "image") {
       
        cv::Mat image = cv::imread(input_path);
        if (image.empty()) {
            std::cerr << "Error: Could not read image." << std::endl;
            return -1;
        }
       
        cv::Mat white_background(image.size(), image.type(), cv::Scalar(255, 255, 255));

        std::vector<poseEstimation::Pose> poses = pe.run(image);
        pt.track(poses);

        for (int i = 0; i < poses.size(); i++) {
            poses[i].draw(white_background, true);
        }

        std::string output_filename = "processed_image.jpg";
        cv::imwrite(output_filename, white_background);

        std::cout << "Image processing completed. Processed image saved as " << output_filename << std::endl;

    }
    else if (type == "camera") {
        cv::VideoCapture cap(0);
        if (!cap.isOpened()) {
            std::cerr << "Error: Could not open camera." << std::endl;
            return -1;
        }

        cv::Mat frame;
        while (true) {
            cap >> frame;
            if (frame.empty()) {
                std::cerr << "Error: Could not retrieve frame from camera." << std::endl;
                break;
            }

            cv::Mat white_background(frame.size(), frame.type(), cv::Scalar(255, 255, 255));

            std::vector<poseEstimation::Pose> poses = pe.run(frame);
            pt.track(poses);

            for (int i = 0; i < poses.size(); i++) {
                poses[i].draw(white_background, true);
            }

            cv::imshow("Pose Estimation", white_background);

        
            if (cv::waitKey(1) == 27) {
                break;
            }
        }

        cap.release();
        cv::destroyAllWindows();
        std::cout << "Camera processing completed." << std::endl;

    }
    else {
        std::cerr << "Error: Unknown type. Please specify 'video', 'image', or 'camera'." << std::endl;
        return -1;
    }

    return 0;
}
