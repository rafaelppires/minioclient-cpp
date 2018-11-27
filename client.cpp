#include <argp.h>
#include <minioclient.h>
#include <iostream>
#include <string>

const char *argp_program_version = "minioclient-cpp";
const char *argp_program_bug_address = "<rafael.pires@unine.ch>";

static char doc[] = "minioclient-cpp test";
static char args_doc[] = "";
static struct argp_option options[] = {
    {"endpoint", 'm', "http[s]://<host>:<port>", 0, "Minio server endpoint"},
    {"access", 'a', "ACCESSKEY", 0, "Minio access key"},
    {"secret", 'k', "SECRETKEY", 0, "Minio secret key"},
    {0}};

struct Arguments {
    std::string minioendpoint, minioaccesskey, miniosecret;
};

//------------------------------------------------------------------------------
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    Arguments *args = (Arguments *)state->input;
    switch (key) {
        case 'm':
            args->minioendpoint = arg;
            break;
        case 'a':
            args->minioaccesskey = arg;
            break;
        case 'k':
            args->miniosecret = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    };
    return 0;
}

//------------------------------------------------------------------------------
int main(int argc, char **argv) {
    Arguments args;
    struct argp argp = {options, parse_opt, 0, doc};
    argp_parse(&argp, argc, argv, 0, 0, &args);

    if (args.minioendpoint.empty() || args.minioaccesskey.empty() ||
        args.miniosecret.empty()) {
        std::cerr << "All arguments required: -m http[s]://<host>:<port> -a "
                     "<ACCESSKEY> -k <SECRETKEY>"
                  << std::endl;
        return 1;
    }

    try {
        MinioClient minioClient(args.minioendpoint, args.minioaccesskey,
                                args.miniosecret);
        minioClient.traceOn(std::cout);

        std::string bucket("asiatripy");
        bool isExist = minioClient.bucketExists(bucket);
        if (isExist) {
            std::cout << "Bucket already exists." << std::endl;
        } else {
            minioClient.makeBucket(bucket);
        }

        std::string filepath = "client.cpp", remotename = "content.txt";
        minioClient.putObject(bucket, remotename, filepath);
        std::cout << filepath + " is successfully uploaded as '" + remotename +
                         "' to '" + bucket + "' bucket."
                  << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
