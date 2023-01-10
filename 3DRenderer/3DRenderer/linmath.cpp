#include <iostream>
#include "linmath.h"

void linmath::rotateMetricies(Vector3D& degree, float* out)
{
    float thetaX = linmath::deg2radians(degree.x);
    float thetaY = linmath::deg2radians(degree.y);
    float thetaZ = linmath::deg2radians(degree.z);

    float rot1[16] = {
                        1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 1, 0
    };
    float rot2[16] = {
                        1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 1, 0
    };
    float rot3[16] = {
                        1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 1, 0
    };
    
    rot1[0] = std::cos(thetaZ);
    rot1[1] = -std::sin(thetaZ);
    rot1[4] = -rot1[1];
    rot1[5] = rot1[0];
    
    rot2[0] = std::cos(thetaY);
    rot2[2] = -std::sin(thetaY);
    rot2[8] = -rot2[2];
    rot2[10] = rot2[0];

    rot3[5] = std::cos(thetaX);
    rot3[6] = -std::sin(thetaX);
    rot3[9] = -rot3[6];
    rot3[10] = rot3[5];

    float result[16] = {
                        1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 1, 0
    };

    multiplyMetricies4x4(rot1, rot2, result);
    multiplyMetricies4x4(result, rot3, out);
}

linmath::vec3 linmath::multiplyByMetricies4x4(float* metricies, vec3 position)
{
    vec3 out = { 0, 0, 0 };
    out.x = metricies[0] * position.x + metricies[1] * position.y + metricies[2] * position.z;
    out.y = metricies[4] * position.x + metricies[5] * position.y + metricies[6] * position.z;
    out.z = metricies[8] * position.x + metricies[9] * position.y + metricies[10] * position.z;
    
    return out;
}

void linmath::perspective(float width, float height, float fov, float far, float near, float* out)
{
    out[5] = 1 / std::tan(fov / 2);
    out[0] = ((float)height / width) * out[5];
    out[10] = far / (far - near);
    out[11] = -far / (far - near) * near;
}
float linmath::deg2radians(float degree)
{
	return degree * PI / 180;
}

void linmath::multiplyMetricies4x4(float* matrix1, float* matrix2, float* out)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            out[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++)
            {
                out[i * 4 + j] += matrix1[i * 4 + k] * matrix2[k * 4 + j];
            }
        }
    }
}

linmath::vec3 linmath::addVector3dByVec3(const Vector3D vec1, linmath::vec3 vec2)
{
    return { vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z };
}

linmath::vec3 linmath::subVector3dByVec3(Vector3D vec1, vec3 vec2)
{
    return { vec2.x - vec1.x, vec2.y - vec1.y, vec2.z - vec1.z };
}
linmath::vec3 linmath::mulVector3dByVec3(Vector3D vec1, vec3 vec2)
{
    return { vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z };
}