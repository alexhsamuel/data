# Goals

- read-optimized

- simple pointer arithmetic, amendable to SSE vectorization

- further optimization for contiguous layouts

- separation of physical and logical types; operations on physical types
  wherever possible


# Types

## Physical type

A physical type specifies the memory size required to store a value.  A physical
type can be of three sorts:

- a fixed number of bits, which are to be packed in memory
- a fixed number of bytes; 1, 2, 4, and 8 bytes are optimized
- a variable number of bytes per element


## Logical type

A logical type specifies the interpretation of a value of a given physical
type.  Some logical types include,

- (raw) bytes
- boolean
- unsigned or signed integer
- IEEE 754 floating point number
- fixed point number
- Unicode code point number
- encoded date or time

Derived logical types are possible.  For example,

- Specific bit patterns maybe be reserved to indicate special values, such
  as first-class "null", "invalid", "missing", or others.

- A finite set of values may be encoded as a **category type**.

- A string type 


# Storage

## Buffer

A **buffer** represents a contiguous block of memory, can signify memory
ownership responsibility, and can (if owned) deallocate the memory block.

- `void* start`
- `size_t size`
- ownership
- deallocation 

## Sequence

A **sequence** represents a fixed-length sequence of fixed-size values in
memory.  They are numbered 0 through _length - 1_, and a fixed pointer stride
from one element to the next.

- `void* start`
- `size_t length`
- `ptrdiff_t stride`

## Array

An **array** is a fixed-length sequence of logical values in memory.

- a sequence
- a logical type
- `bool writeable`

For an array to be **contiguous**, its sequence stride must equal the physical
size of its elements.

## Index

An **index** maps some set of values to positions in an array.  Some types of
indexes include,

- A **counting index** maps array positions from consecutive integers.

- A **label index** effectively inverts a category type to determine positions.
  Each value in the array maps to its position in the category array.

- A **multidimensional index** is the Cartesian product of one or more indexes.

