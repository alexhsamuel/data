import math
import random
import struct
import time

NS = 1E+9
ORDER_FORMAT = "=QLlfL"
assert struct.calcsize(ORDER_FORMAT) == 24

sids = [ random.randint(1000000, 2000000) for _ in range(5000) ]
prices = { s: math.exp(2 + 4 * random.random()) for s in sids }
timestamp = int(time.time() * NS)

with open("orders.dat", "wb") as file:
    for _ in range(10000000):
        timestamp += int(random.random() * 10 * NS)
        instrument = random.choice(sids)
        order_type = 0  # flags

        size = random.randint(-4, 5)
        if size < 1: size -= 1
        size *= 100

        price = prices[instrument]
        price = round(price * math.exp(random.random() * 0.002), 2)
        prices[instrument] = price

        rec = struct.pack(
            ORDER_FORMAT, timestamp, instrument, size, price, order_type)

        file.write(rec)

