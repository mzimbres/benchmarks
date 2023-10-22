#include <boost/cobalt.hpp>
#include <boost/cobalt/main.hpp>

#include <boost/asio.hpp>
#include <list>

namespace cobalt = boost::cobalt;
namespace asio = boost::asio;

using boost::asio::dynamic_buffer;
using boost::asio::ip::tcp;
using boost::asio::detached;
using cobalt::use_op;
namespace this_coro = boost::cobalt::this_coro;
using namespace boost::asio::ip;

int constexpr repeat = 10'000'000;

cobalt::main co_main(int argc, char ** argv)
{
   tcp::socket s{co_await cobalt::this_coro::executor};
   s.async_connect(tcp::endpoint{address::from_string("127.0.0.1"), 12345}, cobalt::use_op);

   std::string buffer{"+PONG\r\n"};
   for (int i = 0; i != repeat; ++i) {
      co_await async_read_until(s, dynamic_buffer(buffer), "\r\n", cobalt::use_op);
   }

   co_return 0u;
}
