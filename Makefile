CXXFLAGS:=-std=c++11
LINKFLAGS:=-L/opt/local/lib -lssl -lcrypto 
HTTPDIR:=simple-http
INCLUDES:=-I. -I/opt/local/include -I$(HTTPDIR)
CLIENTOBJS:=client.o minioclient.o httpclient.o aws_signer.o stringutils.o serversideencryption.o httprequest.o httpresponse.o httpurl.o datetime.o httpheaders.o http1decoder.o httpcommon.o tcpconnection.o

ifeq ($(DEBUG), 1)
    CXXFLAGS+=-g
endif

all: client

client: $(CLIENTOBJS) 
	$(CXX) -o $@ $^ $(LINKFLAGS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

%.o : $(HTTPDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDES)

.PHONY: clean

clean:
	@rm -f client $(CLIENTOBJS)
