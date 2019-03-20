#pragma once
#include "../Shared/Utility.h"

namespace QZL
{
	namespace AZDO
	{
		struct MeshInstance;
		struct TexturedMeshInstance;
		struct BasicMesh;
		struct TextureData;

		// This class should always be dynamically allocated because it can contain a lot of raw data.
		// StorageType gives safety: only instance or textured instance should be used, never mixed,
		// the type allows the class to check that the correct methods are being used.
		class RenderStorage {
		public:
			enum class StorageType {
				kBasic, kTextured
			};

			RenderStorage(StorageType type) : type_(type) {}
			
			// Adding a mesh by pointer (from a VaoWrapper) extracts the data and adds to storage
			// This means the data is duplicated in the program, but the small memory cost allows
			// the renderer to directly copy the data in to its buffers with a single memcpy.
			void addMesh(BasicMesh* mesh);
			void addInstance(MeshInstance* instance);
			void addTexturedInstance(TexturedMeshInstance* instance);
			BasicMesh* meshData() {
				return meshes_.data();
			}
			MeshInstance* instanceData() {
				return instances_.data();
			}
			TextureData* textureData() {
				return textureData_.data();
			}

		private:
			StorageType type_;
			std::vector<BasicMesh> meshes_;
			std::vector<MeshInstance> instances_;
			std::vector<TextureData> textureData_;
		};
	}
}
