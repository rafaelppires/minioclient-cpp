CXXFLAGS:=-std=c++11
INCLUDES:=-I. -I/opt/local/include
LINKFLAGS:=-L/opt/local/lib -lssl -lcrypto 
CLIENTOBJS:=client.o minioclient.o httpclient.o aws_signer.o stringutils.o serversideencryption.o httprequest.o httpresponse.o httpurl.o datetime.o httpheaders.o http1decoder.o httpcommon.o tcpconnection.o

ifeq ($(DEBUG), 1)
    CXXFLAGS+=-g
endif

all: client

client: $(CLIENTOBJS) 
	$(CXX) -o $@ $^ $(LINKFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

.PHONY: clean

clean:
	@rm -f client $(CLIENTOBJS)
