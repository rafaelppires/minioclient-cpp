#include <minioclient.h>
#include <iostream>
#include <string>

int main() {
    try {
        MinioClient minioClient("http://127.0.0.1:9000", "Y87GXVKRA7JUGN705P9V",
                                "tC5AdH3WI5CDGflF2gdSRqEvtqslgMNakiCDpFg+");
        minioClient.traceOn(std::cout);
        bool isExist = minioClient.bucketExists("asiatrip");
        if (isExist) {
            std::cout << "Bucket already exists." << std::endl;
        } else {
            // Make a new bucket called asiatrip to hold a zip file of photos.
            minioClient.makeBucket("asiatrip");
        }

        std::string filepath =
            "/Users/rafaelpp/Temp/minioclient-cpp/client.cpp";
        minioClient.putObject("asiatrip", "asiaphotos.txt", filepath);
        std::cout << filepath +
                         " is successfully uploaded as asiaphotos.txt to "
                         "`asiatrip` bucket.";
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
