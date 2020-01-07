// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2019
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 4.0.2019.08.13

#pragma once

#include <Graphics/KeyframeController.h>

#include "MouseMoveWindow3.h"

using namespace gte;

class WireMeshWindow3 : public MouseMoveWindow3
{
public:
    WireMeshWindow3(Parameters& parameters);

    virtual void OnIdle() override;

    virtual bool OnResize(int xSize, int ySize) override;

private:
    Culler mCuller;

    bool SetEnvironment();
    bool CreateScene();
    
    std::shared_ptr<Node> mScene;

    double mApplicationTime, mApplicationDeltaTime;
};
