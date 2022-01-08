appname=voiconv
CC=c++
src = $(wildcard engine_src/*.cpp) \
	  $(wildcard engine_src/lzma/*.cpp) \
	  main.cpp
	  
obj = $(src:.cpp=.o)

LDFLAGS = -std=c++11 -s -O3
CFLAGS = -O3 -Iengine_src/ -Iengine_src/lzma/ -D_7ZIP_ST

default: release;
debug: CFLAGS += -DDEBUG
debug: executable;
release: executable;

%.o: %.cpp
	$(CC) $(CFLAGS) $(BFLAGS) -c $< -o $@

executable: $(appname);
$(appname): $(obj)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(BFLAGS)

clean:
	rm -f $(obj) $(appname)


