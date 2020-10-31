#include "stdafx.h"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <mynteyed/camera.h>
#include <mynteyed/utils.h>
#include "dist/json/json.h"
#include "dist/jsoncpp.cpp"
#include <fstream>

MYNTEYE_USE_NAMESPACE

std::string getPath() {
	std::ifstream f("image-data.json");
	std::string jsonContent;
	if (f) {
		std::ostringstream ss;
		ss << f.rdbuf();
		jsonContent = ss.str();
	}

	const std::string rawJson = jsonContent;
	const auto rawJsonLength = static_cast<int>(rawJson.length());
	constexpr bool shouldUseOldWay = false;
	JSONCPP_STRING err;
	Json::Value root;

	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(rawJson.c_str(), rawJson.c_str() + rawJsonLength, &root,
		&err)) {
		std::cout << "error" << std::endl;
		return "Failed to parse JSON";
	}
	std::string path = root["filePath"].asString();
	std::string name = root["fileName"].asString();
	std::string fullPath = path + name;
	return fullPath;
}

int main()
{
	Camera cam;
	DeviceInfo dev_info;
	if (!util::select(cam, &dev_info)) {
		return 1;
	}

	OpenParams params(dev_info.index);
	params.depth_mode = DepthMode::DEPTH_COLORFUL;
	params.stream_mode = StreamMode::STREAM_2560x720;
	params.ir_intensity = 4;
	params.framerate = 30;

	cam.Open(params);

	if (!cam.IsOpened()) {
		std::cerr << "Error: Open camera failed" << std::endl;
		return 1;
	}

	cv::namedWindow("left");
	
	std::string fullPath = getPath();
	std::cout << fullPath;
	auto left_color = cam.GetStreamData(ImageType::IMAGE_LEFT_COLOR);
	if (left_color.img) {
		cv::Mat left = left_color.img->To(ImageFormat::COLOR_BGR)->ToMat();
		std::cout << fullPath;
		cv::imwrite(fullPath + ".png", left);
	}
	/*
	for (;;) {
		auto left_color = cam.GetStreamData(ImageType::IMAGE_LEFT_COLOR);
		if (left_color.img) {
			cv::Mat left = left_color.img->To(ImageFormat::COLOR_BGR)->ToMat();
			cv::imshow("left", left);
		}

		/*
		auto image_depth = cam.GetStreamData(ImageType::IMAGE_DEPTH);
		if (image_depth.img) {
			cv::Mat depth = image_depth.img->To(ImageFormat::DEPTH_BGR)->ToMat();
			//cv::imshow("depth", depth);
		}
		


		char key = static_cast<char>(cv::waitKey(1));
		if (key == 27 || key == 'q' || key == 'Q') {  // ESC/Q
			break;
		}
	}
	*/
	cam.Close();
	cv::destroyAllWindows();
	return 0;
}
