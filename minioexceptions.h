// class NoSuchAlgorithmException {};
// class InsufficientDataException {};
// class IOException {};
// class InvalidKeyException {}
// class XmlPullParserException {};
// class InternalException {};
// class InsufficientDataException {};

class ErrorCode {
   public:
    enum Code { NO_ERROR, NO_SUCH_BUCKET };

    ErrorCode() : e_(NO_ERROR) {}
    ErrorCode(Code c) : e_(c) {}

    Code errorCode() const { return e_; }

   private:
    Code e_;
};

class ErrorResponseException : public std::runtime_error{
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
    InvalidBucketNameException( const std::string &name, const std::string &msg ) :
                        std::runtime_error(name + " : " +msg) {}
};

