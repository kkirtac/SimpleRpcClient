/*
 * ProducerBase.h
 *
 *  Created on: Nov 11, 2016
 *      Author: kadir
 */

#ifndef ProducerBase_H_
#define ProducerBase_H_

#include <string>
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include "SimpleRpcClient.h"

#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class ProducerBase {
public:
	ProducerBase();
	virtual ~ProducerBase();

	virtual void send(AmqpClient::SimpleRpcClient::ptr_t, std::vector<cv::Mat> threadBuffer)=0;

	virtual void initialize(int numThreads)=0;

	void startConnection(std::string host = "127.0.0.1", int port = 5672,
			std::string username = "guest", std::string password = "guest",
			std::string rpc_queue = "rpc_queue", int numThreads = 8);

protected:
	std::string host;
	int port;
	std::string username;
	std::string password;
	std::string rpc_queue;
	int numThreads;

};

#endif /* PRODUCERBASE_H_ */
