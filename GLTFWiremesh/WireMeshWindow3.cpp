// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2019
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 4.0.2019.08.13

#include <iostream>
#include "WireMeshWindow3.h"
#include <Graphics/MeshFactory.h>
#include <Mathematics/Transform.h>

WireMeshWindow3::WireMeshWindow3(Parameters& parameters)
    :
    MouseMoveWindow3(parameters),
    mApplicationTime(0.0),
    mApplicationDeltaTime(0.001)
{

    if (!SetEnvironment() || !CreateScene())
    {
        parameters.created = false;
        return;
    }

    // Graphics engine state.
    mEngine->SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f});

    InitializeCamera(60.0f, GetAspectRatio(), 0.1f, 100.0f, 0.01f, 0.001f,
        { 0.0f, 0.0f, 2.5f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });
    mPVWMatrices.Update();
}

void WireMeshWindow3::OnIdle()
{
    mTimer.Measure();

    mFreeMouseCameraRig.Move();
    mPVWMatrices.Update();    

    mCuller.ComputeVisibleSet(mCamera, mScene);

    mEngine->ClearBuffers();

    for (auto const& visual : mCuller.GetVisibleSet())
    {
      mEngine->Draw(visual);
    }

    mEngine->Draw(8, mYSize - 8, { 1.0f, 1.0f, 1.0f, 1.0 }, mTimer.GetFPS());
    mEngine->DisplayColorBuffer(0);

    mTimer.UpdateFrameCount();
}

bool WireMeshWindow3::OnResize(int xSize, int ySize)
{
    if (MouseMoveWindow3::OnResize(xSize, ySize))
    {
        mCuller.ComputeVisibleSet(mCamera, mScene);
    }
    return true;
}

bool WireMeshWindow3::SetEnvironment()
{
    std::string path = GetGTEPath();
    if (path == "")
    {
        return false;
    }

    mEnvironment.Insert(path + "/Samples/Graphics/WireMesh/Shaders/");

    std::vector<std::string> inputs =
    {
        mEngine->GetShaderName("WireMesh.vs"),
        mEngine->GetShaderName("WireMesh.ps"),
        mEngine->GetShaderName("WireMesh.gs")
    };

    for (auto const& input : inputs)
    {
        if (mEnvironment.GetPath(input) == "")
        {
            LogError("Cannot find file " + input);
            return false;
        }
    }

    return true;
}

bool WireMeshWindow3::CreateScene()
{
    mScene = std::make_shared<Node>();

    std::string vsPath = mEnvironment.GetPath(mEngine->GetShaderName("WireMesh.vs"));
    std::string psPath = mEnvironment.GetPath(mEngine->GetShaderName("WireMesh.ps"));
    std::string gsPath = mEnvironment.GetPath(mEngine->GetShaderName("WireMesh.gs"));

    auto program = mProgramFactory->CreateFromFiles(vsPath, psPath, gsPath);
    if (!program)
    {
        return false;
    }

    auto parameters = std::make_shared<ConstantBuffer>(3 * sizeof(Vector4<float>), false);
    auto* data = parameters->Get<Vector4<float>>();
    data[0] = { 0.0f, 0.0f, 1.0f, 1.0f };  // mesh color
    data[1] = { 0.0f, 0.0f, 0.0f, 1.0f };  // edge color
    data[2] = { static_cast<float>(mXSize), static_cast<float>(mYSize), 0.0f, 0.0f };
    program->GetVertexShader()->Set("WireParameters", parameters);
    program->GetPixelShader()->Set("WireParameters", parameters);
    program->GetGeometryShader()->Set("WireParameters", parameters);

    auto cbuffer = std::make_shared<ConstantBuffer>(sizeof(Matrix4x4<float>), true);
    program->GetVertexShader()->Set("PVWMatrix", cbuffer);

    auto effect = std::make_shared<VisualEffect>(program);

    VertexFormat vformat;
    vformat.Bind(VA_POSITION, DF_R32G32B32_FLOAT, 0);
    MeshFactory mf;
    mf.SetVertexFormat(vformat);

    std::shared_ptr<Visual> mMesh = mf.CreateSphere(16, 16, 1.0f);
    mMesh->localTransform.SetTranslation(0.0, 0.0, 0.0);
    mMesh->SetEffect(effect);

    mPVWMatrices.Subscribe(mMesh->worldTransform, cbuffer);

    mScene->AttachChild(mMesh);

    mScene->Update();


    return true;
}
