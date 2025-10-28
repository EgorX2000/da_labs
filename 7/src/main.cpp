#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

bool triangleBuildable(const int& a, const int& b, const int& c) {
    if (a < b + c && b < a + c && c < a + b) {
        return true;
    }
    return false;
}

double calculateArea(const int& a, const int& b, const int& c) {
    double p = (a + b + c) / 2.0;
    double areaSquared = p * (p - a) * (p - b) * (p - c);
    return std::sqrt(areaSquared);
}

int main() {
    int n;
    std::cin >> n;

    std::vector<int> segments;
    int input;
    while (std::cin >> input) {
        segments.push_back(input);
    }

    std::sort(segments.begin(), segments.end(), std::greater<int>());

    double area = 0, maxArea = 0;
    int a, b, c;
    for (int i = 0; i < segments.size() - 2; i++) {
        if (triangleBuildable(segments[i], segments[i + 1], segments[i + 2])) {
            area = calculateArea(segments[i], segments[i + 1], segments[i + 2]);
            if (area > maxArea) {
                maxArea = area;
                a = segments[i];
                b = segments[i + 1];
                c = segments[i + 2];
            }
        }
    }

    if (maxArea > 0) {
        std::array<int, 3> output = {a, b, c};
        std::sort(output.begin(), output.end());
        std::cout << std::fixed << std::setprecision(3);
        std::cout << maxArea << std::endl;
        std::cout << output[0] << " " << output[1] << " " << output[2]
                  << std::endl;
    } else {
        std::cout << maxArea << std::endl;
    }

    return 0;
}