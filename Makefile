

all: trickle-fsync

trickle-fsync: trickle-fsync.cc
	g++ -static -O2 $^ -o $@ -lboost_program_options
