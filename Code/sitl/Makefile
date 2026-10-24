CXX ?= g++
SITL_BUILD_DIR ?= build
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra -Werror -fPIC -ffp-contract=off
CORE_INC := ../Core/Inc
CORE_SRC := ../Core/Src/Subsystems/NavigationEstimator.cpp
LIB := $(SITL_BUILD_DIR)/libnavsitl.so

.PHONY: all clean test
all: $(LIB)

$(LIB): native/nav_sitl.cpp native/nav_sitl.h $(CORE_SRC) $(CORE_INC)/Subsystems/NavigationEstimator.h $(CORE_INC)/matrix.hpp $(CORE_INC)/defines.h
	mkdir -p $(SITL_BUILD_DIR)
	$(CXX) $(CXXFLAGS) -shared -I$(CORE_INC) -Inative native/nav_sitl.cpp $(CORE_SRC) -o $@

test: all
	PYTHONPATH=python NAV_SITL_LIB=$(abspath $(LIB)) python3 -m unittest discover -s tests -v

clean:
	rm -rf $(SITL_BUILD_DIR)
