PARSER = parser
DUG = debug
HTTP_SERVER = http_server
.PHONY:all
all: $(PARSER) $(DUG) $(HTTP_SERVER)

cc=g++
$(PARSER):parser.cc
	$(cc) -o $@ $^ -lboost_system -lboost_filesystem -std=c++11
$(DUG):debug.cc
	$(cc) -o $@ $^ -ljsoncpp -std=c++11
$(HTTP_SERVER):http_server.cc
	$(cc) -o $@ $^ -ljsoncpp -lpthread -std=c++11
.PHONY:clean
clean:
	rm -rf $(PARSER) $(DUG) $(HTTP_SERVER)
