#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

namespace m_std
{
    template<typename T>
    using dynArray = std::vector<T>;
}

constexpr int imageSize{256};
double canvasScale{1.0};

class Vector
{
public:
    Vector(const std::initializer_list<double> &data):
        m_data{data}
    {
    }

    friend std::ostream &operator <<(std::ostream &out, const Vector &vector)
    {
        for (int i{0}; i < static_cast<int>(vector.m_data.size()); ++i)
        {
            out << vector.m_data[i] << " ";
        }
        return out;
    }


    double &operator ()(const int index)
    {
        return m_data[index];
    }

    friend Vector operator *(const Vector& vector, const double& factor)
    {
        Vector result = vector;
        for (int i{0}; i < static_cast<int>(vector.m_data.size()); ++i)
        {
            result.m_data[i] = result.m_data[i] * factor;
        }
        return result;
    }

    friend Vector operator *(const double& factor, const Vector &vector)
    {
        Vector result = vector;
        for (int i{0}; i < static_cast<int>(vector.m_data.size()); ++i)
        {
            result(i) = result(i) * factor;
        }
        return result;
    }

    friend Vector operator +(const Vector &vector1, const Vector &vector2)
    {
        Vector result = vector1;
        for (int i{0}; i < static_cast<int>(vector1.m_data.size()); ++i)
        {
            result.m_data[i] = vector1.m_data[i] + vector2.m_data[i];
        }
        return result;
    }
    friend m_std::dynArray<double> operator +(const m_std::dynArray<double> &arr, const Vector& vector)
    {
        m_std::dynArray<double> result = arr;
        for (int i{0}; i < static_cast<int>(arr.size()); ++i)
        {
            result[i] = arr[i] + vector.m_data[i];
        }
        return result;
    }
    friend m_std::dynArray<double> operator +(const Vector& vector,const m_std::dynArray<double> &arr)
    {
        m_std::dynArray<double> result = arr;
        for (int i{0}; i < static_cast<int>(arr.size()); ++i)
        {
            result[i] = arr[i] + vector.m_data[i];
        }
        return result;
    }

private:
    m_std::dynArray<double> m_data{};
};

Vector fallDistance(const Vector &gravity, const double time)
{
    return gravity * (std::pow(time, 2) / 2);
}
bool compareCoordinates(const m_std::dynArray<double>& arr1, const m_std::dynArray<double>& arr2)
{
    return *std::ranges::max_element(arr1) < *std::ranges::max_element(arr2);
}
const m_std::dynArray<double> startingPosition{10,50};
const Vector gravity = {0, -9.8};
const Vector projectileVelocity = {40, 80};
constexpr double timeStep{0.001};
int main()
{
    m_std::dynArray<m_std::dynArray<double>> coordinates{startingPosition};
    m_std::dynArray<m_std::dynArray<int>> canvasCoordinates{};
    Vector current_velocity = {};
    double currentTime{0.0};
    while (true)
    {
        current_velocity = std::move(projectileVelocity + fallDistance(gravity, currentTime));
        coordinates.push_back(coordinates.back() + current_velocity * timeStep);
        if (coordinates.back()[1] <= 0)
        {
            coordinates.pop_back();
            break;
        }
        currentTime += timeStep;
    }

    auto maxCoordinate = std::ranges::max_element(coordinates,compareCoordinates);
    auto maxVal = std::ranges::max_element(*maxCoordinate);

    while (true)
    {
        if(*maxVal < imageSize*canvasScale)
        {
            break;
        }
        canvasScale += 0.1;
    }

    for (auto coordinate: coordinates)
    {
        canvasCoordinates.push_back({static_cast<int>(coordinate[0]/canvasScale), static_cast<int>(coordinate[1]/canvasScale)});
    }

    std::fstream file{};
    file.open("canvas.ppm", std::ios::out);
    file << "P3\n" << imageSize << " " << imageSize << "\n255\n";
    for (int row{0}; row < imageSize; ++row)
    {
        for (int col{0}; col < imageSize; ++col)
        {
            file << 255 << " " << 255 << " " << 255 << "\n";
        }
    }

    m_std::dynArray<m_std::dynArray<std::string> > imageData(imageSize, std::vector<std::string>(imageSize));
    m_std::dynArray<std::string> imageHeader(3);
    std::string line{};
    file.close();

    file.open("canvas.ppm", std::ios::in);
    for (int i{0}; i < 3; ++i)
    {
        std::getline(file, line);
        imageHeader[i] = std::move(line);
    }
    for (int row{0}; row < imageSize; ++row)
    {
        for (int col{0}; col < imageSize; ++col)
        {
            std::getline(file, line);
            imageData[row][col] = std::move(line);
        }
    }
    file.close();

    for (auto &coordinate: canvasCoordinates)
    {
        imageData[imageSize - 1 - coordinate[1]][coordinate[0]] = "0 0 255";
    }

    file.open("canvas.ppm", std::ios::out);
    for (auto &headerArgument: imageHeader)
    {
        file << headerArgument << "\n";
    }
    for (auto &row: imageData)
    {
        for (auto &pixel: row)
        {
            file << pixel << "\n";
        }
    }
}
