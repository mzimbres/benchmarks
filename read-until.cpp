#include <iostream>
#include <string>
#include <system_error>
#include <boost/asio.hpp>

namespace asio = boost::asio;
using boost::asio::io_context;
using boost::asio::post;
using boost::asio::dynamic_buffer;
using boost::asio::async_read_until;
using boost::asio::bind_immediate_executor;
using boost::asio::use_awaitable;
using boost::asio::awaitable;
using boost::asio::deferred;
using boost::asio::co_spawn;
using boost::system::error_code;
using namespace boost::asio::ip;

int constexpr repeat = 10'000'000;

void read_safe(tcp::socket& s, std::string& buffer)
{
   static int counter = 0;

   if (counter++ == repeat)
      return;

   auto continuation = [&](auto ec, auto)
   {
      if (ec)
         throw std::system_error(ec);

      read_safe(s, buffer); // Recursive call
   };

   async_read_until(s, dynamic_buffer(buffer), "\r\n", continuation);
}

void read_unsafe(tcp::socket& s, std::string& buffer)
{
   static int counter = 0;

   if (counter++ == repeat)
      return;

   auto cont = [&](std::error_code ec, auto)
   {
      if (ec)
         throw std::system_error(ec);

      read_unsafe(s, buffer); // Recursive call
   };

   if (counter % 16 == 0) {
      post(s.get_executor(), [cont](){cont({}, 0);});
      return; // Break the callstack
   }

   async_read_until(s, dynamic_buffer(buffer), "\r\n", bind_immediate_executor(s.get_executor(), cont));
}

awaitable<void> read_asio_awaitable(tcp::socket& s, std::string& buffer)
{
   for (int i = 0; i != repeat; ++i) {
      error_code ec;
      co_await async_read_until(s, dynamic_buffer(buffer), "\r\n", redirect_error(deferred, ec));
      if (ec)
         throw std::system_error(ec);
   }
}

int main()
{
   try {
      io_context ioc;
      std::string buffer{"+PONG\r\n"};

      tcp::resolver resv{ioc};
      auto const res = resv.resolve("127.0.0.1", "12345");

      tcp::socket s{ioc};
      s.connect(*std::begin(res));
      //read_safe(s, buffer);
      read_unsafe(s, buffer);

      //co_spawn(ioc, read_asio_awaitable(s, buffer), asio::detached);
      //asio::co_spawn(ioc, read_asio_awaitable(s, buffer), [](std::exception_ptr p) {
      //   if (p)
      //      std::rethrow_exception(p);
      //});
      ioc.run();
   } catch (std::exception const& e) {
      std::cout << e.what() << std::endl;
   }
}
