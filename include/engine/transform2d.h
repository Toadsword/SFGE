/*
 * transform.h
 *
 *  Created on: Feb 6, 2018
 *      Author: efarhan
 */

#ifndef SFGE_TRANSFORM_H_
#define SFGE_TRANSFORM_H_

#include <engine/entity.h>
#include <engine/component.h>
#include <SFML/System/Vector2.hpp>

namespace sfge
{


struct Transform2d
{
	sf::Vector2f Position = sf::Vector2f();
	sf::Vector2f Scale = sf::Vector2f(1.0f,1.0f);
	float EulerAngle = 0.0f;

};

namespace editor
{
struct Transform2dInfo : ComponentInfo
{
	void DrawOnInspector() override;
	Transform2d* transform = nullptr;
};
}

class Transform2dManager :
	public ComponentManager<Transform2d, editor::Transform2dInfo>, public System, public ResizeObserver
{
public:
	Transform2dManager(Engine& engine);
	Transform2d* AddComponent(Entity entity) override;
	void CreateComponent(json& componentJson, Entity entity) override;
	void DestroyComponent(Entity entity) override;
	void OnResize(size_t new_size) override;
};

class TransformRequiredComponent
{
public:
	TransformRequiredComponent(Transform2d* transform);
	void SetTransform(Transform2d* transform);
	Transform2d* GetTransform();
protected:
	Transform2d * m_Transform = nullptr;
};
}

#endif /* INCLUDE_ENGINE_TRANSFORM_H_ */