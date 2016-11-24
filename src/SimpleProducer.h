/*
 * SimpleProducer.h
 *
 *  Created on: Nov 14, 2016
 *      Author: kadir
 */

#ifndef SIMPLEPRODUCER_H_
#define SIMPLEPRODUCER_H_

#include "ProducerBase.h"
#include <thread>

class SimpleProducer: public ProducerBase {
public:
	SimpleProducer();
	SimpleProducer(cv::String, int);
	virtual ~SimpleProducer();

	virtual void send(AmqpClient::SimpleRpcClient::ptr_t, std::vector<cv::Mat> threadBuffer) override;
	virtual void initialize(int numThreads) override;

	void loadImages();

private:
	std::vector<std::thread> processorThreads;
	std::vector<cv::Mat> images;
	cv::String folderPath;
	int msgCount;
};


#endif /* SIMPLEPRODUCER_H_ */
