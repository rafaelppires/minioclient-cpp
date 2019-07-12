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
## License

Copyright 2018 Rafael Pires

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

