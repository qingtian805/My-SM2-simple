# My-SM2-simple

**TO BUILD A SM2 BASED ON C/C++** is my 2021 Summer Pratical Training topic.  
This repo is a usage simple of the My-SM2 repo.  

## Introduction of files

Only **NEW FILES** and  will be introduced.  

### Prime Folder ./

**certify_server.cpp** A very simple usage of My-SM2, SM3 and SM4.This file is its server program source.  
**certify_client.cpp** A very simple usage of My-SM2, SM3 and SM4.This file is its client program source.  
**CMakeList.txt** CMake list, of my project. If you don`t know how it works, please refer to CMake.  
**OTPclient.cpp** OTP client main file, using TOTP.cpp.  
**README.md** Common ReadMe file.  

### Source ./src/

**sm4.cpp** SM4 source file by [tonyonce2017](https://github.com/tonyonce2017/SM4).  
**TOTP.cpp** C++ version of TOTP using [SM3](https://blog.csdn.net/qq_36298219/article/details/85926747).  

### Include file ./include/

**sm4.h** SM4 header by [tonyonce2017](https://github.com/tonyonce2017/SM4).  
**TOTP.h** Header of TOTP.cpp.  

### Libraries ./lib/

**libmiracl.a** Complied [MIRACL](https://github.com/miracl/MIRACL) lib file.   

### SM4-main ./SM4-main/

**README.md** README file of SM4 project by [tonyonce2017](https://github.com/tonyonce2017/SM4).

