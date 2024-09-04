#include <vector>
#include <iostream>

int binarySearch(const std::vector<int> &vec, const int target)
{
    int low = 0;
    int hight = vec.size() - 1;

    while (low <= hight)
    {
        int mid = (low + hight) / 2;
        int value = vec[mid];

        if (value == target)
            return mid;

        if (target > value)
            low = mid + 1;
        else
            hight = mid - 1;
    }

    return -1;
}

int main()
{
    std::vector<int> vec{1, 2, 3, 4};
    const int target = 1;
    std:: cout<<binarySearch(vec, target)<<std::endl;

    return 0;
}
