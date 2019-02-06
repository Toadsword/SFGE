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

#include <extensions/mushroom_farm_manager.h>

namespace sfge::ext
{
	MushroomFarmManager::MushroomFarmManager(Engine& engine) : System(engine){}

	void MushroomFarmManager::Init()
	{
	}

	void MushroomFarmManager::Update(float dt)
	{
	}

	void MushroomFarmManager::FixedUpdate()
	{
	}

	void MushroomFarmManager::AddNewBuilding(Vec2f position)
	{
	}

	bool MushroomFarmManager::DestroyBuilding(Entity mushroomFarmEntity)
	{
		return false;
	}

	bool MushroomFarmManager::AddDwarfToBuilding(Entity mushroomFarmEntity)
	{
		return false;
	}

	bool MushroomFarmManager::RemoveDwarfToBuilding(Entity mushroomFarmEntity)
	{
		return false;
	}

	void MushroomFarmManager::DwarfEnterBuilding(Entity mushroomFarmEntity)
	{
	}

	void MushroomFarmManager::DwarfExitBuilding(Entity mushroomFarmEntity)
	{
	}

	Entity MushroomFarmManager::GetFreeSlotInBuilding()
	{
		return Entity();
	}

}