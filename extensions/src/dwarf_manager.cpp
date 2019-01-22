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

#include <extensions/dwarf_manager.h>
#include <python/python_engine.h>

namespace sfge::ext
{
DwarfManager::DwarfManager(sfge::Engine& engine) :
	System(engine) {

}

void DwarfManager::Init() {
	//Get managers
	m_Transform2DManager = m_Engine.GetTransform2dManager();
	m_TextureManager = m_Engine.GetGraphics2dManager()->GetTextureManager();
	m_SpriteManager = m_Engine.GetGraphics2dManager()->GetSpriteManager();
	m_NavigationGraphManager = m_Engine.GetPythonEngine()->GetPySystemManager().GetPySystem<NavigationGraphManager>(
		"NavigationGraphManager");

	//Read config
	const auto config = m_Engine.GetConfig();
	m_FixedDeltaTime = m_Engine.GetConfig()->fixedDeltaTime;

	auto* entityManager = m_Engine.GetEntityManager();

	//Load texture
	m_TexturePath = "data/sprites/triangle.png";
	m_TextureId = m_TextureManager->LoadTexture(m_TexturePath);
	m_Texture = m_TextureManager->GetTexture(m_TextureId);

#ifdef DEBUG_SPAWN_DWARF
	const Vec2f screenSize = sf::Vector2f(config->screenResolution.x, config->screenResolution.y);

	entityManager->ResizeEntityNmb(config->currentEntitiesNmb + m_DwarfToSpawn);

	//Create dwarfs
	for (auto i = 0u; i < m_DwarfToSpawn; i++) {
		const Vec2f pos(std::rand() % static_cast<int>(screenSize.x), std::rand() % static_cast<int>(screenSize.y));

		SpawnDwarf(pos);
	}
#endif
}

void DwarfManager::SpawnDwarf(const Vec2f pos) {
	auto* entityManager = m_Engine.GetEntityManager();
	const auto newEntity = entityManager->CreateEntity(0);

	//Check if vectors are big enough, otherwise resize them
	if (m_DwarfsEntitiesIndex.size() < m_IndexNewDwarf + 1)
		ResizeContainers(m_DwarfsEntitiesIndex.size() + m_ContainersExtender);

	//Update std::vectors
	m_DwarfsEntitiesIndex[m_IndexNewDwarf] = newEntity;
	m_States[m_IndexNewDwarf] = State::IDLE;
	m_Paths[m_IndexNewDwarf] = std::vector<Vec2f>();

	m_IndexNewDwarf++;

	//Add transform
	auto transformPtr = m_Transform2DManager->AddComponent(newEntity);
	transformPtr->Position = pos;

	//Add sprite
	auto sprite = m_SpriteManager->AddComponent(newEntity);
	sprite->SetTexture(m_Texture);

	//Set sprite infos
	auto& spriteInfo = m_SpriteManager->GetComponentInfo(newEntity);
	spriteInfo.name = "Sprite";
	spriteInfo.sprite = sprite;
	spriteInfo.textureId = m_TextureId;
	spriteInfo.texturePath = m_TexturePath;
}

void DwarfManager::ResizeContainers(const size_t newSize) {
	m_DwarfsEntitiesIndex.resize(newSize);
	m_Paths.resize(newSize);
	m_States.resize(newSize);
}

void DwarfManager::Update(float dt) {
#ifdef DEBUG_RANDOM_PATH
	const auto config = m_Engine.GetConfig();
	const Vec2f screenSize = sf::Vector2f(config->screenResolution.x, config->screenResolution.y);
#endif
	
	for (auto i = 0u; i < m_IndexNewDwarf; i++) {
		switch (m_States[i]) { 
		case IDLE: {
#ifdef DEBUG_RANDOM_PATH
			const auto transformPtr = m_Engine.GetTransform2dManager()->GetComponentPtr(m_DwarfsEntitiesIndex[i]);
			m_NavigationGraphManager->AskForPath(&m_Paths[i], transformPtr->Position,
				Vec2f(std::rand() % static_cast<int>(screenSize.x),
					std::rand() % static_cast<int>(screenSize.y
						)));
			m_States[i] = State::WAITING_NEW_PATH;
#endif
			break;
		}

			case WALKING: 
			break;

			case WAITING_NEW_PATH: 
				if (!m_Paths[i].empty()) {
					m_States[i] = State::WALKING;
				}
			break;
			default: ;
		}
	}
}

void DwarfManager::FixedUpdate() {
	for (auto i = 0u; i < m_IndexNewDwarf; i++) {
		switch (m_States[i]) { 
			case IDLE: break;
			case WALKING: {
				auto transformPtr = m_Engine.GetTransform2dManager()->GetComponentPtr(m_DwarfsEntitiesIndex[i]);

				auto dir = m_Paths[i][0] - transformPtr->Position;

				if (dir.GetMagnitude() < m_StoppingDistance) {
					std::reverse(m_Paths[i].begin(), m_Paths[i].end());
					m_Paths[i].pop_back();
					std::reverse(m_Paths[i].begin(), m_Paths[i].end());

					if (m_Paths[i].empty()) {
						m_States[i] = State::IDLE;
					}
				}
				else {
					//TODO ajouter un manager pour les velocit�s. L'id�es est de cr�er un syst�me qui ne comporte que �a comme donn�es et fait un traitement uniquement dessus. Il faut rajouter des fonctions comme AddComponent() en lui passant directement l'entit�
					transformPtr->Position += dir.Normalized() * m_SpeedDwarf;
				}
				break;
			}
			case WAITING_NEW_PATH: break;
			default: ;
		}
	}
}

void DwarfManager::Draw() {
#ifdef DEBUG_DRAW_PATH
	auto window = m_Engine.GetGraphics2dManager()->GetWindow();

	for (auto i = 0u; i < m_IndexNewDwarf; i++) {
		const auto color = m_Colors[i % m_Colors.size()];

		sf::VertexArray lines{sf::LineStrip, m_Paths[i].size()};
		for (size_t j = 0u; j < m_Paths[i].size(); j++) {
			lines[j].position = m_Paths[i][j];

			lines[j].color = color;
		}

		window->draw(lines);
	}
#endif
}
}
