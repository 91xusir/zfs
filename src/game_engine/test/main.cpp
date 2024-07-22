#include "core/rt2_core.h"
#include "graph/rtg_graph.h"
using namespace rt_graph;
void _AdjustCameraMatrix(RtgMatrix16 *matOut, RtgMatrix12 *matIn)
{
	RtgVertex3 x = *(RtgVertex3 *)&matIn->_00;
	x.Normalize();
	RtgVertex3 y = *(RtgVertex3 *)&matIn->_10;
	x.Normalize();
	RtgVertex3 z = *(RtgVertex3 *)&matIn->_20;
	y.Normalize();
	matOut->Identity();
	// 设置方向向量
	matOut->SetRow(0, x);
	matOut->SetRow(1, y);
	matOut->SetRow(2, z);
	// 设置位置
	matOut->SetRow(3, (float *)&matIn->_30);
}
int main()
{
	RtgMatrix12 viewMatrix = {
		-0.996642, 0.081885, 0.000000,
		0.013642, 0.166036, 0.986025,
		0.080741, 0.982714, -0.166596,
		400.431702, 5561.816895, -582.978577};
	RtgMatrix16 viewMatrix2 ;
	_AdjustCameraMatrix(&viewMatrix2, (RtgMatrix12 *)&viewMatrix);
	std::cout << viewMatrix2.ToString() << std::endl;
	return 0;
}