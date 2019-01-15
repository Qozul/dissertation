/// Author: Ralph Ridley
/// Date: 15/01/19
/// Purpose: Provide basic operations for transformations
#pragma once
#include "Utility.h"

namespace QZL
{
	namespace Shared
	{
		/// Each transform simply has a position, a rotation using axis-angle, and a scale.
		/// Storing only these rather than a matrix reduces memory footprint per transform.
		class Transform {
		public:
			Transform()
				: position_({ 0.0f, 0.0f, 0.0f }), rotation_({ 0.0f, 1.0f, 0.0f }),
				angle_(0.0f), scale_({ 0.0f, 1.0f, 0.0f }) {}
			Transform(glm::vec3& position, glm::vec3& rotation, float angle, glm::vec3& scale)
				: position_(position), rotation_(rotation), angle_(angle), scale_(scale) {}

			glm::vec3 getPosition() const { return position_; };
			glm::vec3 getRotation() const { return rotation_; };
			glm::vec3 getScale() const { return scale_; };

			glm::mat4 toModelMatrix() {
				glm::mat4 model;
				glm::translate(model, position_);
				glm::rotate(model, angle_, rotation_);
				glm::scale(model, scale_);
				return model; 
			}
		private:
			glm::vec3 rotation_; // axis-angle
			float angle_;
			glm::vec3 position_;
			glm::vec3 scale_;
		};
	}
}
