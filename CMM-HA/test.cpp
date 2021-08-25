#include<iostream>
#include<boost/array.hpp>
#include<boost/asio.hpp>
using boost::asio::ip::tcp;
using namespace std;

int main()
{
	try
	{
		// 기본적으로Boost Asio프로그램은하나의IO Service 객체를가집니다.
		boost::asio::io_service io_service;
		// 도메인이름을TCP 종단점으로바꾸기위해Resolver를사용합니다.
		tcp::resolver resolver(io_service);
		// 서버로는로컬서버, 서비스는Daytime 프로토콜을적어줍니다.
		tcp::resolver::query query("10.0.6.107", "daytime");
		// DNS를거쳐IP 주소및포트번호를얻어옵니다.
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		// 소켓객체를초기화하여서버에연결합니다.
		tcp::socket socket(io_service);
		boost::asio::connect(socket, endpoint_iterator);

		while (1) {
			// 버퍼및오류처리변수를선언합니다.
			boost::array<char, 128> buf;
			boost::system::error_code error;
			// 버퍼를이용해서버로부터데이터를받아옵니다.
			size_t len = socket.read_some(boost::asio::buffer(buf), error);
			if (error == boost::asio::error::eof)
				break;
			else if(error)
				throw boost::system::system_error(error);
			// 버퍼에담긴데이터를화면에출력합니다.
			cout.write(buf.data(), len);
		}
	}
	catch (exception & e) {
		cerr << e.what() << endl;
	}
	system("pause");
	return 0;
}
