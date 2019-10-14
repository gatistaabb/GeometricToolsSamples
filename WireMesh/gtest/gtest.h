#pragma once
#include <Applications/Window3.h>
using namespace gte;

class gtest : public Window3
{
public:
	gtest(Parameters& parameters);

	virtual void OnIdle() override;

	virtual bool OnResize(int xSize, int ySize) override;

private:
	Culler mCuller;

	bool SetEnvironment();
	bool CreateScene();
	void CullScene();

	std::shared_ptr<Node> mScene;
	std::shared_ptr<Visual*> culledScene;
};