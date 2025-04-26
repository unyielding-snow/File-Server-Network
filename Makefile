# UNAME := $(shell uname -s)

# export CPLUS_INCLUDE_PATH=/opt/homebrew/Cellar/boost/1.87.0_1/include:
# export LIBRARY_PATH=/opt/homebrew/Cellar/boost/1.87.0_1/lib:
# export LD_LIBRARY_PATH=/opt/homebrew/Cellar/boost/1.87.0_1/lib:

# ifeq ($(UNAME),Darwin)
#     CC=clang++
#     CC+=-D_XOPEN_SOURCE
#     CC+=-I/opt/homebrew/Cellar/boost/1.87.0_1/include
#     LIBFSCLIENT=libfs_client_macos.o
#     LIBFSSERVER=libfs_server_macos.o
#     BOOST_THREAD=boost_thread
# BOOST_LIBDIR=-L/opt/homebrew/Cellar/boost/1.87.0_1/lib
# else
#     CC=g++
#     LIBFSCLIENT=libfs_client.o
#     LIBFSSERVER=libfs_server.o
#     BOOST_THREAD=boost_thread
# endif

# # /opt/homebrew/Cellar/boost/1.87.0_1 for Ashton

# CC+=-g -Wall -std=c++17 -Wno-deprecated-declarations

# # List of source files for your file server
# FS_SOURCES=directoryManager.cpp socketManager.cpp requestProcessing.cpp main.cpp errorHandling.cpp

# # Generate the names of the file server's object files
# FS_OBJS=${FS_SOURCES:.cpp=.o}


# TESTS := test1 test2 test3 test4 test5 test6 test7 test8 test9 \
#          test10 test11 test12 test13 test14 test15 test17 test18 test19 test20

# all: fs $(TESTS)

# # Compile the file server and tag this compilation
# #
# # Remember to set the CPLUS_INCLUDE_PATH, LIBRARY_PATH, and LD_LIBRARY_PATH
# # environment variables to include your Boost installation directory.
# fs: ${FS_OBJS} ${LIBFSSERVER}
# 	./autotag.sh push ${FS_SOURCES}
# 	${CC} -o $@ $^ ${BOOST_LIBDIR} -l${BOOST_THREAD} -lboost_system -pthread -ldl

# # Compile a client program
# test1: test1-spec.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test2: test2-fsReadError.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test3: test3-fsWriteError.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test4: test4-fsCreateError.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test5: test5-fsDeleteError.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test6: test6-smallerDir.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test7: test7-growDir.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test8: test8-stresstest1.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test9: test9-concurrency.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test10: test10-writeDeleteSharedOwner.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test11: test11-lamps.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test12: test12-usernameError.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test13: test13-disk-full-error.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test14: test14-showfs.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test15: test15-overflow.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test17: test17.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test18: test18-leading0s.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test19: test19-simple.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# test20: test20-simple.cpp ${LIBFSCLIENT}
# 	${CC} -o $@ $^

# # Generic rules for compiling a source file to an object file
# %.o: %.cpp
# 	${CC} -c $<
# %.o: %.cc
# 	${CC} -c $<

# clean:
# 	rm -f ${FS_OBJS} fs $(TESTS)

# - CAEN BELOW ----------------------------------------------------

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
    CC=clang++
    CC+=-D_XOPEN_SOURCE
    LIBFSCLIENT=libfs_client_macos.o
    LIBFSSERVER=libfs_server_macos.o
    BOOST_THREAD=boost_thread-mt
else
    CC=g++
    LIBFSCLIENT=libfs_client.o
    LIBFSSERVER=libfs_server.o
    BOOST_THREAD=boost_thread
endif

# CC+=-g -Wall -std=c++17 -Wno-deprecated-declarations -DDEBUG
CC+=-g -Wall -std=c++17 -Wno-deprecated-declarations

# List of source files for your file server
FS_SOURCES=directoryManager.cpp socketManager.cpp requestProcessing.cpp main.cpp errorHandling.cpp

# Generate the names of the file server's object files
FS_OBJS=${FS_SOURCES:.cpp=.o}

TESTS := test2 test3 test4 test5 test6 test7 test8 test9 \
         test10 test11 test12 test13 test14 test15 test16 test17 test18 test19 test100

all: fs $(TESTS)

# all: fs test1 test2 test3 test4 test5 test6 test7 test8 test9 test10 test11 test13 test14 test15 test18 test19 test20

# Compile the file server and tag this compilation
#
# Remember to set the CPLUS_INCLUDE_PATH, LIBRARY_PATH, and LD_LIBRARY_PATH
# environment variables to include your Boost installation directory.
fs: ${FS_OBJS} ${LIBFSSERVER}
	./autotag.sh push ${FS_SOURCES}
	${CC} -o $@ $^ -l${BOOST_THREAD} -lboost_system -pthread -ldl

# Compile a client program
test1: test1-spec.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test2: test2-fsReadError.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test3: test3-fsWriteError.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test4: test4-fsCreateError.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test5: test5-fsDeleteError.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test6: test6-smallerDir.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test7: test7-growDir.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test8: test8-stresstest1.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test9: test9-concurrency.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test10: test10-writeDeleteSharedOwner.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test11: test11-lamps.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test12: test12-usernameError.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test13: test13-disk-full-error.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test14: test14-showfs.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test15: test15-overflow.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test16: test16-createblock.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test17: test17.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test18: test18-leading0s.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test19: test19-simple.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test20: test20-simple.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

test99: test99-ha.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^ -pthread
	
test100: test100-massive.cpp ${LIBFSCLIENT}
	${CC} -o $@ $^

# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${FS_OBJS} fs $(TESTS)


testOutputs: $(TESTS)
	@echo "Running tests..."
	@for test in $(TESTS); do \
		if [ -x "$$test" ]; then \
			if diff -q "$$test.txt" "correctOutputs/$$test.txt" >/dev/null; then \
				echo "$$test: PASS"; \
			else \
				echo "$$test: FAIL"; \
				diff -u "correctOutputs/$$test.txt" "$$test.txt"; \
			fi; \
		else \
			echo "$$test: NOT FOUND"; \
		fi; \
		echo; \
	done

