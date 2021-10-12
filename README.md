# CppThreadPool
## Introduction
The original intention of this project is to learn the new C++20 standard in use. Therefore, make sure your compiler supports C++20 or later standards.

**TODO :**
- [x] Smart pointer as parameter
  - [x] Solve memory leaking of shared_ptr + lambda
- [x] New function wrapper that supports move semantics
- [x] Locked task queue
- [ ] Unlocked task queue
- [ ] Coroutine supported
- [ ] (Try) High concurrency support
  - [ ] High concurrency test

[comment]: <> (- [ ] &#40;Maybe&#41; Higher performance smart pointer than shared_ptr)

## Installation
- Clone the repo.
```shell
git clone https://github.com/JieRen98/CppThreadPool.git
cd CppThreadPool
```
- Include the directory in your project.
```cmake
# ...
include_directories("path/to/the/dir/CppThreadPool")
# ...
```
- Enjoy.

### Example
#### Compile test program
```shell
mkdir test

echo "
#include <iostream>
#include <ThreadPool.hpp>

void run_ref(int &x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    x++;
    std::cout << "value address: " << &x << " in run_ref" << std::endl;
}

std::string run_ref_ret_value(int &x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    x++;
    std::cout << "value address: " << &x << " in run_ref_ret_value" << std::endl;
    return "string from run_ref_ret_value";
}

void run_cp(int x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    x++;
    std::cout << "value address: " << &x << " in run_cp" << std::endl;
}

int main() {
    ThreadPool::ThreadPool_t tp{ 3 };
    auto sptr = std::make_shared<int>(0);
    auto uptr = std::make_unique<int>(0);
    std::cout << "value address: " << sptr.get() << ", start with: " << *sptr << std::endl;
    tp.start();

    auto future1 = tp.Submit(run_ref, sptr)->get_future();
    auto future2 = tp.Submit(run_cp, sptr)->get_future();
    auto future3 = tp.Submit(run_ref_ret_value, std::move(uptr))->get_future();
    future1.wait();
    future2.wait();
    std::cout << "sptr: " << *sptr << ", use count: " << sptr.use_count() << std::endl;
    std::cout << future3.get() << std::endl;
    tp.shutdown();
}
" > test/test.cpp

echo "add_executable(test test.cpp)" > test/CmakeList.txt

mkdir build && cd build

cmake .. && make
```

#### Run the program
```shell
cd test
./test
```
result:
```
value address: 0xefb100, start with: 0
value address: 0xefb100 in run_ref
value address: 0xefb110 in run_ref_ret_value
value address: 0x7f44f7b0d7ac in run_cp
sptr: 1, use count: 1
string from run_ref_ret_value
```

#### Explanation
The test code as following:
```c++
#include <iostream>
#include <ThreadPool.hpp>

void run_ref(int &x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    x++;
    std::cout << "value address: " << &x << " in run_ref" << std::endl;
}

std::string run_ref_ret_value(int &x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    x++;
    std::cout << "value address: " << &x << " in run_ref_ret_value" << std::endl;
    return "string from run_ref_ret_value";
}

void run_cp(int x) {
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    x++;
    std::cout << "value address: " << &x << " in run_cp" << std::endl;
}

int main() {
    ThreadPool::ThreadPool_t tp{3   };
    auto sptr = std::make_shared<int>(0);
    auto uptr = std::make_unique<int>(0);
    std::cout << "value address: " << sptr.get() << ", start with: " << *sptr << std::endl;
    tp.start();

    auto future1 = tp.Submit(run_ref, sptr)->get_future();
    auto future2 = tp.Submit(run_cp, sptr)->get_future();
    auto future3 = tp.Submit(run_ref_ret_value, std::move(uptr))->get_future();
    future1.wait();
    future2.wait();
    std::cout << "sptr: " << *sptr << ", use count: " << sptr.use_count() << std::endl;
    std::cout << future3.get() << std::endl;
    tp.shutdown();
}
```

As we can see, we support unique_ptr and shared_ptr. Notably, we should use `std::move` to transfer the control of the unique_ptr.

At the same time, thanks to the pointer setting, we can easily use lvalue reference in our submitted function.
Several existing thread pools have difficulty in passing reference. Some require users to pass reference by using `std::ref` that will cause some problems in type deduction.
