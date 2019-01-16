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
			float getRotationAngle() const { return angle_; };
			glm::vec3 getScale() const { return scale_; };
			void setPosition(const glm::vec3& newPos) { position_ = newPos; };
			void setRotation(const glm::vec3& newRot) { rotation_ = newRot; };
			void setRotationAngle(const float newAng) { angle_ = newAng; };
			void setScale(const glm::vec3& newScale) { scale_ = newScale; };
			void setScale(const float newScale) { 
				scale_.x = newScale;
				scale_.y = newScale;
				scale_.z = newScale;
			};

			glm::mat4 toModelMatrix() {
				glm::mat4 model(1.0f);
				model = glm::translate(model, position_);
				model = glm::rotate(model, angle_, rotation_);
				model = glm::scale(model, scale_);
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
