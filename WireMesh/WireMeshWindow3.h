// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2019
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 4.0.2019.08.13

#pragma once

#include <Applications/Window3.h>
using namespace gte;

class WireMeshWindow3 : public Window3
{
public:
    WireMeshWindow3(Parameters& parameters);

    virtual void OnIdle() override;

    virtual bool OnResize(int xSize, int ySize) override;

	virtual bool OnCharPress(unsigned char key, int x, int y) override;

private:
    Culler mCuller;

    bool SetEnvironment();
    bool CreateScene();
	void RotateCamera(gte::Vector3<float> amount);
    
    std::shared_ptr<Node> mScene;
	std::shared_ptr<Visual*> culledScene;
};
