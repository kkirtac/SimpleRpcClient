/*
 * ProducerConsumer.cpp
 *
 *  Created on: Nov 11, 2016
 *      Author: kadir
 */

#include "ProducerBase.h"
#include "glog/logging.h"

ProducerBase::ProducerBase() {
	// TODO Auto-generated constructor stub

}

ProducerBase::~ProducerBase() {
	// TODO Auto-generated destructor stub
}

void ProducerBase::startConnection(std::string _host, int _port,
		std::string _username, std::string _password,
		std::string _rpc_queue, int _numThreads)
{

	host = _host;
	port = _port;
	username = _username;
	password = _password;
	rpc_queue = _rpc_queue;
	numThreads = _numThreads;

	LOG(ERROR) << " [x] Sending RPC requests";

	// start receiving & sending messages
	initialize(numThreads);
}
