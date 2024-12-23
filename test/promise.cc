#include <thread>
#include <future>
#include <iostream>

int main()
{
    // like packaged_task
    std::promise<std::uint64_t> promise;
    std::thread thr([](std::promise<std::uint64_t> &pro)
    {
        pro.set_value(1000);
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cout << "Sleep three seconds\n";
    }, std::ref(promise));
    
    // out of thread to get value
    std::future<std::uint64_t> future = promise.get_future();
    std::cout << future.get() << '\n';

    thr.join(); 
}
