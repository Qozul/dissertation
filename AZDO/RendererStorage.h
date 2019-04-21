#pragma once
#include "../Shared/Utility.h"
#include "DrawElementsCommand.h"
#include "Texture.h"
#include "Mesh.h"

namespace QZL
{
	namespace AZDO
	{
		class VaoWrapper;

		struct TextureDataWrapper {
			TextureData data0;
			TextureData data1;
		};

		// This class should always be dynamically allocated because it can contain a lot of raw data.
		// StorageType gives safety: only instance or textured instance should be used, never mixed,
		// the type allows the class to check that the correct methods are being used.
		// 
		// Also every mesh must be in the vao given on construction, this class does not own the vao
		class RenderStorage {
		public:
			RenderStorage(VaoWrapper* vao) : vao_(vao) {}
			
			// Adding a mesh by pointer (from a VaoWrapper) extracts the data and adds to storage
			// This means the data is duplicated in the program, but the small memory cost allows
			// the renderer to directly copy the data in to its buffers with a single memcpy.
			void addMesh(const std::string& name, BasicMesh* mesh);
			void addInstance(const std::string& name, MeshInstance* instance);
			void addInstance(const std::string& name, TexturedMeshInstance* instance);

			void modifyInstance(const std::string& name, const size_t instanceIndex, MeshInstance* instance);
			void modifyInstance(const std::string& name, const size_t instanceIndex, TexturedMeshInstance* instance);

			DrawElementsCommand* meshData();
			MeshInstance* instanceData();
			TextureDataWrapper* textureData();
			size_t meshCount();
			size_t instanceCount();
			size_t textureCount();

			VaoWrapper* vao();

		private:
			VaoWrapper* vao_;
			std::unordered_map<std::string, size_t> dataMap_;

			std::vector<DrawElementsCommand> meshes_;
			// instances_ and textureData_ should always be an equal size IF textures are being used
			std::vector<MeshInstance> instances_;
			std::vector<TextureDataWrapper> textureData_;
		};
	}
}
