CXX             =       g++

CXXFLAGS        =       --std=c++11 -Wl,-rpath=/usr/local/gcc491/lib64 -pthread
CPPFILES        =       client.cpp
OUTPUT          =       client

all     :
                @$(CXX) $(CXXFLAGS) $(CPPFILES) -o $(OUTPUT)

clean   :
                @rm $(OUTPUT) *.o 2>/dev/null || true
