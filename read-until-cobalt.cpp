#include <boost/cobalt.hpp>
#include <boost/cobalt/main.hpp>

#include <boost/asio.hpp>
#include <string_view>
#include <algorithm>
#include <iostream>

namespace cobalt = boost::cobalt;
namespace asio = boost::asio;

using boost::asio::dynamic_buffer;
using boost::asio::ip::tcp;
using boost::asio::detached;
using boost::asio::detached;
using boost::asio::async_read_until;
using boost::system::error_code;
using cobalt::use_op;
namespace this_coro = boost::cobalt::this_coro;
using namespace boost::asio::ip;

int constexpr repeat = 10'000'000;
//int constexpr repeat = 10;

template<class Socket, class Buffer>
struct read_until : cobalt::op<error_code, std::size_t> {
   Socket& socket;
   Buffer buffer;
   std::string_view delim = "\r\n";

   read_until(Socket & socket, Buffer buffer)
   : socket{socket}
   , buffer{buffer} {}

   void ready(cobalt::handler<error_code, std::size_t> h) override
   {
      auto buf = buffer.data();
      const auto begin = asio::buffers_begin(buf);
      const auto end = asio::buffers_end(buf);
      auto itr = std::search(begin, end, std::cbegin(delim), std::cend(delim));
      if (itr != end)
         h({}, std::distance(begin, itr) + 2);
   }

   void initiate(cobalt::completion_handler<error_code, std::size_t> complete) override
   {
      async_read_until(socket, buffer, delim, std::move(complete));
   }
};

template <class Socket, class Buffer>
auto make_read_until(Socket& socket, Buffer buffer)
{
   return read_until<Socket, Buffer>(socket, buffer);
}

cobalt::main co_main(int argc, char ** argv)
{
   try {
      std::string which;
      if (argc != 1)
         which = argv[1];

      tcp::socket s{co_await cobalt::this_coro::executor};
      s.async_connect(tcp::endpoint{address::from_string("127.0.0.1"), 12345}, cobalt::use_op);
      std::string buffer{"+PONG\r\n"};

      if (which == "1") {

         for (int i = 0; i != repeat; ++i) {
            auto const n = co_await async_read_until(s, dynamic_buffer(buffer), "\r\n", cobalt::use_op);
            //std::cout << buffer.substr(1, n - 2) << std::endl;
         }

      } else if (which == "2") {

         for (int i = 0; i != repeat; ++i) {
            auto const n = co_await make_read_until(s, dynamic_buffer(buffer));
            //std::cout << buffer.substr(1,  n - 2) << std::endl;
         }

      } else {
         throw std::runtime_error("No such option.");
      } 

      co_return 0u;
   } catch (...) {
      std::cout << "Error" << std::endl;
   }
}
