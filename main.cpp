#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <ranges>

/*A naming alias for std::vector is used to avoid confusing an array with a vector*/
namespace m_std
{
    template<typename T>
    using dynArray = std::vector<T>;
}

constexpr int imageSize{256};
double canvasScale{1.0};

/*This vector class can be of any size, +,* operators are overloaded to make vectors operations simpler to write.
() operator is overloaded so it can provide access to private elements of the vector.*/
class Vector
{
public:
    Vector(const std::initializer_list<double>& data):
        m_elements{data}
    {
    }

    friend std::ostream& operator <<(std::ostream& out, const Vector& vector)
    {
        for (int i{0}; i < static_cast<int>(vector.m_elements.size()); ++i)
        {
            out << vector.m_elements[i] << " ";
        }
        return out;
    }


    double& operator ()(const int index)
    {
        return m_elements[index];
    }

    friend Vector operator *(const Vector& ls_vector, const double& rs_factor)
    {
        Vector result = ls_vector;
        for (int i{0}; i < std::ssize(ls_vector.m_elements); ++i)
        {
            result.m_elements[i] = result.m_elements[i] * rs_factor;
        }
        return result;
    }

    friend Vector operator *(const double& ls_factor, const Vector& rs_vector)
    {
        Vector result = rs_vector;
        for (int i{0}; i < std::ssize(rs_vector.m_elements); ++i)
        {
            result(i) = result(i) * ls_factor;
        }
        return result;
    }

    friend Vector operator +(const Vector& ls_vector, const Vector& rs_vector)
    {
        Vector result = ls_vector;
        for (int i{0}; i < std::ssize(ls_vector.m_elements); ++i)
        {
            result.m_elements[i] = ls_vector.m_elements[i] + rs_vector.m_elements[i];
        }
        return result;
    }

    friend m_std::dynArray<double> operator +(const m_std::dynArray<double>& ls_arr, const Vector& rs_vector)
    {
        m_std::dynArray<double> result = ls_arr;
        for (int i{0}; i < std::ssize(ls_arr); ++i)
        {
            result[i] = ls_arr[i] + rs_vector.m_elements[i];
        }
        return result;
    }

    friend m_std::dynArray<double> operator +(const Vector& ls_vector, const m_std::dynArray<double>& rs_arr)
    {
        m_std::dynArray<double> result = rs_arr;
        for (int i{0}; i < std::ssize(rs_arr); ++i)
        {
            result[i] = rs_arr[i] + ls_vector.m_elements[i];
        }
        return result;
    }

private:
    m_std::dynArray<double> m_elements{};
};

Vector gravityDisplacement(const Vector& gravity, const double elapsedTime)
{
    return gravity * (std::pow(elapsedTime, 2) / 2);
}

bool isMaxElementSmaller(const m_std::dynArray<double>& arr1, const m_std::dynArray<double>& arr2)
{
    return *std::ranges::max_element(arr1) < *std::ranges::max_element(arr2);
}

/*Simulation settings*/
const m_std::dynArray<double> startingPosition{10, 50};
const Vector gravity = {0, -9.8};
const Vector projectileVelocity = {40, 80};
constexpr double timeStep{0.001};

int main()
{
    m_std::dynArray<m_std::dynArray<double> > gridCoordinates{startingPosition};
    m_std::dynArray<m_std::dynArray<int> > canvasCoordinates{};
    Vector currentVelocity = {};
    double currentTime{0.0};
    /*Calculates location based on velocity at a given time and saves it an dynamic array*/
    while (true)
    {
        currentVelocity = std::move(projectileVelocity + gravityDisplacement(gravity, currentTime));
        gridCoordinates.push_back(gridCoordinates.back() + currentVelocity * timeStep);
        if (gridCoordinates.back()[1] <= 0)
        {
            gridCoordinates.pop_back();
            break;
        }
        currentTime += timeStep;
    }

    /* Scales coordinates based on maximum value to fit within image bounds */
    auto maxCoordinate = std::ranges::max_element(gridCoordinates, isMaxElementSmaller);
    auto maxCoordinateValue = std::ranges::max_element(*maxCoordinate);

    while (true)
    {
        if (*maxCoordinateValue < imageSize * canvasScale)
        {
            break;
        }
        canvasScale += 0.1;
    }
    /*Converts grid coordinates into canvas coordinates*/
    for (auto& coordinate: gridCoordinates)
    {
        canvasCoordinates.push_back({
            static_cast<int>(coordinate[0] / canvasScale), static_cast<int>(coordinate[1] / canvasScale)
        });
    }

    std::fstream file{};
    /*Creates a white square ppm image*/
    file.open("canvas.ppm", std::ios::out);
    file << "P3\n" << imageSize << " " << imageSize << "\n255\n";
    for (int row{0}; row < imageSize; ++row)
    {
        for (int col{0}; col < imageSize; ++col)
        {
            file << 255 << " " << 255 << " " << 255 << "\n";
        }
    }
    /*Saves the image into dynamic arrays to ease modification*/
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
    /*Sets the color of each canvas coordinate to blue*/
    for (auto& coordinate: canvasCoordinates)
    {
        imageData[imageSize - 1 - coordinate[1]][coordinate[0]] = "0 0 255";
    }
    /*Saves the image data into a ppm image*/
    file.open("canvas.ppm", std::ios::out);
    for (auto& headerArgument: imageHeader)
    {
        file << headerArgument << "\n";
    }
    for (auto& row: imageData)
    {
        for (auto& pixel: row)
        {
            file << pixel << "\n";
        }
    }
    return 0;
}
