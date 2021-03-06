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

#include <extensions/Building/warehouse_manager.h>

namespace sfge::ext
{
	WarehouseManager::WarehouseManager(Engine& engine) : System(engine){}

	void WarehouseManager::Init()
	{
		m_EntityManager = m_Engine.GetEntityManager();
		m_Transform2DManager = m_Engine.GetTransform2dManager();
		m_TextureManager = m_Engine.GetGraphics2dManager()->GetTextureManager();
		m_SpriteManager = m_Engine.GetGraphics2dManager()->GetSpriteManager();
		m_Configuration = m_Engine.GetConfig();

		m_Window = m_Engine.GetGraphics2dManager()->GetWindow();

		//Load Texture
		m_TexturePath = "data/sprites/warehouse.png";
		m_TextureId = m_TextureManager->LoadTexture(m_TexturePath);
		m_Texture = m_TextureManager->GetTexture(m_TextureId);

		m_Init = true;

		Log::GetInstance()->Msg("Warehouse Manager initialized");
	}

	void WarehouseManager::Update(float dt)
	{
		if (!m_Init)
		{
			Init();
		}
	}

	void WarehouseManager::FixedUpdate()
	{
	}

	void WarehouseManager::Draw()
	{
	}

	void WarehouseManager::SpawnBuilding(Vec2f position)
	{

		Entity newEntity = m_EntityManager->CreateEntity(INVALID_ENTITY);

		if (newEntity == INVALID_ENTITY)
		{
			m_EntityManager->ResizeEntityNmb(m_Configuration->currentEntitiesNmb + CONTAINER_RESERVATION);
			newEntity = m_EntityManager->CreateEntity(INVALID_ENTITY);
		}

		//add transform
		Transform2d* transformPtr = m_Transform2DManager->AddComponent(newEntity);
		transformPtr->Position = Vec2f(position.x, position.y);

		SetupTexture(newEntity);

		editor::EntityInfo& entityInfo = m_EntityManager->GetEntityInfo(newEntity);
		entityInfo.name = "Warehouse " + std::to_string(m_BuildingIndexCount);

		if (CheckFreeSlot(newEntity))
		{
			return;
		}

		m_BuildingIndexCount++;



		if (m_BuildingIndexCount >= CONTAINER_RESERVATION * m_NmbReservation)
		{
			ReserveContainer(m_BuildingIndexCount + CONTAINER_RESERVATION);
			m_NmbReservation++;
		}

		AttributeContainer();

		const size_t newWarehouse = m_BuildingIndexCount - 1;

		m_EntityIndex[newWarehouse] = newEntity;

	}

