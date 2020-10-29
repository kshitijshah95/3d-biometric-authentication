#include "stdafx.h"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <mynteyed/camera.h>
#include <mynteyed/utils.h>
#include "dist/json/json.h"
#include "dist/jsoncpp.cpp"
#include <fstream>
#include <string>

MYNTEYE_USE_NAMESPACE

std::string getPath(){
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
	std::string path = root["path"].asString();
	return path;
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
	
	//Get Path from JSON
	std::string fullPath = getPath();
	
	// Cature Image using the Path
	auto left_color = cam.GetStreamData(ImageType::IMAGE_LEFT_COLOR);
	if (left_color.img) {
		cv::Mat left = left_color.img->To(ImageFormat::COLOR_BGR)->ToMat();
		imwrite(fullPath, left);
	}

	cam.Close();
	return 0;
}


