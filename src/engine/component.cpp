/*
 MIT License
 
 Copyright (c) 2017 SAE Institute Switzerland AG
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <engine/component.h>
#include <engine/transform.h>
#include <graphics/sprite.h>
#include <graphics/shape.h>
#include <python/pycomponent.h>

namespace sfge
{

Component::Component(GameObject& parentObject) :
	gameObject(parentObject)
{

}

std::shared_ptr<Component> Component::LoadComponent(Engine& engine, json& componentJson, GameObject& gameObject)
{
	std::shared_ptr<Component> component = nullptr;
	if (CheckJsonParameter(componentJson, "type", json::value_t::number_integer))
	{
		ComponentType componentType = (ComponentType)componentJson["type"];

		switch(componentType)
		{
		case ComponentType::TRANSFORM:
			component = Transform::LoadTransform(componentJson, gameObject);
			gameObject.SetTransform(std::dynamic_pointer_cast<Transform>(component));
			break;
		case ComponentType::SPRITE:
			component = Sprite::LoadSprite(engine, componentJson, gameObject);
			break;
		case ComponentType::PYCOMPONENT:
			component = PyComponent::LoadPythonScript(engine, componentJson, gameObject);
			break;
		case ComponentType::SHAPE:
			component = Shape::LoadShape(engine, componentJson, gameObject);
		}
		if (component != nullptr)
		{
			gameObject.m_Components.push_back(component);
		}
	}
	return component;
}


}
