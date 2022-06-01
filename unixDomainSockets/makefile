# Copyright 2021 Ana Boccanfuso
cpp = g++

flags = -std=c++17
flags += -Wall
flags += -g

link = $(cpp) $(flags) -o
compile = $(cpp) $(flags) -c

# text-server
#
text-server: server.cpp
	$(link) $@ $^

# text-client
#
text-client: client.cpp
	$(link) $@ $^

# clean up
clean:
		$(RM) text-server text-client
