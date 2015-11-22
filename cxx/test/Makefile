CXX		:= $(CXX) -std=c++14
CPPFLAGS	+= -I..
CXXFLAGS    	+= -Wall -O3 -g

.PHONY: all
all:			arith1

%.s:			%.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -S $<

