#include <chrono>
#include <random>
#include <iostream>

#include <boost/random.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

int main()
{
    // cc自带的都是引擎加分布
    // 11的
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int> dist(0, 99);
    std::cout << dist(gen) << '\n';   

    // 时间戳作种子
    std::mt19937_64 gen2(std::chrono::system_clock::now().time_since_epoch().count());
    std::cout << dist(gen2) << '\n';

    // boost: random
    // 正态分布
    boost::random::normal_distribution<double> normal(0.0, 1.0);

    // 泊松分布
    boost::random::poisson_distribution<int> poisson(3.0);

    // 伽马分布
    boost::random::gamma_distribution<double> gamma(2.0);

    // 二项分布
    boost::random::binomial_distribution<int> binomial(10, 0.5);

    boost::uuids::random_generator gen3;
    boost::uuids::uuid uuid = gen3();
    std::cout << uuid << std::endl;
}