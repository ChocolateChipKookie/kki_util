#include <iostream>
#include <array>
#include <chrono>
#include "random.h"
#include "string.h"
#include <deque>

constexpr size_t hash(const char* str){
    const long long p = 131;
    const long long m = 4294967291; // 2^32 - 5, largest 32 bit prime
    long long total = 0;
    long long current_multiplier = 1;
    for (int i = 0; str[i] != '\0'; ++i){
        total = (total + current_multiplier * str[i]) % m;
        current_multiplier = (current_multiplier * p) % m;
    }
    return total;
}

void test_find(const kki::string& s, kki::random& rand, size_t tests){
    size_t l{0};
    for (size_t i = 0; i < tests; ++i){
        char elem = rand.random_alnum();
        //size_t len = rand.random_int(3);
        //std::vector<char> element(len + 1);
        //for(size_t j = 0; j < len; ++j){
        //    element[j] = rand.random_alnum();
        //}

        l += s.find(elem);
    }

    std::cout << "Find: " << l << std::endl;
}

void test_find_std(const std::string& s, kki::random& rand, size_t tests){
    size_t l{0};
    for (size_t i = 0; i < tests; ++i){
        char elem = rand.random_alnum();
        //size_t len = rand.random_int(3);
        //std::vector<char> element(len + 1);
        //for(size_t j = 0; j < len; ++j){
        //    element[j] = rand.random_alnum();
        //}

        l += s.find(elem);
    }

    std::cout << "Find std: " << l << std::endl;
}

void find_substr(kki::string& str, std::vector<std::vector<char>>& substrs){
    size_t l{0};
    for(auto& substr : substrs){
        l += str.find(substr.data());
    }
    std::cout << "Find kki: " << l << std::endl;
}

void find_substr(std::string& str, std::vector<std::vector<char>>& substrs){
    size_t l{0};
    for(auto& substr : substrs){
        l += str.find(substr.data());
    }
    std::cout << "Find std: " << l << std::endl;
}

void test_element(){
    kki::random rand(0);
    const size_t data_size = 10000000;
    std::vector<char> data;
    data.reserve(data_size);
    for(unsigned i = 0; i < data_size; ++i){
        data.emplace_back(rand.random_alnum());
    }

    std::string s;
    s.reserve(data_size);
    std::copy(data.begin(), data.end(), std::back_inserter(s));
    kki::string str(data.data(), data_size);

    kki::random r1(1);
    kki::random r2(1);

    auto begin1 = std::chrono::high_resolution_clock::now();
    test_find(str, r1, 10000);
    auto end1 = std::chrono::high_resolution_clock::now();

    auto begin2 = std::chrono::high_resolution_clock::now();
    test_find_std(s, r2, 10000);
    auto end2 = std::chrono::high_resolution_clock::now();

    std::cout << "Find " << (end1 - begin1).count() << std::endl;
    std::cout << "Find std " << (end2 - begin2).count() << std::endl;
}

void test_substrs(){
    kki::random rand(0);
    const size_t data_size = 10000000;
    std::vector<char> data;
    data.reserve(data_size);
    for(unsigned i = 0; i < data_size; ++i){
        data.emplace_back(rand.random_alnum());
    }

    std::string s;
    s.reserve(data_size);
    std::copy(data.begin(), data.end(), std::back_inserter(s));
    kki::string str(data.data(), data_size);

    const size_t test_size = 1000;
    std::vector<std::vector<char>> tests;
    tests.reserve(test_size);
    for(size_t i = 0; i < test_size; ++i){
        size_t len = rand.random_int(1,10);
        std::vector<char> element(len + 1);
        for(size_t j = 0; j < len; ++j){
            element[j] = rand.random_alnum();
        }
        tests.push_back(element);
    }

    auto begin1 = std::chrono::high_resolution_clock::now();
    find_substr(str, tests);
    auto end1 = std::chrono::high_resolution_clock::now();

    auto begin2 = std::chrono::high_resolution_clock::now();
    find_substr(s, tests);
    auto end2 = std::chrono::high_resolution_clock::now();

    std::cout << "Find     " << (end1 - begin1).count() << std::endl;
    std::cout << "Find std " << (end2 - begin2).count() << std::endl;
}

void test_find_all(){
    kki::random rand(0);
    const size_t data_size = 10000000;
    std::vector<char> data;
    data.reserve(data_size);
    for(unsigned i = 0; i < data_size; ++i){
        data.emplace_back(rand.random_alnum());
    }
    kki::string str(data.data(), data_size);

    const char test_begin = 'a';
    const char test_end = 'z';


    auto begin1 = std::chrono::high_resolution_clock::now();
    size_t total_1{0};
    for ( char i = test_begin; i <= test_end; ++i){
        auto res = str.find_all(i);
        total_1 += res.size();
    }
    auto end1 = std::chrono::high_resolution_clock::now();

    std::cout << "Find     " << (end1 - begin1).count() << std::endl;
}

int main() {

    kki::string a{"AdiDoloresMia"};

    std::cout << a << std::endl;
    std::cout << a.substr(0, 3) << std::endl;
    std::cout << a.substr(3, 7) << std::endl;
    std::cout << a.substr(10, 3) << std::endl;

    return 0;
    std::string val;

    switch (hash(val.c_str())){
        case hash("monday"):
            std::cout << "Have a nice monday" << std::endl;
            break;
        case hash("tuesday"):
            std::cout << "Have a nice tuesday" << std::endl;
            break;
        case hash("wednesday"):
            std::cout << "Have a nice wednesday" << std::endl;
            break;
        case hash("thursday"):
            std::cout << "Have a nice thursday" << std::endl;
            break;
        case hash("friday"):
            std::cout << "Have a nice friday" << std::endl;
            break;
        default:
            std::cout << "It is the weekend" << std::endl;
    }
    return 0;
}
