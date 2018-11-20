#include <minioclient.h>
#include <iostream>
#include <string>

int main() {
    try {
        MinioClient minioClient("http://127.0.0.1:9000", "Y87GXVKRA7JUGN705P9V",
                                "tC5AdH3WI5CDGflF2gdSRqEvtqslgMNakiCDpFg+");
        minioClient.traceOn(std::cout);

        std::string bucket("newbucket");
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
}
