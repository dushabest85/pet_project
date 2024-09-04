#include <vector>
#include <iostream>

auto getMax(const std::vector<int> &vec)
{
    auto itMax = vec.begin();
    for (auto it = vec.begin(); it != vec.end(); ++it)
    {
        if (*itMax < *it)
            itMax = it;
    }

    return itMax;
}

int main()
{
    std::vector<int> vec{5, 1, 3, 6, 2};
    std::vector<int> res;

    while (!vec.empty())
    {
        auto max = getMax(vec);
        res.emplace_back(std::move(*max));
        vec.erase(max);
    }

    for (const auto item : res)
    {
        std::cout<<item<<std::endl;
    }

    return 0;
}
