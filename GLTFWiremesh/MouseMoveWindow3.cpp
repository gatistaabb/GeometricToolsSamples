// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2019
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 4.0.2019.08.13

#include<iostream>

#include <Applications/GTApplicationsPCH.h>
#include "MouseMoveWindow3.h"
using namespace gte;

MouseMoveWindow3::MouseMoveWindow3(Parameters& parameters)
    :
    Window(parameters),
    mUpdater([this](std::shared_ptr<Buffer> const& buffer){ mEngine->Update(buffer); }),
    mCamera(std::make_shared<Camera>(true, mEngine->HasDepthRange01())),
    mFreeMouseCameraRig(mCamera, 0.0f, 0.0f),
    mPVWMatrices(mCamera, mUpdater),
    mTrackBall(mXSize, mYSize, mCamera),
    mouse_x(0),
    mouse_y(0)
{
    mFreeMouseCameraRig.RegisterMoveForward(KEY_UP);
    mFreeMouseCameraRig.RegisterMoveBackward(KEY_DOWN);
    mFreeMouseCameraRig.RegisterMoveUp(KEY_HOME);
    mFreeMouseCameraRig.RegisterMoveDown(KEY_END);
    mFreeMouseCameraRig.RegisterMoveRight(KEY_RIGHT);
    mFreeMouseCameraRig.RegisterMoveLeft(KEY_LEFT);
    mFreeMouseCameraRig.RegisterTurnRight(MOUSE_MOVE_RIGHT);
    mFreeMouseCameraRig.RegisterTurnLeft(MOUSE_MOVE_LEFT);
    mFreeMouseCameraRig.RegisterLookUp(MOUSE_MOVE_UP);
    mFreeMouseCameraRig.RegisterLookDown(MOUSE_MOVE_DOWN);
}

void MouseMoveWindow3::InitializeCamera(float upFovDegrees, float aspectRatio, float dmin, float dmax,
    float translationSpeed, float rotationSpeed, std::array<float, 3> const& pos,
    std::array<float, 3> const& dir, std::array<float, 3> const& up)
{
    mCamera->SetFrustum(upFovDegrees, aspectRatio, dmin, dmax);
    Vector4<float> camPosition{ pos[0], pos[1], pos[2], 1.0f };
    Vector4<float> camDVector{ dir[0], dir[1], dir[2], 0.0f };
    Vector4<float> camUVector{ up[0], up[1], up[2], 0.0f };
    Vector4<float> camRVector = Cross(camDVector, camUVector);
    mCamera->SetFrame(camPosition, camDVector, camUVector, camRVector);

    mFreeMouseCameraRig.ComputeWorldAxes();
    mFreeMouseCameraRig.SetTranslationSpeed(translationSpeed);
    mFreeMouseCameraRig.SetRotationSpeed(rotationSpeed);
}

bool MouseMoveWindow3::OnResize(int xSize, int ySize)
{
    if (Window::OnResize(xSize, ySize))
    {
        float upFovDegrees, aspectRatio, dMin, dMax;
        mCamera->GetFrustum(upFovDegrees, aspectRatio, dMin, dMax);
        mCamera->SetFrustum(upFovDegrees, GetAspectRatio(), dMin, dMax);
        mPVWMatrices.Update();
        return true;
    }
    return false;
}

bool MouseMoveWindow3::OnCharPress(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 't':  // Slower camera translation.
        mFreeMouseCameraRig.SetTranslationSpeed(0.5f * mFreeMouseCameraRig.GetTranslationSpeed());
        return true;

    case 'T':  // Faster camera translation.
        mFreeMouseCameraRig.SetTranslationSpeed(2.0f * mFreeMouseCameraRig.GetTranslationSpeed());
        return true;

    case 'r':  // Slower camera rotation.
        mFreeMouseCameraRig.SetRotationSpeed(0.5f * mFreeMouseCameraRig.GetRotationSpeed());
        return true;

    case 'R':  // Faster camera rotation.
        mFreeMouseCameraRig.SetRotationSpeed(2.0f * mFreeMouseCameraRig.GetRotationSpeed());
        return true;
    }

    return Window::OnCharPress(key, x, y);
}

bool MouseMoveWindow3::OnKeyDown(int key, int, int)
{
    return mFreeMouseCameraRig.PushMotion(key);
}

bool MouseMoveWindow3::OnKeyUp(int key, int, int)
{
    return mFreeMouseCameraRig.PopMotion(key);
}

bool MouseMoveWindow3::OnMouseClick(MouseButton button, MouseState state, int x, int y, unsigned int)
{
    if (button == MOUSE_LEFT)
    {
        if (state == MOUSE_DOWN)
        {
            mTrackBall.SetActive(true);
            mTrackBall.SetInitialPoint(x, mYSize - 1 - y);
        }
        else
        {
            mTrackBall.SetActive(false);
        }

        return true;
    }

    return false;
}

bool MouseMoveWindow3::OnMouseMotion(MouseButton button, int x, int y, unsigned int)
{
    if (button == MOUSE_LEFT && mTrackBall.GetActive())
    {
        mTrackBall.SetFinalPoint(x, mYSize - 1 - y);
        mPVWMatrices.Update();
        return true;
    }
    else {
        int dx = x - mouse_x;
        int dy = y - mouse_y;
        if(abs(dx) > abs(dy)) {
            if(x > mouse_x) {
                //look right
                mFreeMouseCameraRig.PushMotion(MOUSE_MOVE_RIGHT);
            }
            else {
                // look left
                mFreeMouseCameraRig.PushMotion(MOUSE_MOVE_LEFT);
            }
        }
        else {
            if(y > mouse_y) {
                //look up
                mFreeMouseCameraRig.PushMotion(MOUSE_MOVE_UP);
            }
            else {
                // look down
                mFreeMouseCameraRig.PushMotion(MOUSE_MOVE_DOWN);
            }
        }
        mouse_x = x;
        mouse_y = y;

        return true;
    }


    return false;
}
