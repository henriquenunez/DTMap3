#include "camera.hpp"

#include "glm/gtc/matrix_transform.hpp"
#include <glm/glm.hpp>
#include <glm/geometric.hpp>

#include "globals.h"
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

void Camera::setTopView()
{
    _theta = 0.0f;
    _phi = M_PI / 2.0f;
}

void Camera::setSideView()
{
    _theta = M_PI / 2.0f;
    _phi = 0.0f;
}

void Camera::setFrontView()
{
    _theta = 0.0f;
    _phi = 0.0f;
}

void Camera::setDefault()
{
    //[Camera] rho: -35.000000 theta: -0.785398 phi: 0.523599
    //[Camera] pos: (15.000000, -4.000000, 0.000000) lookat: (15.000000, 6.000000, 0.000000)
    //[Camera] rho: -35.000000 theta: -0.785398 phi: 0.523599, zoom: 19.500000
    //[Camera] pos: (14.000000, -4.000000, 48.000000) lookat: (14.000000, 6.000000, 48.000000)
    _rho = -35.0f;
    setIsometric();

    _zoom_val = 19.5;
    _camera_pos = glm::vec3(15.000000, -4.000000, 48.000000);;
    _lookat = glm::vec3(15.000000, 6.000000, 48.000000);
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
        if(_phi+amount >= -M_PI && _phi+amount <= M_PI)
            _phi += amount;
    }
}

void Camera::translate(glm::vec3 offset)
{
    _camera_pos += offset;
    _lookat += offset;
}

void Camera::reCenter()
{
    _camera_pos = glm::vec3(0.0f);
    _lookat = glm::vec3(0.0f);
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

    //view = glm::lookAt(cameraPos, glm::vec3(0,0,0), glm::vec3(0,0,1));

    view = glm::lookAt(_camera_pos, _lookat, glm::vec3(0,0,1));
    //projection = glm::perspective(glm::radians(50.0f), 1.0f, 0.1f, 1000.0f);
    //glm::mat4 projection = glm::ortho(0.0f, (float)SCREEN_WIDTH, 0.0f, (float)SCREEN_HEIGHT, 0.1f, 100.0f);
    projection = glm::ortho(-(float)SCREEN_WIDTH/2.0f, (float)SCREEN_WIDTH/2.0f, -(float)SCREEN_HEIGHT/2.0f, (float)SCREEN_HEIGHT/2.0f, -1000.0f, 1000.0f);

    return projection * view;
}

void Camera::printInfo()
{
    printf("[Camera] rho: %f theta: %f phi: %f, zoom: %f\n", _rho, _theta, _phi, _zoom_val);
    printf("[Camera] pos: (%f, %f, %f) lookat: (%f, %f, %f)\n",
	    _camera_pos.x,
	    _camera_pos.y,
	    _camera_pos.z,
	    _lookat.x,
	    _lookat.y,
	    _lookat.z);
}

