#include <tcpconnection.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#ifdef ENCLAVED
#include <inet_pton_ntop.h>
#include <my_wrappers.h>
#define htons(n) \
    (((((unsigned short)(n)&0xFF)) << 8) | (((unsigned short)(n)&0xFF00) >> 8))
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#endif


//------------------------------------------------------------------------------
// PLAIN CONNECTION
//------------------------------------------------------------------------------
PlainConnection::~PlainConnection() {
    close();
}

//------------------------------------------------------------------------------
bool PlainConnection::connect(const std::string &host, int port) {
    socket = getsocket(host, port);
    return socket > 0;
}

//------------------------------------------------------------------------------
int PlainConnection::send(const char *buff, size_t len) {
    return ::send(socket, buff, len, 0);
}

//------------------------------------------------------------------------------
int PlainConnection::recv(char *buff, size_t len) {
    return ::recv(socket, buff, len, 0);
}

//------------------------------------------------------------------------------
// TLS CONNECTION
//------------------------------------------------------------------------------
#include <openssl/err.h>
SSL_CTX *TlsConnection::context = 0;
//------------------------------------------------------------------------------
TlsConnection::~TlsConnection() {
    close();
}

//------------------------------------------------------------------------------
void TlsConnection::close() {
    if( ssl_ != nullptr ) {
        SSL_shutdown(ssl_);
        SSL_free(ssl_);
        ssl_ = nullptr;
    }
    EndpointConnection::close();
}

//------------------------------------------------------------------------------
const char *TlsConnection::err_str(int e) {
    switch (e) {
        case SSL_ERROR_NONE:
            return "SSL_ERROR_NONE";
        case SSL_ERROR_SSL:
            return "SSL_ERROR_SSL";
        case SSL_ERROR_WANT_READ:
            return "SSL_ERROR_WANT_READ";
        case SSL_ERROR_WANT_WRITE:
            return "SSL_ERROR_WANT_WRITE";
        case SSL_ERROR_WANT_X509_LOOKUP:
            return "SSL_ERROR_WANT_X509_LOOKUP";
        case SSL_ERROR_SYSCALL:
            return "SSL_ERROR_SYSCALL";
        case SSL_ERROR_ZERO_RETURN:
            return "SSL_ERROR_ZERO_RETURN";
        case SSL_ERROR_WANT_CONNECT:
            return "SSL_ERROR_WANT_CONNECT";
        case SSL_ERROR_WANT_ACCEPT:
            return "SSL_ERROR_WANT_ACCEPT";
        default:
            return "UNKKNOWN";
    };
}

//------------------------------------------------------------------------------
int TlsConnection::password_cb(char *buf, int size, int rwflag, void *password) {
    strncpy(buf, (char *)(password), size);
    buf[size - 1] = '\0';
    return strlen(buf);
}


//------------------------------------------------------------------------------
EVP_PKEY *TlsConnection::generatePrivateKey() {
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    EVP_PKEY_keygen_init(pctx);
    EVP_PKEY_CTX_set_rsa_keygen_bits(pctx, 2048);
    EVP_PKEY_keygen(pctx, &pkey);
    return pkey;
}

//------------------------------------------------------------------------------
X509 *TlsConnection::generateCertificate(EVP_PKEY *pkey) {
    X509 *x509 = X509_new();
    X509_set_version(x509, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 0);
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), (long)60 * 60 * 24 * 365);
    X509_set_pubkey(x509, pkey);

    X509_NAME *name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC,
                               (const unsigned char *)"CH", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                               (const unsigned char *)"A-SKY", -1, -1, 0);
    X509_set_issuer_name(x509, name);
    X509_sign(x509, pkey, EVP_sha256());
    return x509;
}

//------------------------------------------------------------------------------
SSL_CTX *TlsConnection::create_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLSv1_2_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        printf("Unable to create SSL context");
        exit(EXIT_FAILURE);
    }
    return ctx;
}

//------------------------------------------------------------------------------
void TlsConnection::configure_context(SSL_CTX *ctx) {
    EVP_PKEY *pkey = generatePrivateKey();
    X509 *x509 = generateCertificate(pkey);

    SSL_CTX_use_certificate(ctx, x509);
    SSL_CTX_set_default_passwd_cb(ctx, password_cb);
    SSL_CTX_use_PrivateKey(ctx, pkey);

    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
}

//------------------------------------------------------------------------------
void TlsConnection::init_openssl(SSL_CTX **ctx) {
    OpenSSL_add_ssl_algorithms();
    OpenSSL_add_all_ciphers();
    SSL_load_error_strings();

    printf("%s\n", SSLeay_version(SSLEAY_VERSION));
    *ctx = create_context();
    SSL_CTX_set_ecdh_auto(*ctx, 1);
    configure_context(*ctx);
}

//------------------------------------------------------------------------------
bool TlsConnection::connect(const std::string &host, int port) {
    if( context == 0) init_openssl(&context);
    socket = getsocket(host, port);
    if (socket > 0) {
        SSL *cli = SSL_new(context);
        SSL_set_fd(cli, socket);
        ERR_clear_error();
        int r = SSL_connect(cli);
        if (r <= 0) {
            SSL_free(cli);
            r = SSL_get_error(cli, r);
            printf("%s: %s\n", err_str(r),
                   ERR_error_string(ERR_get_error(), nullptr));
            close();
            return false;
        }
        ssl_ = cli;
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
int TlsConnection::send(const char *buff, size_t len) {
    if(ssl_ == nullptr) return -1;
    return SSL_write(ssl_, buff, len);
}

//------------------------------------------------------------------------------
int TlsConnection::recv(char *buff, size_t len) {
    if(ssl_ == nullptr) return -1;
    return SSL_read(ssl_, buff, len);
}

//------------------------------------------------------------------------------
// ENDPOINT CONNECTION
//------------------------------------------------------------------------------
int EndpointConnection::getsocket(const std::string &host, int port) {
    struct sockaddr_in address;
    int sock = 0;

    struct addrinfo *res, *it;
    int errorcode;
    if ((errorcode = getaddrinfo(host.c_str(), std::to_string(port).c_str(),
                                 nullptr, &res))) {
        printf("Address resolution error %d: '%s'\n", errorcode, host.c_str());
        return -1;
    }

    for (it = res; it != nullptr; it = it->ai_next) {
        if ((sock = ::socket(it->ai_family, it->ai_socktype, it->ai_protocol)) <
            0) {
            printf("Error getting socket: %d\n", errno);
            continue;
        }

        if (::connect(sock, it->ai_addr, it->ai_addrlen) < 0) {
            ::close(sock);
            printf("TCP Connection failed: %d\n", errno);
            continue;
        }
        break;
    }

    return it == nullptr ? -2 : sock;
}

//------------------------------------------------------------------------------
bool EndpointConnection::connect(EndpointConnection **endpoint,
                                 const HttpUrl &url) {
    if (url.isHttps())
        *endpoint = new TlsConnection();
    else
        *endpoint = new PlainConnection();

    if (!(*endpoint)->connect(url.host(), url.port())) {
        delete *endpoint;
        *endpoint = nullptr;
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
void EndpointConnection::close() {
    if( socket > 0 )
        ::close(socket);
    socket = -1;
}

