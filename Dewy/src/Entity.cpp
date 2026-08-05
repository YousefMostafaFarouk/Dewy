#include "Entity.h"
#include "ConnectionComponent.h"

Entity::~Entity()
{
	for (auto& component : m_components)
		delete component;

	std::cout << "deleted\n";
}
