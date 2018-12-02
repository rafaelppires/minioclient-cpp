#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#ifdef ENCLAVED
#include <libc_mock/libc_proxy.h>
#endif
#include <openssl/ssl.h>
#include <string>
#include <httpurl.h>
#ifdef ENCLAVED
#undef connect
#endif

//------------------------------------------------------------------------------
class EndpointConnection {
public:
    virtual ~EndpointConnection() {}
    virtual int send( const char *buff, size_t len ) = 0;
    virtual int recv( char *buff, size_t len ) = 0;
    virtual bool connect( const std::string &host, int port ) = 0;
    virtual void close();

    static bool connect( EndpointConnection **endpoint, const HttpUrl &url );
protected:
    int getsocket(const std::string &host, int port);
    int socket;
};

//------------------------------------------------------------------------------
class TlsConnection : public EndpointConnection {
public:
    virtual ~TlsConnection();
    TlsConnection() : ssl_(nullptr) {}
    bool connect( const std::string &host, int port );
    int send( const char *buff, size_t len );
    int recv( char *buff, size_t len );
    void close();
private:
    SSL *ssl_;
    static const char *sslerr_str(int e);
    static int password_cb(char *buf, int size, int rwflag, void *password);
    static EVP_PKEY *generatePrivateKey();
    static X509 *generateCertificate(EVP_PKEY *pkey);
    static SSL_CTX *create_context();
    static void configure_context(SSL_CTX *ctx);
    static void init_openssl(SSL_CTX **ctx);
    static SSL_CTX *context;
};


//------------------------------------------------------------------------------
class PlainConnection : public EndpointConnection {
public:
    virtual ~PlainConnection();
    bool connect( const std::string &host, int port );
    int send( const char *buff, size_t len );
    int recv( char *buff, size_t len );
};

//------------------------------------------------------------------------------

#endif
