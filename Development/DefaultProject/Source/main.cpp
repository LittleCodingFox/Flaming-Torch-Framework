#include "FlamingCore.hpp"
#include "DefaultProject.hpp"

using namespace FlamingTorch;

int main(int argc, char **argv)
{
	DisposablePointer<DefaultProject> InterfaceInstance(new DefaultProject());

	g_Game = InterfaceInstance;

	return g_Game->Run(argc, argv);
};
