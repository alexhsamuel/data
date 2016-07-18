# Compiler and linker
CXX            += -std=c++14
CPPFLAGS        = -I$(CXX_INCDIR)
CXXFLAGS    	= -g -Wall -Werror -fdiagnostics-color=always -O3
LDFLAGS	    	= 
LDLIBS          = 

all:

#-------------------------------------------------------------------------------

# How to compile a C++ file, and generate automatic dependencies.
%.o:	    	    	%.cc
%.o:	    	    	%.cc %.cc.d
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CXX_DEPFLAGS) $< -c -o $@

# How to generate assember for C++ files.
%.s:	    	    	%.cc force
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -S -o $@

# How to link an executable. 
%:  	    	    	%.o
%:  	    	    	%.o 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

#-------------------------------------------------------------------------------

.PHONY: all
all:			rec

rec:			rec.o

# Use this target as a dependency to force another target to be rebuilt.
.PHONY: force
force: ;

#-------------------------------------------------------------------------------

# Include autodependency makefles.
%.d: ;
.PRECIOUS: $(DEPS)
-include $(DEPS) 

