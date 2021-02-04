include_path    = /mammoth/Graduate/include
unit_test_path  = /mammoth/Graduate/unit_test
build_path		= /mammoth/Graduate/build
link_library	= -lgtest -lgtest_main -lpthread
#Library	= -L/usr/local/lib/libgmock.a -L/usr/local/lib/libgmock_main.a -L/usr/local/lib/libgtest.a -L/usr/local/lib/libgtest_main.a
CXXFLAGS 	= -std=c++17 -I$(include_path) -I/usr/local/include -L/usr/local/lib

.PHONY : all
all	: Transcript SAMHeader SAM ReadPair SAMToReadPairs

.PHONY : debug
debug : CXXFLAGS += -DDEBUG -g
debug : SAMToReadPairs

.PHONY : Transcript
Transcript : $(unit_test_path)/Transcript_test.cpp
	$(CXX) $(CXXFLAGS) -o $@_test $< $(link_library)

.PHONY : SAMHeader
SAMHeader : $(unit_test_path)/SAMHeader_test.cpp
	$(CXX) $(CXXFLAGS) -o $@_test $< $(link_library)

.PHONY : SAM
SAM : $(unit_test_path)/SAM_test.cpp
	$(CXX) $(CXXFLAGS) -o $@_test $< $(link_library)

.PHONY : ReadPair
ReadPair : $(unit_test_path)/ReadPair_test.cpp
	$(CXX) $(CXXFLAGS) -o $@_test $< $(link_library)

.PHONY : SAMToReadPairs
SAMToReadPairs : $(unit_test_path)/SAMToReadPairs_test.cpp
	$(CXX) $(CXXFLAGS) -o $@_test $< $(link_library)

.PHONY : FragmentLengthDistribution
FragmentLengthDistribution : $(unit_test_path)/FragmentLengthDistribution_test.cpp
	$(CXX) $(CXXFLAGS) -o $@_test $< $(link_library)

.PHONY : clean
clean :
	rm -f $(build_path)/*_test
