#include "camera.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/geometric.hpp>

#include "defines.h"
#include <stdio.h>
#include <iostream>

// Spherical Camera
Camera::Camera():
    _rho(60),
    _phi(glm::radians(45.0f)),
    _theta(glm::radians(270.0f)),
    _zoom_val(2.0f),
    _camera_pos(glm::vec3(0.0,-10.0,0.0))
{}

void Camera::setIsometric()
{
    _theta = -1.0f * M_PI / 4.0f;
    _phi = M_PI / 6.0f;
}

void Camera::zoom(float amount)
{
    _rho -= amount;
    _zoom_val += amount * 0.1f;

    if (_zoom_val < 0.1f) _zoom_val = 0.1f;
}

void Camera::rotateDirection(float amount, bool axis)
{
    if(!axis)
    {
        _theta += amount;
        while(_theta <= 0) _theta += 2*M_PI;
        while(_theta >= 2*M_PI) _theta -= 2*M_PI;
    }
    else
    {
        if(_phi+amount >= 0 && _phi+amount <= M_PI)
            _phi += amount;
    }
}

void Camera::translate(glm::vec3 offset)
{
    _camera_pos += offset;
    _lookat += offset;
}

glm::mat4 Camera::getPartMatrix()
{
    // Just compute the negative rotation wrt the part
    glm::mat4 rot(1.0), scale(1.0);

    rot = glm::rotate(rot, _phi, glm::vec3(1.0f, 0.0f, 0.0f));
    rot = glm::rotate(rot, -1.0f * _theta, glm::vec3(0.0f, 0.0f, 1.0f));

    // We also need to scale our part
    scale = glm::scale(scale, glm::vec3(_zoom_val, _zoom_val, _zoom_val));

    return rot * scale;
}

glm::mat4 Camera::getViewProjectionMatrix()
{
    // First, find the camera position
    //float cam_x, cam_y, cam_z;
    //cam_x = _rho * cos(_theta) * sin(_phi);
    //cam_y = _rho * sin(_theta) * sin(_phi);
    //cam_z = _rho * cos(_phi);
    //glm::vec3 cameraPos = glm::vec3(cam_x, cam_y, cam_z);
    
    glm::mat4 view (1.0f), projection (1.0f);

    //printf("cameraPos: %f %f %f\n", cam_x, cam_y, cam_z);
    //view = glm::lookAt(cameraPos, glm::vec3(0,0,0), glm::vec3(0,0,1));

    // Testing with a fixed camera atm 
    view = glm::lookAt(_camera_pos, _lookat, glm::vec3(0,0,1));
    //projection = glm::perspective(glm::radians(50.0f), 1.0f, 0.1f, 1000.0f);
    //glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT, 0.1f, 100.0f);
    projection = glm::ortho(-(float)SCREEN_WIDTH/2.0f, (float)SCREEN_WIDTH/2.0f, -(float)SCREEN_HEIGHT/2.0f, (float)SCREEN_HEIGHT/2.0f, -1000.0f, 1000.0f);

    return projection * view;
}
