#include "gtest.h"
#include <Applications/LogReporter.h>
#include <Graphics/MeshFactory.h>
#include <gtest/gtest.h>

#include "MyWindow.h"

#define SPHERE_COUNT 10
#define TEST_CULL 1


std::vector<std::shared_ptr<gte::Visual*>>* GetVisualsOfScene(const std::shared_ptr<gte::Node>& node)
{
	// TODO: try linked list because of the append function and might be faster than std::vector
	std::vector<std::shared_ptr<gte::Visual*>>* visuals = new std::vector<std::shared_ptr<gte::Visual*>>();

	for (int i = 0; i < node->GetNumChildren(); i++)
	{
		auto child = node->GetChild(i);
		if (auto v = dynamic_cast<gte::Visual*>(child.get()))
		{
			std::shared_ptr<gte::Visual*> vp = std::make_shared<gte::Visual*>(v);
			visuals->push_back(vp);
		} else if (auto n = dynamic_cast<gte::Node*>(child.get()))
		{
			std::shared_ptr<gte::Node> np = std::make_shared<gte::Node>(*n);
			std::vector<std::shared_ptr<gte::Visual*>>* t = GetVisualsOfScene(np);
			visuals->insert(visuals->end(), t->begin(), t->end());
		}
	}

	return visuals;
}

gtest::gtest(Parameters& parameters) : Window3(parameters)
{
	if (!SetEnvironment() || !CreateScene())
	{
		parameters.created = false;
		return;
	}

	// Graphics engine state.
	mEngine->SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	InitializeCamera(60.0f, GetAspectRatio(), 0.1f, 100.0f, 0.01f, 0.001f,
		{ 0.0f, 0.0f, -2.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f });
	mPVWMatrices.Update();

	mCuller.ComputeVisibleSet(mCamera, mScene);
}

void gtest::OnIdle()
{
	mTimer.Measure();

	if (mCameraRig.Move())
	{
		mPVWMatrices.Update();
#if (TEST_CULL == 0)
		mCuller.ComputeVisibleSet(mCamera, mScene);			// Built in cull
		//CullScene();
	}
	mEngine->ClearBuffers();

	for (auto const& visual : mCuller.GetVisibleSet())
	{
		mEngine->Draw(visual);
	}
#else
}
	mEngine->ClearBuffers();
	std::vector<std::shared_ptr<gte::Visual*>>* test = GetVisualsOfScene(mScene);

	for (auto const& visual : *test)
	{
		mEngine->Draw(*visual.get());
	}
#endif


	mEngine->Draw(8, mYSize - 8, { 1.0f, 1.0f, 1.0f, 1.0 }, mTimer.GetFPS());
	mEngine->DisplayColorBuffer(0);

	mTimer.UpdateFrameCount();
}

bool gtest::OnResize(int xSize, int ySize)
{
	if (Window3::OnResize(xSize, ySize))
	{
		mCuller.ComputeVisibleSet(mCamera, mScene);
	}
	return true;
}

bool gtest::SetEnvironment()
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

bool gtest::CreateScene()
{
	mScene = std::make_shared<Node>();

	for (int i = 0; i < SPHERE_COUNT; i++)
	{
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
		std::shared_ptr<Visual> Spheres;

		Spheres = mf.CreateSphere(16, 16, 1.0f);
		Spheres->localTransform.SetTranslation(2 * i - SPHERE_COUNT + 1, 0.0, 10.0);
		Spheres->SetEffect(effect);

		mPVWMatrices.Subscribe(Spheres->worldTransform, cbuffer);

		mScene->AttachChild(Spheres);
	}

	for (int i = 0; i < SPHERE_COUNT; i++)
	{
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
		std::shared_ptr<Visual> Spheres;

		Spheres = mf.CreateSphere(16, 16, 1.0f);
		Spheres->localTransform.SetTranslation(2 * i - SPHERE_COUNT + 1, 0.0, -10.0);
		Spheres->SetEffect(effect);

		mPVWMatrices.Subscribe(Spheres->worldTransform, cbuffer);

		mScene->AttachChild(Spheres);
	}

	for (int i = 0; i < SPHERE_COUNT; i++)
	{
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
		std::shared_ptr<Visual> Spheres;

		Spheres = mf.CreateTorus(16, 16, 1.0f, 0.5f);
		Spheres->localTransform.SetTranslation(-SPHERE_COUNT + 1, 0.0, 9 - 2 * i);
		Spheres->SetEffect(effect);

		mPVWMatrices.Subscribe(Spheres->worldTransform, cbuffer);

		mScene->AttachChild(Spheres);
	}

	for (int i = 0; i < SPHERE_COUNT; i++)
	{
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
		std::shared_ptr<Visual> Spheres;

		Spheres = mf.CreateOctahedron();
		Spheres->localTransform.SetTranslation(SPHERE_COUNT, 0.0, 9 - 2 * i);
		Spheres->SetEffect(effect);

		mPVWMatrices.Subscribe(Spheres->worldTransform, cbuffer);

		mScene->AttachChild(Spheres);
	}

	mScene->Update();

	return true;
}

int main(int, char const*[])
{
	EXPECT_EQ(1, 2);
	InitWindow();


	return 0;
}