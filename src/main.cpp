//============================================================================
// Name        : RpcClient.cpp
// Author      : kadir.kirtac
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "SimpleProducer.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include <opencv2/core/core.hpp>

using namespace std;

//DEFINE_string(iniFileName, "config.ini",
//              "ini file which contains various parameters needed for messaging");
DEFINE_string(host, "127.0.0.1",
		"host");
DEFINE_int32(port, 5672,
		"port");
DEFINE_string(username, "guest",
		"user name");
DEFINE_string(password, "guest",
		"password");
DEFINE_string(rpc_queue_name, "rpc_queue",
		"rpc queue name");
DEFINE_int32(numThreads, 1,
		"number of threads");
DEFINE_string(folder_path, "/home/kadir/Documents/images",
		"root folder path of the source images");
DEFINE_int32(msgCount, 5,
		"number of messages to send");


int main(int argc, char* argv[]) {

	::google::InitGoogleLogging(argv[0]);

	gflags::ParseCommandLineFlags(&argc, &argv, true);


	if( true )
	{

		SimpleProducer producer(cv::String(FLAGS_folder_path), FLAGS_msgCount);

		producer.startConnection(FLAGS_host, FLAGS_port, FLAGS_username, FLAGS_password,
				FLAGS_rpc_queue_name, FLAGS_numThreads);
	}


	return 0;
}

