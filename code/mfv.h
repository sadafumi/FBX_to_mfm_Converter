#pragma once

#include <string>
#include <vector>

#define GLM_FORCE_PURE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "glm/glm.hpp"

namespace mfv 
{
	class model
	{
	public:
		enum Stride
		{
			Str_vertex,
			Str_color,
			Str_uv,
			Str_normal,
			Str_weight,
			STRIDE_MAX
		};
		struct Vertex
		{
			float Pos_x;
			float Pos_y;
			float Pos_z;
			float Pos_w;

			float Normal_x;
			float Normal_y;
			float Normal_z;
		
			float U;
			float V;
		};

	private:
		struct Weight
		{
			std::vector<float> weight;
			std::vector<unsigned int> index;
		};
		struct Animation 
		{
			glm::mat4 init;
			std::vector<glm::mat4> perFrame;
		};
		struct Bone
		{
			std::vector<Weight> weights;
			std::vector<Animation> index;
		};
		struct material 
		{
			std::string name;
		};
		struct mash
		{
			int vertex_size;
			std::vector<float> vertexs;
			std::vector<Vertex> vertex_arry;
			material tex_file;
			std::vector<Bone> Bones;
		};
	public:
		int stride_size;
		std::vector<Stride> strides;
		std::vector<mash> meshs;
		bool Save_File(std::string file_name);
		bool Load_File(std::string file_name);
	};
}

