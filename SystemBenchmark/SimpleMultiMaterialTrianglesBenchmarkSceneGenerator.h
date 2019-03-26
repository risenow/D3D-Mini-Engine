#pragma once

class SimpleMultiMaterialTrianglesBenchmarkSceneGenerator
{
public:
	SimpleMultiMaterialTrianglesBenchmarkSceneGenerator(unsigned long trianglesNum = 100000) : m_TrianglesNum(trianglesNum)
	{}
	void Generate();
private:
	unsigned long m_TrianglesNum;
};