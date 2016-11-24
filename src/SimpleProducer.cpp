/*
 * SimpleProducerConsumer.cpp
 *
 *  Created on: Nov 14, 2016
 *      Author: kadir
 */

#include "SimpleProducer.h"
#include "glog/logging.h"
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "base64.h"
#include <iostream>
#include <sstream>
#include <opencv2/imgproc/imgproc.hpp>

using namespace rapidjson;
using namespace std;

SimpleProducer::SimpleProducer() {
	// TODO Auto-generated constructor stub

}

SimpleProducer::SimpleProducer(cv::String _folderPath, int _msgCount) {
	// TODO Auto-generated constructor stub
	folderPath = _folderPath;
	msgCount = _msgCount;
}

SimpleProducer::~SimpleProducer() {
	// TODO Auto-generated destructor stub
	// deallocate images and thread buffers
	for (int i = 0; i < images.size(); ++i) {
		images[i].release();
	}
	images.clear();
	processorThreads.clear();

}

void SimpleProducer::send(AmqpClient::SimpleRpcClient::ptr_t simpleClient, std::vector<cv::Mat> threadBuffer){

	std::string requestMessage;
	std::string responseMessage;

	int resizeWidth = 200;
	int resizeHeight = 200;

	LOG(ERROR) << "thread: threadBuffer.size() " << threadBuffer.size() ;

	int ctr = 0;
	while( threadBuffer.size() > 0 )
	{

		std::vector<cv::Mat>::iterator it = threadBuffer.begin() ;
		cv::Mat image = *it;

		// base64 encoding
		vector<uchar> buf;
		cv::imencode(".jpg", image, buf);
		uchar *enc_msg = new uchar[buf.size()];
		for(int i=0; i < buf.size(); i++)
			enc_msg[i] = buf[i];
		std::string encodedImg = base64_encode(enc_msg, buf.size());

		// build json
		StringBuffer s;
		Writer<StringBuffer> writer(s);
		writer.StartObject();               // Between StartObject()/EndObject(),
		writer.Key("base64Image");
		writer.String(encodedImg.c_str());
		writer.Key("resizeWidth");
		writer.Int(resizeWidth);
		writer.Key("resizeHeight");
		writer.Int(resizeHeight);
		writer.EndObject();

		requestMessage = s.GetString();

		responseMessage = simpleClient->Call(requestMessage);

		LOG(ERROR) << "Producer Thread " << std::this_thread::get_id() << " sent message: ";
		LOG(ERROR) << "Producer Thread " << std::this_thread::get_id() << " received message: ";

		threadBuffer.erase (threadBuffer.begin());

		// parse response message
		// parse json
		Document document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.

	    if (document.Parse(responseMessage.c_str()).HasParseError()){
	    	LOG(ERROR) << "Producer Thread " << std::this_thread::get_id() << threadBuffer.size() ;
	    	continue;
	    }

	    assert(document.IsObject());    // Document is a JSON value represents the root of DOM. Root can be either an object or array.

	    assert(document.HasMember("base64Image"));
	    assert(document["base64Image"].IsString());
	    std::string base64Response = document["base64Image"].GetString();

	    assert(document.HasMember("resizeWidth"));
	    assert(document["resizeWidth"].IsInt());
	    int resizedWidth = document["resizeWidth"].GetInt();

	    assert(document.HasMember("resizeHeight"));
	    assert(document["resizeHeight"].IsInt());
	    int resizedHeight = document["resizeHeight"].GetInt();

		// base64 decoding
	    std::string decodedStr = base64_decode(base64Response);
	    std::vector<uchar> data(decodedStr.begin(), decodedStr.end());
	    cv::Mat decodedImg = cv::imdecode(data, CV_LOAD_IMAGE_UNCHANGED);

	    stringstream ss;
	    ss << std::this_thread::get_id() << "_" << ctr;
	    ctr++;

	    cv::namedWindow(ss.str());
	    cv::imshow(ss.str(), decodedImg);
	    cv::waitKey(0);

	}

	LOG(ERROR) << "Producer Thread " << std::this_thread::get_id() << threadBuffer.size() ;


}

void SimpleProducer::initialize(int numThreads){

	loadImages();

	int numMsgPerThread = msgCount / numThreads;
	int remainder = msgCount % numThreads;
	std::vector<int> values(numThreads, numMsgPerThread);
	for (int i = 0; i < remainder; ++i)
	{
		values[i]++;
		LOG(ERROR) << values[i] ;
	}

	LOG(ERROR) << "images.size()" << images.size();

	int curr_assigned_msg_idx = 0;

	for (int i = 0; i < numThreads; ++i) {

		std::vector<cv::Mat> threadBuffer;
		for (int j=0; j < values[i]; ++j){
			threadBuffer.push_back(images[curr_assigned_msg_idx % images.size()]);
			curr_assigned_msg_idx++;
		}

		LOG(ERROR) << "threadBufferSize: " << threadBuffer.size() ;

		AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create(host, port, username, password);
		AmqpClient::SimpleRpcClient::ptr_t simpleClient = AmqpClient::SimpleRpcClient::Create(channel, rpc_queue);
		processorThreads.emplace_back(&SimpleProducer::send, this, simpleClient, threadBuffer);

	}

	for (int i = 0; i < numThreads; ++i) {
		if(processorThreads[i].joinable())
			processorThreads[i].join();
	}
}

void SimpleProducer::loadImages()
{

    std::vector<cv::String> filenames; // notice here that we are using the Opencv's embedded "String" class

    cv::glob(folderPath, filenames); // new function that does the job ;-)

    for(size_t i = 0; i < filenames.size(); ++i)
    {
        cv::Mat src = cv::imread(filenames[i]);

        if(!src.data)
            LOG(ERROR) << "Problem loading image " << filenames[i];
        else
        	images.push_back(src);

    }

}