	void WarehouseManager::DestroyBuilding(Entity entity)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_EntityIndex[i] == entity)
			{
				m_EntityIndex[i] = INVALID_ENTITY;
				m_EntityManager->DestroyEntity(entity);
				return;
			}
		}
	}

	void WarehouseManager::AttributeDwarfToBuilding(Entity entity)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_EntityIndex[i] == entity)
			{
				m_DwarfSlots[i].dwarfAttributed++;
				return;
			}
		}
	}

	void WarehouseManager::DeallocateDwarfToBuilding(Entity entity)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_EntityIndex[i] == entity)
			{
				m_DwarfSlots[i].dwarfAttributed--;
				return;
			}
		}
	}

	void WarehouseManager::DwarfEnterBuilding(Entity entity)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_EntityIndex[i] == entity)
			{
				m_DwarfSlots[i].dwarfIn++;
				return;
			}
		}
	}

	void WarehouseManager::DwarfExitBuilding(Entity entity)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_EntityIndex[i] == entity)
			{
				m_DwarfSlots[i].dwarfIn++;
				return;
			}
		}
	}

	Entity WarehouseManager::GetBuildingWithFreePlace()
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_DwarfSlots[i].dwarfAttributed < m_DwarfSlots[i].maxDwarfCapacity)
			{
				return m_EntityIndex[i];
			}
		}
		return INVALID_ENTITY;
	}

	Entity WarehouseManager::GetWarehouseWithResources(ResourceType resourceType)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_EntityIndex[i] == INVALID_ENTITY)
			{
				continue;
			}

			short currentinventory;

			switch (resourceType)
			{
			case ResourceType::IRON:
				currentinventory = m_IronInventories[i] - m_ReservedExportStackNumberIron[i] * GetStackSizeByResourceType(resourceType);

				if (GetStackSizeByResourceType(resourceType) <= currentinventory)
				{
					return m_EntityIndex[i];
				}
				break;

			case ResourceType::FOOD:
				currentinventory = m_MushroomInventories[i] - m_ReservedExportStackNumberMushroom[i] * GetStackSizeByResourceType(resourceType);

				if (GetStackSizeByResourceType(resourceType) <= currentinventory)
				{
					return m_EntityIndex[i];
				}
				break;


			case ResourceType::STONE:
				currentinventory = m_StoneInventories[i] - m_ReservedExportStackNumberStone[i] * GetStackSizeByResourceType(resourceType);

				if (GetStackSizeByResourceType(resourceType) <= currentinventory)
				{
					return m_EntityIndex[i];
				}
				break;


			case ResourceType::TOOL:
				currentinventory = m_ToolInventories[i] - m_ReservedExportStackNumberTool[i] * GetStackSizeByResourceType(resourceType);

				if (GetStackSizeByResourceType(resourceType) <= currentinventory)
				{
					return m_EntityIndex[i];
				}
				break;
			}
		}
		return INVALID_ENTITY;
	}

	Entity WarehouseManager::GetWarehouseWithFreeSpaceAvailable(ResourceType resourceType)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_EntityIndex[i] == INVALID_ENTITY)
			{
				continue;
			}

			unsigned short currentinventory;

			switch (resourceType)
			{
			case ResourceType::IRON:
				currentinventory = m_IronInventories[i] + m_ReservedImportStackNumberIron[i] * GetStackSizeByResourceType(resourceType);

				if(GetStackSizeByResourceType(resourceType) <= m_MaxCapacity - currentinventory)
				{
					return m_EntityIndex[i];
				}
				break;

			case ResourceType::FOOD:
				currentinventory = m_MushroomInventories[i] + m_ReservedImportStackNumberMushroom[i] * GetStackSizeByResourceType(resourceType);

				if (GetStackSizeByResourceType(resourceType) <= m_MaxCapacity - currentinventory)
				{
					return m_EntityIndex[i];
				}
				break;


			case ResourceType::STONE:
				currentinventory = m_StoneInventories[i] + m_ReservedImportStackNumberStone[i] * GetStackSizeByResourceType(resourceType);

				if (GetStackSizeByResourceType(resourceType) <= m_MaxCapacity - currentinventory)
				{
					return m_EntityIndex[i];
				}
				break;


			case ResourceType::TOOL:
				currentinventory = m_ToolInventories[i] + m_ReservedImportStackNumberTool[i] * GetStackSizeByResourceType(resourceType);

				if (GetStackSizeByResourceType(resourceType) <= m_MaxCapacity - currentinventory)
				{
					return m_EntityIndex[i];
				}
				break;
			}
		}
		return INVALID_ENTITY;
	}

	void WarehouseManager::DwarfTakesResources(Entity entity, ResourceType resourceType)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if(m_EntityIndex[i] == entity)
			{
				switch(resourceType)
				{
				case ResourceType::IRON:
					m_IronInventories[i] -= GetStackSizeByResourceType(resourceType);
					m_ReservedExportStackNumberIron[i]--;
					break;

				case ResourceType::FOOD:
					m_MushroomInventories[i] -= GetStackSizeByResourceType(resourceType);
					m_ReservedExportStackNumberMushroom[i]--;
					break;

				case ResourceType::STONE:
					m_StoneInventories[i] -= GetStackSizeByResourceType(resourceType);
					m_ReservedExportStackNumberStone[i]--;
					break;

				case ResourceType::TOOL:
					m_ToolInventories[i] -= GetStackSizeByResourceType(resourceType);
					m_ReservedExportStackNumberTool[i]--;
					break;
				}
			}
		}
	}

	void WarehouseManager::DwarfPutsResources(Entity entity, ResourceType resourceType)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_EntityIndex[i] == entity)
			{
				switch (resourceType)
				{
				case ResourceType::IRON:
					m_IronInventories[i] += GetStackSizeByResourceType(resourceType);
					m_ReservedImportStackNumberIron[i]--;
					break;

				case ResourceType::FOOD:
					m_MushroomInventories[i] += GetStackSizeByResourceType(resourceType);
					m_ReservedImportStackNumberMushroom[i]--;
					break;

				case ResourceType::STONE:
					m_StoneInventories[i] += GetStackSizeByResourceType(resourceType);
					m_ReservedImportStackNumberStone[i]--;
					break;

				case ResourceType::TOOL:
					m_ToolInventories[i] += GetStackSizeByResourceType(resourceType);
					m_ReservedImportStackNumberTool[i]--;
					break;
				}
			}
		}
	}

	void WarehouseManager::ReserveForFill(Entity entity, ResourceType resourceType)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_EntityIndex[i] == entity)
			{
				switch (resourceType)
				{
				case ResourceType::IRON:
					m_ReservedImportStackNumberIron[i]++;
					break;

				case ResourceType::FOOD:
					m_ReservedImportStackNumberMushroom[i]++;
					break;

				case ResourceType::STONE:
					m_ReservedImportStackNumberStone[i]++;
					break;

				case ResourceType::TOOL:
					m_ReservedImportStackNumberTool[i]++;
					break;
				}
			}
		}
	}

	void WarehouseManager::ReserveForEmpty(Entity entity, ResourceType resourceType)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_EntityIndex[i] == entity)
			{
				switch (resourceType)
				{
				case ResourceType::IRON:
					m_ReservedExportStackNumberIron[i]++;
					break;

				case ResourceType::FOOD:
					m_ReservedExportStackNumberMushroom[i]++;
					break;

				case ResourceType::STONE:
					m_ReservedExportStackNumberStone[i]++;
					break;

				case ResourceType::TOOL:
					m_ReservedExportStackNumberTool[i]++;
					break;
				}
			}
		}
	}

	bool WarehouseManager::CheckFreeSlot(Entity newEntity)
	{
		for (int i = 0; i < m_BuildingIndexCount; i++)
		{
			if (m_EntityIndex[i] != INVALID_ENTITY)
			{
				continue;
			}

			m_EntityIndex[i] = newEntity;

			m_DwarfSlots[i] = DwarfSlots();

			m_IronInventories[i] = 0;
			m_StoneInventories[i] = 0;
			m_ToolInventories[i] = 0;
			m_MushroomInventories[i] = 0;

			m_ReservedExportStackNumberIron[i] = 0;
			m_ReservedExportStackNumberStone[i] = 0;
			m_ReservedExportStackNumberTool[i] = 0;
			m_ReservedExportStackNumberMushroom[i] = 0;

			m_ReservedImportStackNumberIron[i] = 0;
			m_ReservedImportStackNumberStone[i] = 0;
			m_ReservedImportStackNumberTool[i] = 0;
			m_ReservedImportStackNumberMushroom[i] = 0;

			return true;
		}
		return false;
	}

	void WarehouseManager::SetupTexture(const Entity entity)
	{
		//Sprite Component part
		Sprite* sprite = m_SpriteManager->AddComponent(entity);
		sprite->SetTexture(m_Texture);

		auto& spriteInfo = m_SpriteManager->GetComponentInfo(entity);
		spriteInfo.name = "Sprite";
		spriteInfo.sprite = sprite;
		spriteInfo.textureId = m_TextureId;
		spriteInfo.texturePath = m_TexturePath;
	}

	void WarehouseManager::ReserveContainer(const size_t newSize)
	{
		m_EntityIndex.reserve(newSize);
		m_DwarfSlots.reserve(newSize);

		m_IronInventories.reserve(newSize);
		m_StoneInventories.reserve(newSize);
		m_ToolInventories.reserve(newSize);
		m_MushroomInventories.reserve(newSize);

		m_ReservedExportStackNumberIron.reserve(newSize);
		m_ReservedExportStackNumberStone.reserve(newSize);
		m_ReservedExportStackNumberTool.reserve(newSize);
		m_ReservedExportStackNumberMushroom.reserve(newSize);

		m_ReservedImportStackNumberIron.reserve(newSize);
		m_ReservedImportStackNumberStone.reserve(newSize);
		m_ReservedImportStackNumberTool.reserve(newSize);
		m_ReservedImportStackNumberMushroom.reserve(newSize);
	}

	void WarehouseManager::AttributeContainer()
	{
		m_EntityIndex.emplace_back(INVALID_ENTITY);
		m_DwarfSlots.emplace_back(DwarfSlots());

		m_IronInventories.emplace_back(0);
		m_StoneInventories.emplace_back(0);
		m_ToolInventories.emplace_back(0);
		m_MushroomInventories.emplace_back(0);

		m_ReservedExportStackNumberIron.emplace_back(0);
		m_ReservedExportStackNumberStone.emplace_back(0);
		m_ReservedExportStackNumberTool.emplace_back(0);
		m_ReservedExportStackNumberMushroom.emplace_back(0);

		m_ReservedImportStackNumberIron.emplace_back(0);
		m_ReservedImportStackNumberStone.emplace_back(0);
		m_ReservedImportStackNumberTool.emplace_back(0);
		m_ReservedImportStackNumberMushroom.emplace_back(0);
	}
}