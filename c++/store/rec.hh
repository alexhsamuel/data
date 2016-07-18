#include <cstdint>
#include <iostream>

using Timestamp     = uint64_t;
using Sid           = uint32_t;
using Price         = float;
using Size          = int32_t;
using OrderType     = uint32_t;

struct Trade
{
  Timestamp     timestamp;
  Sid           instrument;
  Size          size;
  Price         price;
};

struct Order
{
  Timestamp     timestamp;      //  0
  Sid           instrument;     //  8
  Size          size;           // 12
  Price         price;          // 16
  OrderType     type;           // 20
};

inline std::ostream&
operator<<(
  std::ostream& os,
  Order const& order)
{
  os << '[' << order.timestamp << "] " << order.instrument
     << ": " << order.size << '@' << order.price << " (" << order.type << ")";
  return os;
}


