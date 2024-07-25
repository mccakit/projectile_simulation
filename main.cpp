#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

constexpr int imageWidth{256};
constexpr int imageHeight{256};

class MyVector
{
public:
    MyVector(const std::initializer_list<double> &data):
        m_data{data}
    {
    }

    friend std::ostream &operator <<(std::ostream &out, const MyVector &my_vector)
    {
        for (int i{0}; i < my_vector.m_data.size(); ++i)
        {
            out << my_vector.m_data[i] << " ";
        }
        return out;
    }

    double &operator ()(const int index)
    {
        return m_data[index];
    }

    friend MyVector operator *(const MyVector &vector, const double scaler)
    {
        MyVector result = vector;
        for (int i{0}; i < vector.m_data.size(); ++i)
        {
            result.m_data[i] = result.m_data[i] * scaler;
        }
        return result;
    }

    friend MyVector operator *(const double scaler, const MyVector &vector)
    {
        MyVector result = vector;
        for (int i{0}; i < vector.m_data.size(); ++i)
        {
            result(i) = result(i) * scaler;
        }
        return result;
    }

    friend MyVector operator +(const MyVector &vector1, const MyVector &vector2)
    {
        MyVector result = vector1;
        for (int i{0}; i < vector1.m_data.size(); ++i)
        {
            result.m_data[i] = vector1.m_data[i] + vector2.m_data[i];
        }
        return result;
    }

private:
    std::vector<double> m_data{};
};

MyVector fallDistance(const MyVector &gravity, const double time)
{
    return gravity * (std::pow(time, 2) / 2);
}

int main()
{
    std::vector<MyVector> coordinates{{0, 0}};
    const MyVector gravity = {0, -9.8};
    const MyVector velocity = {30, 80};
    MyVector current_velocity = velocity;
    double time{0.0};
    constexpr double increment{0.1};
    while (true)
    {
        current_velocity = velocity + fallDistance(gravity, time);
        std::cout << current_velocity << "\n";
        MyVector new_position = coordinates.back() + current_velocity * increment;
        if (new_position(1) <= 0)
        {
            break;
        }
        coordinates.push_back(new_position);
        time += increment;
        std::cout << "Time: " << time << "s, Velocity: " << current_velocity << "\n";
    }
    std::vector<std::vector<int>> canvasCoordinates{};
    for (auto &coordinate: coordinates)
    {
        canvasCoordinates.push_back({static_cast<int>(coordinate(0)),static_cast<int>(coordinate(1))});
    }

    std::fstream file{};
    file.open("canvas.ppm", std::ios::out);
    file << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";
    for (int row{0}; row < imageHeight; ++row)
    {
        for (int col{0}; col < imageWidth; ++col)
        {
            file << 255 << " " << 255 << " " << 255 << "\n";
        }
    }

    std::vector<std::vector<std::string>> imageData(imageWidth,std::vector<std::string>(imageHeight));
    std::vector<std::string> imageHeader(3);
    std::string line{};
    file.close();

    file.open("canvas.ppm", std::ios::in);
    for (int i {0}; i < 3; ++i)
    {
        std::getline(file, line);
        imageHeader[i] = std::move(line);
    }
    for (int row {0}; row < imageHeight; ++row)
    {
        for (int col {0}; col < imageWidth; ++col)
        {
            std::getline(file, line);
            imageData[row][col] = std::move(line);
        }
    }
    file.close();

    for (auto &coordinate: canvasCoordinates)
    {
        imageData[imageHeight - 1 - coordinate[1]][coordinate[0]] = "0 0 255";
    }

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
}
