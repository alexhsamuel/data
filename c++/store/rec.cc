#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "rec.hh"

unsigned int constexpr GiB = 1024 * 1024 * 1024;

//------------------------------------------------------------------------------

template<class REC>
class MmapReader
{
public:

  // FIXME: Iterator may not outlive container.
  class Iterator
  {
  public:

    Iterator(
      MmapReader const* const file,
      size_t const pos)
    : file_(file),
      pos_(pos)
    {
      assert(0 <= pos_);
      assert(pos_ <= file->length());
    }

    ~Iterator() = default;

    bool operator==(Iterator const& other) const { return other.pos_ == pos_; }
    bool operator!=(Iterator const& other) const { return ! operator==(other); }
    void operator++() { ++pos_; }

    REC const& operator->() { return file_->get(pos_); }
    REC const& operator*() { return file_->get(pos_); }

  private:

    MmapReader const* const file_;
    size_t pos_;

  };

  MmapReader(
    char const* const filename)
  {
    fd_ = open(filename, O_RDONLY);
    assert(fd_ != -1);

    struct stat file_info;
    int const rval = fstat(fd_, &file_info);
    assert(rval == 0);
    size_ = file_info.st_size;
    length_ = file_info.st_size / sizeof(REC);

    void const* data 
      = mmap(nullptr, size_, PROT_READ, MAP_FILE | MAP_SHARED, fd_, 0);
    assert(data != nullptr);
    data_ = reinterpret_cast<REC const*>(data);
  }

  MmapReader(MmapReader const&) = delete;
  MmapReader(MmapReader&&) = delete;

  ~MmapReader()
  {
    int const rval = munmap((void*) data_, size_);
    assert(rval == 0);
  }

  size_t size() const { return size_; }
  size_t length() const { return length_; }

  REC const& get(
    size_t const pos)
    const
  {
    assert(0 <= pos);
    assert(pos < length_);
    return data_[pos];
  }

  Iterator begin() const { return {this, 0}; }
  Iterator end() const { return {this, length_}; }
  
private:

  int fd_;
  size_t size_;
  size_t length_;
  REC const* data_;

};


//------------------------------------------------------------------------------

template<class REC>
class BufferReader
{
public:

  // FIXME: Iterator may not outlive container.
  class Iterator
  {
  public:

    Iterator(
      BufferReader const* const reader,
      size_t const pos)
    : reader_(reader),
      pos_(pos)
    {
      assert(0 <= pos_);
      assert(pos_ <= reader->length());
    }

    ~Iterator() = default;

    bool operator==(Iterator const& other) const { return other.pos_ == pos_; }
    bool operator!=(Iterator const& other) const { return ! operator==(other); }
    void operator++() { ++pos_; }

    REC const& operator->() { return reader_->get(pos_); }
    REC const& operator*() { return reader_->get(pos_); }

  private:

    BufferReader const* const reader_;
    size_t pos_;

  };

  BufferReader(
    char const* const filename)
  {
    int const fd = open(filename, O_RDONLY);
    assert(fd != -1);

    struct stat file_info;
    auto const rval = fstat(fd, &file_info);
    assert(rval == 0);
    size_ = file_info.st_size;
    length_ = size_ / sizeof(REC);

    data_ = new REC[length_];
    auto const read_size = read(fd, data_, size_);
    assert(read_size == size_);
  }

  BufferReader(BufferReader const&) = delete;
  BufferReader(BufferReader&&) = delete;

  ~BufferReader()
  {
    delete[] data_;
  }

  size_t size() const { return size_; }
  size_t length() const { return length_; }

  REC const& get(
    size_t const pos)
    const
  {
    assert(0 <= pos);
    assert(pos < length_);
    return data_[pos];
  }

  Iterator begin() const { return {this, 0}; }
  Iterator end() const { return {this, length_}; }
  
private:

  size_t size_;
  size_t length_;
  REC* data_;

};


//------------------------------------------------------------------------------

struct OrderStats
{
  uint32_t count;
  Size net_size;
  Size volume;
  float vwp;
  Price last_price;
};


template<class READER>
std::map<Sid, OrderStats>
get_order_stats(
  READER const& reader) 
{
  std::map<Sid, OrderStats> stats;
  for (auto const& order : reader) {
    // std::cout << order << "\n";
    auto iter = stats.find(order.instrument);
    if (iter == stats.end())
      stats[order.instrument] = 
        {1, order.size, std::abs(order.size), 
         std::abs(order.size) * order.price, order.price};
    else {
      OrderStats& s = iter->second;
      ++s.count;
      s.net_size += order.size;
      s.volume += std::abs(order.size);
      s.vwp += std::abs(order.size) * order.price;
      s.last_price = order.price;
    }
  }
  return stats;
}


template<class READER>
uint64_t
get_total_volume(
  READER const& reader)
{
  uint64_t volume = 0;
  for (auto const& order : reader)
    volume += std::abs(order.size);
  return volume;
}


//------------------------------------------------------------------------------

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

  struct timeval start_time;
  gettimeofday(&start_time, nullptr);

  MmapReader<Order> reader(filename);
  // BufferReader<Order> reader(filename);
  // auto const stats = get_total_stats(reader);
  auto const total_volume = get_total_volume(reader);

  struct timeval end_time;
  gettimeofday(&end_time, nullptr);

#if 0
  for (auto i = stats.begin(); i != stats.end(); ++i) 
    std::cout << i->first << ": " << i->second.count 
              << " volume=" << i->second.volume
              << " last=" << i->second.last_price
              << " vwap=" << i->second.vwp / i->second.volume << "\n";
#endif
  std::cout << "total volume = " << total_volume << "\n";

  auto const elapsed 
    = end_time.tv_sec + end_time.tv_usec * 1E-6
      - start_time.tv_sec - start_time.tv_usec * 1E-6;
  std::cerr << "elapsed: " << elapsed << " = " 
            << elapsed / reader.length() / 1E-6 << " µs/rec  "
            << (double) reader.size() / GiB * 8 / elapsed << " Gib/s"
            << "\n";

  return 0;
}

