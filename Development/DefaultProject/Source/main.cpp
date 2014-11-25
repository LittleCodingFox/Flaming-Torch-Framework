#include "FlamingCore.hpp"
#include "DefaultProject.hpp"

using namespace FlamingTorch;

int main(int argc, char **argv)
{
	DisposablePointer<DefaultProject> InterfaceInstance(new DefaultProject());

	GameInterface::SetInstance(InterfaceInstance);

	return GameInterface::Instance->Run(argc, argv);
};
