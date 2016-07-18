#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include "rec.hh"

unsigned int constexpr GiB = 1024 * 1024 * 1024;

int
main(
  int const argc,
  char const* const* const argv)
{
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " FILENAME\n";
    return 2;
  }
  char const* const filename = argv[1];

  auto fd = open(filename, O_RDONLY);
  assert(fd != -1);

  struct stat file_info;
  int rval = fstat(fd, &file_info);
  assert(rval == 0);

  struct timeval start_time;
  gettimeofday(&start_time, nullptr);

  void const* const data 
    = mmap(nullptr, file_info.st_size, PROT_READ, MAP_FILE | MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    std::cerr << "mmap failed: " << strerror(errno) << "\n";
    return 1;
  }

  Order const* const orders = reinterpret_cast<Order const*>(data);
  size_t const num_orders = file_info.st_size / sizeof(Order);
  std::cerr << "num orders: " << num_orders << "\n";

  struct Stats
  {
    uint32_t count;
    Size net_size;
    Size volume;
    float vwp;
    Price last_price;
  };
  std::map<Sid, Stats> stats;

  for (size_t i = 0; i < num_orders; ++i) {
    Order const& order = orders[i];
    // std::cout << order << "\n";
    auto iter = stats.find(order.instrument);
    if (iter == stats.end())
      stats[order.instrument] = 
        {1, order.size, std::abs(order.size), 
         std::abs(order.size) * order.price, order.price};
    else {
      Stats& s = iter->second;
      ++s.count;
      s.net_size += order.size;
      s.volume += std::abs(order.size);
      s.vwp += std::abs(order.size) * order.price;
      s.last_price = order.price;
    }
  }

  struct timeval end_time;
  gettimeofday(&end_time, nullptr);

  if (false) 
    for (auto i = stats.begin(); i != stats.end(); ++i) 
      std::cout << i->first << ": " << i->second.count 
                << " volume=" << i->second.volume
                << " last=" << i->second.last_price
                << " vwap=" << i->second.vwp / i->second.volume << "\n";

  auto const elapsed 
    = end_time.tv_sec + end_time.tv_usec * 1E-6
      - start_time.tv_sec - start_time.tv_usec * 1E-6;
  std::cerr << "elapsed: " << elapsed << " = " 
            << elapsed / num_orders / 1E-6 << " µs/rec  "
            << (double) file_info.st_size / GiB * 8 / elapsed << " Gib/s"
            << "\n";

  return 0;
}

