#ifndef _MINIO_EXCEPTIONS_H_
#define _MINIO_EXCEPTIONS_H_

class ErrorCode {
   public:
    enum Code { NO_ERROR, NO_SUCH_BUCKET };

    ErrorCode() : e_(NO_ERROR) {}
    ErrorCode(Code c) : e_(c) {}

    Code errorCode() const { return e_; }

   private:
    Code e_;
};

class ErrorResponseException : public std::runtime_error {
   public:
    ErrorResponseException(const std::string &msg) : std::runtime_error(msg) {}
    ErrorCode errorResponse() const { return ErrorCode(ErrorCode::NO_ERROR); }
};

class RegionConflictException : public std::invalid_argument {
   public:
    RegionConflictException(const std::string &msg)
        : std::invalid_argument(msg) {}
};

class NoResponseException : public std::runtime_error {
   public:
    NoResponseException() : std::runtime_error("No response") {}
    NoResponseException(const std::string &msg) : std::runtime_error(msg) {}
};

class InvalidBucketNameException : public std::runtime_error {
   public:
    InvalidBucketNameException(const std::string &name, const std::string &msg)
        : std::runtime_error(name + " : " + msg) {}
};

class InvalidEndpointException : public std::runtime_error {
   public:
    InvalidEndpointException(const std::string &name, const std::string &msg)
        : std::runtime_error(name + " : " + msg) {}
};

class InvalidPortException : public std::runtime_error {
   public:
    InvalidPortException(int port, const std::string &msg)
        : std::runtime_error(std::to_string(port) + " : " + msg) {}
};

#endif
