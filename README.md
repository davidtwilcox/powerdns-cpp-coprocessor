<!-- PROJECT LOGO -->
<p align="center">
    <h3 align="center">PowerDNS Co-processor</h3>
  <p align="center">
    A C++ pipe backend for the <a href="https://www.powerdns.com/">PowerDNS</a> open source DNS server.
    <br />
    <a href="https://github.com/bigangryguy/powerdns-cpp-coprocessor"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/bigangryguy/powerdns-cpp-coprocessor/issues">Report Bug</a>
    ·
    <a href="https://github.com/bigangryguy/powerdns-cpp-coprocessor/issues">Request Feature</a>
  </p>
</p>

<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
  * [Built With](#built-with)
* [Getting Started](#getting-started)
  * [Prerequisites](#prerequisites)
  * [Installation](#installation)
* [Usage](#usage)
* [Roadmap](#roadmap)
* [Contributing](#contributing)
* [License](#license)
* [Contact](#contact)
* [Acknowledgements](#acknowledgements)



<!-- ABOUT THE PROJECT -->
## About The Project
A C++ pipe backend for the [PowerDNS](https://www.powerdns.com/) open source DNS server.
This is an example project for implementing a co-processor in C++, showing how to 
read the PowerDNS input via pipes and manage v1 - v3 ABIs.

### Built With

* [{fmt}](https://fmt.dev/latest/index.html)
* [Catch2](https://github.com/catchorg/Catch2)
* [Crypto++](https://cryptopp.com/)

<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running follow these simple steps. The instructions assume you are on a
Linux or Unix-like system. I can verify this will work on macOS as well.

### Prerequisites

The PowerDNS co-processor requires the {fmt} and Catch2 libraries to be installed and 
able to be found by cmake.
* {fmt} - Download and unzip the library first. You can follow the latest [instructions](https://fmt.dev/latest/usage.html) or try:
```shell script
$ cd fmt-7.0.3  # Change to the library directory
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make test
$ sudo make install
```
* Catch2 - Follow the latest [instructions](https://github.com/catchorg/Catch2/blob/master/docs/cmake-integration.md#installing-catch2-from-git-repository) or try:
```shell script
$ git clone https://github.com/catchorg/Catch2.git
$ cd Catch2
$ cmake -Bbuild -H. -DBUILD_TESTING=OFF
$ sudo cmake --build build/ --target install
```
* Crypto++ - Version 8.2 of the library is part of this repo. However, you do need to download and add the static library
to your repo under the `src/lib/` path. Get the static library at the Crypto++ [download](https://cryptopp.com/#download)
page

### Installation

1. Clone the repo
```sh
git clone https://github.com/bigangryguy/powerdns-cpp-coprocessor.git
```
2. Install prerequisites (see above)
3. Create your build files (run these commands in the PowerDNS co-processor root directory)
```shell script
$ cmake -DCMAKE_BUILD_TYPE=Debug -G "YOUR_PREFERRED_IDE" PATH_TO_SOURCE
```

<!-- USAGE EXAMPLES -->
## Usage

The best usage reference can be found in the PowerDNS pipe backend [documentation](https://doc.powerdns.com/authoritative/backends/pipe.html)

<!-- ROADMAP -->
## Roadmap

None - this is a complete example project. Feel free to fork and extend it for v4 and v5
ABIs, or add new features to what you see here.


<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.



<!-- CONTACT -->
## Contact

David Wilcox - [@davidtwilcox](https://twitter.com/davidtwilcox) - david@dtwil.co

Project Link: [https://github.com/bigangryguy/powerdns-cpp-coprocessor](https://github.com/bigangryguy/powerdns-cpp-coprocessor)


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/bigangryguy/repo.svg?style=flat-square
[contributors-url]: https://github.com/bigangryguy/repo/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/bigangryguy/repo.svg?style=flat-square
[forks-url]: https://github.com/bigangryguy/repo/network/members
[stars-shield]: https://img.shields.io/github/stars/bigangryguy/repo.svg?style=flat-square
[stars-url]: https://github.com/bigangryguy/repo/stargazers
[issues-shield]: https://img.shields.io/github/issues/bigangryguy/repo.svg?style=flat-square
[issues-url]: https://github.com/bigangryguy/repo/issues
[license-shield]: https://img.shields.io/github/license/bigangryguy/repo.svg?style=flat-square
[license-url]: https://github.com/bigangryguy/repo/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=flat-square&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/davidtwilcox
[product-screenshot]: images/screenshot.png
