#include "camera.h"

Camera::Camera()
{
    projectionMatrix.setToIdentity();
    computeViewMatrix();
}

// Now we must set the view matrix to transform the world coordinates to eye coordinates.
void Camera::computeViewMatrix()
{
    viewMatrix.setToIdentity();
    // LookAt method sets up the Camera. It defines a matrix multiplication
    viewMatrix.lookAt(eye, center, up);
}

void Camera::resizeViewPort(int width, int height)
{
    // Now that the camera is in place, we need to set how the images are represented on the projection plane.
    projectionMatrix.setToIdentity();

    float aspectRadio = static_cast<float>(width) / static_cast<float>(height);

    float verticalAngle = 20;
    float nearPlane = 0.1f;
    float farPlane = 100;

    // Projectors converge to the center of projection.
    // Multiplies this matrix by another that applies a perspective projection.
    // The vertical field of view will be verticalAngle degrees within a window with a given aspectRatio that determines the horizontal field of view.
    // The projection will have the specified nearPlane and farPlane clipping planes which are the distances from the viewer to the corresponding planes.
    // In the previous matrix, we must assume: l = −r, b = −t and compute t = n * tan(verticalAngle) and r = t × aspectRatio.
    projectionMatrix.perspective(verticalAngle, aspectRadio, nearPlane, farPlane);
}
