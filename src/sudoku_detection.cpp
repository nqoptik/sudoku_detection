/**
 * @file sudoku_detection.cpp
 * @author Nguyen Quang <nguyenquang.emailbox@gmail.com>
 * @brief The Sudoku board detection implementation
 * @since 0.0.1
 * 
 * @copyright Copyright (c) 2015, Nguyen Quang, all rights reserved.
 * 
 */

#include <iostream>
#include <sstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * @brief Select the top contours by their area.
 * 
 * @param[in] input_contours The input contours.
 * @param[in] number_of_desired_contours The number of desired contours.
 * @return The top desired contours.
 * @since 0.0.1
 */
std::vector<std::vector<cv::Point>> select_top_contours(std::vector<std::vector<cv::Point>> input_contours,
                                                        int number_of_desired_contours);

/**
 * @brief The main function.
 * 
 * @param[in] argc The argument count.
 * @param[in] argv The argument vector.
 * @return The status value.
 * @since 0.0.1
 */
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("To run the Sudoku board detection, type ./sudoku_detection <image_file>\n");
        return 1;
    }
    cv::Mat image = cv::imread(argv[1], 1);
    if (image.empty())
    {
        printf("The input image is empty.\n");
        return 1;
    }

    cv::Mat gray_image;
    cv::cvtColor(image, gray_image, cv::COLOR_BGR2GRAY);

    cv::Mat dilate_image;
    cv::dilate(gray_image, dilate_image, cv::Mat(), cv::Point(-1, -1), 1, 1, 1);

    cv::Mat edge_dilate_image;
    cv::Canny(dilate_image, edge_dilate_image, 50, 100, 3);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(edge_dilate_image, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    contours = select_top_contours(contours, 0);

    cv::Mat contours_image = cv::Mat::zeros(gray_image.size(), CV_8UC1);
    for (size_t i = 0; i < contours.size(); i++)
    {
        cv::drawContours(contours_image, contours, i, 255, cv::FILLED);
    }

    cv::Mat main_image;
    cv::bitwise_and(contours_image, gray_image, main_image);

    cv::Mat edge_main_image;
    cv::Canny(main_image, edge_main_image, 100, 200, 3);
    cv::dilate(edge_main_image, edge_main_image, cv::Mat(), cv::Point(-1, -1), 1, 1, 1);
    std::vector<std::vector<cv::Point>> contours_2;
    std::vector<cv::Vec4i> hierarchy_2;
    cv::findContours(edge_main_image, contours_2, hierarchy_2, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);
    if (contours_2.size() > 81)
    {
        contours_2 = select_top_contours(contours_2, 81);
    }
    contours_2.erase(contours_2.begin(), contours_2.begin() + 1);

    std::vector<cv::Point> centre_point;
    for (size_t i = 0; i < contours_2.size(); i++)
    {
        cv::Point average_point;
        for (size_t j = 0; j < contours_2[i].size(); j++)
        {
            average_point += contours_2[i][j];
        }
        average_point.x /= contours_2[i].size();
        average_point.y /= contours_2[i].size();
        centre_point.push_back(average_point);
    }

    cv::Mat output_image = image.clone();
    for (size_t i = 0; i < contours_2.size(); i++)
    {
        cv::drawContours(output_image, contours_2, i, 255, 2);
        cv::circle(output_image, centre_point[i], 1, cv::Scalar(0, 0, 255), 4, 8, 0);
    }

    cv::imshow("image", image);
    cv::imshow("output_image", output_image);
    cv::waitKey();

    return 0;
}

std::vector<std::vector<cv::Point>> select_top_contours(std::vector<std::vector<cv::Point>> input_contours,
                                                        int number_of_desired_contours)
{
    if (input_contours.size() == 0)
    {
        return input_contours;
    }
    else
    {
        std::vector<double> input_contours_areas;
        for (size_t i = 0; i < input_contours.size(); i++)
        {
            input_contours_areas.push_back(contourArea(input_contours[i]));
        }

        std::sort(input_contours_areas.begin(), input_contours_areas.end());
        std::reverse(input_contours_areas.begin(), input_contours_areas.end());

        ///Remove noise contours
        std::vector<std::vector<cv::Point>> output_contours;
        for (size_t i = 0; i < input_contours.size(); i++)
        {
            if (cv::contourArea(input_contours[i]) >= input_contours_areas[number_of_desired_contours])
            {
                output_contours.push_back(input_contours[i]);
            }
        }
        return output_contours;
    }
}
