#include"TaskServer.hpp"
#define BOOST_ARCHIVE_TEXT_IARCHIVE_HPP

string make_daytime_string()
{
	time_t now = time(0);
	return ctime(&now);
}

void TaskServer::start()
{


	// oarch << g_record;
	try
	{
		// 기본적으로Boost Asio프로그램은하나의IO Service 객체를가집니다.
		boost::asio::io_service io_service;
		// TCP 프로토콜의13번포트로연결을받는수동소켓을생성합니다.
		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 13));
		// 모든클라이언트에대해무한정반복수행합니다.
		while (1)
		{
			// 소켓객체를생성해연결을기다립니다.
			tcp::socket socket(io_service);
			acceptor.accept(socket);
			// 연결이완료되면해당클라이언트에게보낼메시지를생성합니다.
			string message = "asd";
			// 해당클라이언트에게메시지를담아전송합니다.
			boost::system::error_code ignored_error;
			boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
		}
	}
	catch (exception & e) {
		cerr << e.what() << '\n';
	}
}