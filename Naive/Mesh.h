#pragma once
#include "../Shared/Transform.h"

namespace QZL
{
	namespace Naive
	{
		class Texture;

		/// BasicMesh needs to provide a transform and suitable access to its own buffer data
		struct BasicMesh {
			GLuint vaoId;
			GLsizei indexCount;
			Shared::Transform transform;
		};
		/// Only difference is it now stores a texture
		struct TexturedBasicMesh {
			GLuint vaoId;
			GLsizei indexCount;
			Texture* texture;
			Texture* texture2;
			Shared::Transform transform;
		};

		inline TexturedBasicMesh* basicToTextured(BasicMesh* bmesh, Texture* texture, Texture* texture2) {
			Naive::TexturedBasicMesh* tbm = new Naive::TexturedBasicMesh();
			tbm->vaoId = bmesh->vaoId;
			tbm->indexCount = bmesh->indexCount;
			tbm->transform = bmesh->transform;
			tbm->texture = texture;
			tbm->texture2 = texture2;

			SAFE_DELETE(bmesh);
			return tbm;
		}
	}
}
