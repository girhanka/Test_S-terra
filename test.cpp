#include <iostream>
#include <bitset>
#include <list>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <thread>
#include <mutex>
#include <ctime>

uint32_t count_0 = 0, count_thr1 = 0, count_1 = 0, count_thr2 = 0;
std::mutex mtx;

void thr(std::list<uint16_t> &lst, bool bit, uint32_t &bitcount, uint32_t &count_thr)
{
    while (!lst.empty()) {
        // choosing current element
        mtx.lock();
        std::list<uint16_t>::iterator it = bit ? lst.begin() : --lst.end();
        
        if (it == lst.end()) 
            break;
        // count zeros in *it bitset
        bitcount += bit ? (std::bitset<16>(*it).count()) : (std::bitset<16>(*it).size() - std::bitset<16>(*it).count());
        count_thr++;
        //bit ? lst.pop_back() : lst.pop_front();
        lst.erase(it);
        mtx.unlock();
    }
}


int main()
{
    std::cout << "hardware concurrency = " << std::thread::hardware_concurrency() << '\n';
    
    // creating list
    std::list<uint16_t> lst;
    
    uint32_t lst_size = 1000000;

    // filling the list with random values
    std::srand(std::time(NULL)); // current tiem as seed
    for (int i = 0; i < lst_size; ++i) 
        lst.push_back(std::rand() % 10000);
    
    // output values
    //std::cout << "list = { ";
    //for (uint16_t i : lst)
    //    std::cout << i << ' ';
    //std::cout << " }\n";

    double start = std::clock(); 
    
    // creating 2 threads
    std::thread thr1(thr, std::ref(lst), 0, std::ref(count_0), std::ref(count_thr1));
    std::thread thr2(thr, std::ref(lst), 1, std::ref(count_1), std::ref(count_thr2));

    thr1.join();
    thr2.join();

    double end = std::clock();

    std::cout << "Duration = " << (end - start) / CLOCKS_PER_SEC << '\n';

    std::cout << "The number of zeros counted by thread 1 is " << count_0 << '\n';
    std::cout << "The number of elements processed by thread 1 is " << count_thr1 << '\n';
    std::cout << "The number of ones counted by thread 2 is " << count_1 << '\n';
    std::cout << "The number of elements processed by thread 2 is " << count_thr2 << '\n';
}
