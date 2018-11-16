CXXFLAGS:=-std=c++11
INCLUDES:=-I.
CLIENTOBJS:=client.o minioclient.o httpclient.o aws_signer.o stringutils.o serversideencryption.o httpresponse.o httpurl.o

ifeq ($(DEBUG), 1)
    CXXFLAGS+=-g
endif

all: client

client: $(CLIENTOBJS) 
	$(CXX) -o $@ $^

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

.PHONY: clean

clean:
	@rm -f client $(CLIENTOBJS)
