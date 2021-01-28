include_path    = /mammoth/Graduate/include
unit_test_path  = /mammoth/Graduate/unit_test
build_path		= /mammoth/Graduate/build
link_library	= -lgtest -lgtest_main -lpthread
#Library	= -L/usr/local/lib/libgmock.a -L/usr/local/lib/libgmock_main.a -L/usr/local/lib/libgtest.a -L/usr/local/lib/libgtest_main.a
CXXFLAGS 	= -std=c++17 -I$(include_path) -I/usr/local/include -L/usr/local/lib -g

.PHONY : all
all	: Transcript

.PHONY : Transcript
Transcript : $(unit_test_path)/Transcript_test.cpp
	g++ $(CXXFLAGS) -o $@_test $< $(link_library)

.PHONY : clean
clean : 
	rm -f $(build_path)/Transcript_test
