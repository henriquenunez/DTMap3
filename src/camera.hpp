#ifndef __CAMERA
#define __CAMERA

#include <glm/glm.hpp>

class Camera
{
public:
    Camera();

    void zoom(float amount);
    void rotateDirection(float amount, bool axis); // 2D rotation
    void translate(glm::vec3 offset);

    glm::mat4 getPartMatrix();
    glm::mat4 getViewProjectionMatrix();

    void setIsometric();
    void setTopView();
    void setSideView();
    void setFrontView();

    void reCenter();

private:
    float _rho, _phi, _theta, _zoom_val;
    glm::vec3 _camera_pos, _lookat;
    glm::mat4 _part_matrix;
};
/*
class RegularCamera
{
public:
    RegularCamera();

    void zoom(float amount);
    void rotateDirection(float amount, bool axis); // 2D rotation

    glm::mat4 getViewProjectionMatrix();

private:
    float rho, phi, theta;
};

*/
#endif
