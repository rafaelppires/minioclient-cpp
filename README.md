# minioclient-cpp

C++ port of [minio-java](https://github.com/minio/minio-java).
Many features missing.
Probably working for:
* Checking the existence of a bucket
* Creating a bucket
* Pushing a file to an existing bucket (one shot only, no multi-chunk)

To compile:
```
$ make
```
With debug symbols:
```
$ make DEBUG=1
```

