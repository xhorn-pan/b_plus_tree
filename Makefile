#
# In order to execute this "Makefile" just type "make"
#	Xinghua Pan (xi.pan@ufl.edu)
#

SRCS		= b_plus_tree.cc main.cc
OBJS		= $(SRCS:.cc=.o)
HEADERS		= b_plus_tree.h catch2.h
OUT		= bplustree
TEST_SRCS	= b_plus_tree_test.cc  main_test.cc
TEST_OBJS	= $(TEST_SRCS:.cc=.o)
TEST_OUT	= runtest

CC	 	= g++
FLAGS	 	= -std=c++11 -Wall -Wextra -Werror
LFLAGS	 	= #-lpthread
# -g option enables debugging mode 
# -c flag generates object code for separate files


all: $(OBJS)
	$(CC) $(OBJS) -o $(OUT) $(LFLAGS)

# unittest using catch2 https://github.com/catchorg/Catch2
.PHONY: runtest clean

test:
	$(CC) -g -o $(TEST_OUT) $(TEST_SRC) $(LFLAGS)

runtest: test
	./runtest

%.o: %.cc
	$(CC) $(FLAGS) -c $< -o $@

# clean house
clean:
	rm -f $(OBJS) $(TEST_OBJS) $(OUT) $(TEST_OUT)
