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
#include <python/python_engine.h>
#include <utility>

#include <extensions/AI/behavior_tree_nodes_core.h>
#include <extensions/AI/behavior_tree.h>

namespace sfge::ext::behavior_tree
{
Node::Node(BehaviorTree* bt, ptr parentNode)
{
	m_BehaviorTree = bt;
	m_ParentNode = std::move(parentNode);
}

void Node::Execute(const unsigned int index)
{
	switch (nodeType)
	{
	case NodeType::SEQUENCE_COMPOSITE:
		SequenceComposite(index);
		break;
	case NodeType::SELECTOR_COMPOSITE:
		SelectorComposite(index);
		break;
	case NodeType::REPEATER_DECORATOR:
		RepeaterDecorator(index);
		break;
	case NodeType::REPEAT_UNTIL_FAIL_DECORATOR:
		RepeatUntilFailDecorator(index);
		break;
	case NodeType::INVERTER_DECORATOR:
		InverterDecorator(index);
		break;
	case NodeType::SUCCEEDER_DECORATOR:
		SucceederDecorator(index);
		break;
	case NodeType::WAIT_FOR_PATH_LEAF:
		WaitForPath(index);
		break;
	case NodeType::MOVE_TO_LEAF:
		MoveToLeaf(index);
		break;
	case NodeType::HAS_DWELLING_LEAF:
		HasDwellingLeaf(index);
		break;
	case NodeType::SET_DWELLING_LEAF:
		SetDwellingLeaf(index);
		break;
	case NodeType::ENTER_DWELLING_LEAF:
		EnterDwellingLeaf(index);
		break;
	case NodeType::EXIT_DWELLING_LEAF:
		ExitDwellingLeaf(index);
		break;
	case NodeType::ENTER_WORKING_PLACE_LEAF:
		EnterWorkingPlaceLeaf(index);
		break;
	case NodeType::EXIT_WORKING_PLACE_LEAF:
		ExitWorkingPlaceLeaf(index);
		break;
	case NodeType::HAS_JOB_LEAF:
		HasJobLeaf(index);
		break;
	case NodeType::HAS_STATIC_JOB_LEAF:
		HasStaticJobLeaf(index);
		break;
	case NodeType::ASSIGN_JOB_LEAF:
		AssignJobLeaf(index);
		break;
	case NodeType::IS_DAY_TIME_LEAF:
		IsDayTimeLeaf(index);
		break;
	case NodeType::IS_NIGHT_TIME_LEAF:
		IsNightTimeLeaf(index);
		break;
	case NodeType::WAIT_DAY_TIME_LEAF:
		WaitDayTimeLeaf(index);
		break;
	case NodeType::WAIT_NIGHT_TIME_LEAF:
		WaitNightTimeLeaf(index);
		break;
	case NodeType::ASK_INVENTORY_TASK_LEAF:
		AskInventoryTaskLeaf(index);
		break;
	case NodeType::TAKE_RESOURCE_LEAF:
		TakeResourcesLeaf(index);
		break;
	case NodeType::FIND_PATH_TO_LEAF:
		FindPathToLeaf(index);
		break;
	default: 
		std::cout << "Node not implemented\n";
		break;
	}
}

void Node::SequenceComposite(const unsigned int index)
{
#ifdef BT_AOS
	//if last one returned fail => then it's a fail
	if (m_BehaviorTree->dataBehaviorTree[index].previousStatus == NodeStatus::FAIL)
	{
		m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::FAIL;
		return;
	}

	//If last one is parent => first time entering the sequence
	if (m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown)
	{
		m_BehaviorTree->dataBehaviorTree[index].sequenceActiveChild = 0;
		m_BehaviorTree->dataBehaviorTree[index].currentNode = static_cast<CompositeData*>(m_Datas.get())->m_Children[0];

		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
		return;
	}

	//Else it means that the previous node is a children
	for (size_t i = 0; i < static_cast<CompositeData*>(m_Datas.get())->m_Children.size(); i++)
	{
		if (i == m_BehaviorTree->dataBehaviorTree[index].sequenceActiveChild)
		{
			if (i == static_cast<CompositeData*>(m_Datas.get())->m_Children.size() - 1)
			{
				m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

				m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
				return;
			}
			else
			{
				m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoDown;
				m_BehaviorTree->dataBehaviorTree[index].sequenceActiveChild++;
				m_BehaviorTree->dataBehaviorTree[index].currentNode = static_cast<CompositeData*>(m_Datas.get())->m_Children[i + 1];

				m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
				return;
			}
		}
	}
	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
#endif

#ifdef BT_SOA
		//if last one returned fail => then it's a fail
		if (m_BehaviorTree->previousStatus[index] == Status::FAIL)
		{
			m_BehaviorTree->currentNode[index] = m_ParentNode;

			m_BehaviorTree->previousStatus[index] = Status::FAIL;
			return;
		}

		//If last one is parent => first time entering the sequence
		if (m_BehaviorTree->doesFlowGoDown[index])
		{
			m_BehaviorTree->sequenceActiveChild[index] = 0;
			m_BehaviorTree->currentNode[index] = m_Children[0];

			m_BehaviorTree->previousStatus[index] = Status::RUNNING;
			return;
		}

		//Else it means that the previous node is a children
		for (size_t i = 0; i < m_Children.size(); i++)
		{
			if (i == m_BehaviorTree->sequenceActiveChild[index])
			{
				if (i == m_Children.size() - 1)
				{
					m_BehaviorTree->currentNode[index] = m_ParentNode;

					m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
					return;
				}
				else
				{
					m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoDown;
					m_BehaviorTree->sequenceActiveChild[index]++;
					m_BehaviorTree->currentNode[index] = m_Children[i + 1];

					m_BehaviorTree->previousStatus[index] = Status::RUNNING;
					return;
				}
			}
		}
		m_BehaviorTree->previousStatus[index] = Status::RUNNING;
#endif
}

void Node::SelectorComposite(const unsigned int index)
{
#ifdef BT_AOS
	//If last one is parent => first time entering the sequence
	if (m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown)
	{
		m_BehaviorTree->dataBehaviorTree[index].sequenceActiveChild = 0;
		m_BehaviorTree->dataBehaviorTree[index].currentNode = static_cast<CompositeData*>(m_Datas.get())->m_Children[0];

		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
		return;
	}

	//Else it means that the previous node is a children
	for (size_t i = 0; i < static_cast<CompositeData*>(m_Datas.get())->m_Children.size(); i++)
	{
		if (i == m_BehaviorTree->dataBehaviorTree[index].sequenceActiveChild)
		{
			//If last one is a success => going out of node
			if (m_BehaviorTree->dataBehaviorTree[index].previousStatus == NodeStatus::SUCCESS)
			{
				m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

				m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
				return;
			}

			//Else if not last child => go next child
			if (i < static_cast<CompositeData*>(m_Datas.get())->m_Children.size() - 1)
			{
				m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoDown;
				m_BehaviorTree->dataBehaviorTree[index].currentNode = static_cast<CompositeData*>(m_Datas.get())->m_Children[i + 1];
				m_BehaviorTree->dataBehaviorTree[index].sequenceActiveChild++;

				m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
				return;
			}
			else //mean that they all failed => return fail
			{
				m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

				m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::FAIL;
				return;
			}
		}
	}
	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
#endif

#ifdef BT_SOA
	//If last one is parent => first time entering the sequence
	if (m_BehaviorTree->doesFlowGoDown[index])
	{
		m_BehaviorTree->sequenceActiveChild[index] = 0;
		m_BehaviorTree->currentNode[index] = m_Children[0];

		m_BehaviorTree->previousStatus[index] = Status::RUNNING;
		return;
	}

	//Else it means that the previous node is a children
	for (size_t i = 0; i < m_Children.size(); i++)
	{
		if (i == m_BehaviorTree->sequenceActiveChild[index])
		{
			//If last one is a success => going out of node
			if (m_BehaviorTree->previousStatus[index] == Status::SUCCESS)
			{
				m_BehaviorTree->currentNode[index] = m_ParentNode;

				m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
				return;
			}

			//Else if not last child => go next child
			if (i < m_Children.size() - 1)
			{
				m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoDown;
				m_BehaviorTree->currentNode[index] = m_Children[i + 1];
				m_BehaviorTree->sequenceActiveChild[index]++;

				m_BehaviorTree->previousStatus[index] = Status::RUNNING;
				return;
			}
			else //mean that they all failed => return fail
			{
				m_BehaviorTree->currentNode[index] = m_ParentNode;

				m_BehaviorTree->previousStatus[index] = Status::FAIL;
				return;
			}
		}
	}
	m_BehaviorTree->previousStatus[index] = Status::RUNNING;
#endif
}

void Node::RepeaterDecorator(const unsigned int index)
{
#ifdef BT_AOS
	//If flow goes down => start counter 
	if (m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown)
	{
		m_BehaviorTree->dataBehaviorTree[index].repeaterCounter = 0;
	}
	else
	{
		//If limit == 0 => infinity, if m_Counter == m_Limit it's over
		if (static_cast<RepeaterData*>(m_Datas.get())->m_Limit > 0 && ++m_BehaviorTree->dataBehaviorTree[index].repeaterCounter == static_cast<RepeaterData*>(m_Datas.get())->m_Limit)
		{
			m_BehaviorTree->dataBehaviorTree[index].repeaterCounter = 0;

			m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
			m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

			m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
			return;
		}

		if (static_cast<RepeaterData*>(m_Datas.get())->m_Limit > 0) {}
	}

	//Switch current node to child
	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoDown;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = static_cast<RepeaterData*>(m_Datas.get())->m_Child;

	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
#endif

#ifdef BT_SOA
	//If flow goes down => start counter 
	if (m_BehaviorTree->doesFlowGoDown[index])
	{
		m_BehaviorTree->repeaterCounter[index] = 0;
	}
	else
	{
		//If limit == 0 => infinity, if m_Counter == m_Limit it's over
		if (m_Limit > 0 && ++m_BehaviorTree->repeaterCounter[index] == m_Limit)
		{
			m_BehaviorTree->repeaterCounter[index] = 0;

			m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
			m_BehaviorTree->currentNode[index] = m_ParentNode;

			m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
			return;
		}

		if (m_Limit > 0) {}
	}

	//Switch current node to child
	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoDown;
	m_BehaviorTree->currentNode[index] = m_Child;

	m_BehaviorTree->previousStatus[index] = Status::RUNNING;
#endif
}

void Node::RepeatUntilFailDecorator(const unsigned int index)
{
#ifdef BT_AOS
	if (m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown)
	{
		m_BehaviorTree->dataBehaviorTree[index].currentNode = static_cast<DecoratorData*>(m_Datas.get())->m_Child;

		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
	}
	else
	{
		if (m_BehaviorTree->dataBehaviorTree[index].previousStatus == NodeStatus::FAIL)
		{
			m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;
			m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
		}
		else
		{
			m_BehaviorTree->dataBehaviorTree[index].currentNode = static_cast<DecoratorData*>(m_Datas.get())->m_Child;
			m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
		}
	}
#endif

#ifdef BT_SOA
	if (m_BehaviorTree->doesFlowGoDown[index])
	{
		m_BehaviorTree->currentNode[index] = m_Child;

		m_BehaviorTree->previousStatus[index] = Status::RUNNING;
	}
	else
	{
		if (m_BehaviorTree->previousStatus[index] == Status::FAIL)
		{
			m_BehaviorTree->currentNode[index] = m_ParentNode;
			m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
		}
		else
		{
			m_BehaviorTree->currentNode[index] = m_Child;
			m_BehaviorTree->previousStatus[index] = Status::RUNNING;
		}
	}
#endif
}

void Node::InverterDecorator(const unsigned int index)
{
#ifdef BT_AOS
	if (m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown)
	{
		m_BehaviorTree->dataBehaviorTree[index].currentNode = static_cast<DecoratorData*>(m_Datas.get())->m_Child;

		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
		return;
	}

	if (m_BehaviorTree->dataBehaviorTree[index].previousStatus == NodeStatus::SUCCESS)
	{
		m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::FAIL;
		return;
	}

	if (m_BehaviorTree->dataBehaviorTree[index].previousStatus == NodeStatus::FAIL)
	{
		m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
		return;
	}

	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::FAIL;
#endif

#ifdef BT_SOA
	if (m_BehaviorTree->doesFlowGoDown[index])
	{
		m_BehaviorTree->currentNode[index] = m_Child;

		m_BehaviorTree->previousStatus[index] = Status::RUNNING;
		return;
	}

	if (m_BehaviorTree->previousStatus[index] == Status::SUCCESS)
	{
		m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->currentNode[index] = m_ParentNode;

		m_BehaviorTree->previousStatus[index] = Status::FAIL;
		return;
	}

	if (m_BehaviorTree->previousStatus[index] == Status::FAIL)
	{
		m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->currentNode[index] = m_ParentNode;

		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
		return;
	}

	m_BehaviorTree->previousStatus[index] = Status::FAIL;
#endif
}

void Node::SucceederDecorator(const unsigned int index)
{
#ifdef BT_AOS
	if (m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown)
	{
		m_BehaviorTree->dataBehaviorTree[index].currentNode = static_cast<DecoratorData*>(m_Datas.get())->m_Child;

		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
		return;
	}

	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;
	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
#endif

#ifdef BT_SOA
	if (m_BehaviorTree->doesFlowGoDown[index])
	{
		m_BehaviorTree->currentNode[index] = m_Child;

		m_BehaviorTree->previousStatus[index] = Status::RUNNING;
		return;
	}

	m_BehaviorTree->currentNode[index] = m_ParentNode;
	m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
#endif
}

void Node::WaitForPath(const unsigned int index)
{
#ifdef BT_AOS
	const auto hasPath = m_BehaviorTree->dwarfManager->HasPath(index);

	if (hasPath)
	{
		m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
		return;
	}

	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
#endif

#ifdef BT_SOA
	if (m_BehaviorTree->dwarfManager->HasPath(index))
	{
		m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->currentNode[index] = m_ParentNode;

		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
		return;
	}

	m_BehaviorTree->previousStatus[index] = Status::RUNNING;
#endif
}

void Node::MoveToLeaf(const unsigned int index)
{
#ifdef BT_AOS
	const auto isAtDestination = m_BehaviorTree->dwarfManager->IsDwarfAtDestination(index);

	if (isAtDestination)
	{
		m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
		return;
	}

	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;

	m_BehaviorTree->dwarfManager->AddPathFollowingBT(index);
#endif

#ifdef BT_SOA
	if (m_BehaviorTree->dwarfManager->IsDwarfAtDestination(index))
	{
		m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->currentNode[index] = m_ParentNode;

		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
		return;
	}

	m_BehaviorTree->dwarfManager->AddPathFollowingBT(index);

	m_BehaviorTree->previousStatus[index] = Status::RUNNING;
#endif
}

void Node::HasDwellingLeaf(const unsigned int index)
{
#ifdef BT_AOS
	const auto hasDwelling = m_BehaviorTree->dwarfManager->GetDwellingEntity(index);
	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	if (hasDwelling == INVALID_ENTITY)
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::FAIL;
	}
	else
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
	}
#endif

#ifdef BT_SOA
	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	if (m_BehaviorTree->dwarfManager->GetDwellingEntity(index) == INVALID_ENTITY)
	{
		m_BehaviorTree->previousStatus[index] = Status::FAIL;
	}
	else
	{
		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
	}
#endif
}

void Node::SetDwellingLeaf(const unsigned int index)
{
#ifdef BT_AOS
	const auto hasBeenAssigned = m_BehaviorTree->dwarfManager->AssignDwellingToDwarf(index);
	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	if (hasBeenAssigned)
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::FAIL;
	}
	else
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
	}
#endif

#ifdef BT_SOA
	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	if (m_BehaviorTree->dwarfManager->AssignDwellingToDwarf(index))
	{
		m_BehaviorTree->previousStatus[index] = Status::FAIL;
	}
	else
	{

		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
	}
#endif
}

void Node::EnterDwellingLeaf(const unsigned int index)
{
#ifdef BT_AOS
	m_BehaviorTree->dwarfManager->DwarfEnterDwelling(index);

	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
#endif

#ifdef BT_SOA
	m_BehaviorTree->dwarfManager->DwarfEnterDwelling(index);

	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
#endif
}

void Node::ExitDwellingLeaf(const unsigned int index)
{
#ifdef BT_AOS
	m_BehaviorTree->dwarfManager->DwarfExitDwelling(index);

	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
#endif

#ifdef BT_SOA
	m_BehaviorTree->dwarfManager->DwarfExitDwellin(index);

	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
#endif
}

void Node::EnterWorkingPlaceLeaf(const unsigned int index)
{
#ifdef BT_AOS
	m_BehaviorTree->dwarfManager->DwarfEnterWorkingPlace(index);

	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
#endif

#ifdef BT_SOA
	m_BehaviorTree->dwarfManager->DwarfEnterWorkingPlace(index);

	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
#endif
}

void Node::ExitWorkingPlaceLeaf(const unsigned int index)
{
#ifdef BT_AOS
	m_BehaviorTree->dwarfManager->DwarfExitWorkingPlace(index);

	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
#endif

#ifdef BT_SOA
	m_BehaviorTree->dwarfManager->DwarfExitWorkingPlace(index);

	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
#endif
}

void Node::HasJobLeaf(const unsigned int index)
{
	#ifdef BT_AOS
	const auto hasJob = m_BehaviorTree->dwarfManager->HasJob(index);

	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	if (hasJob)
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
	}
	else
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::FAIL;

	}
#endif

#ifdef BT_SOA
	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	if (m_BehaviorTree->dwarfManager->HasJob(index))
	{
		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
	}
	else
	{
		m_BehaviorTree->previousStatus[index] = Status::FAIL;

	}
#endif
}

void Node::HasStaticJobLeaf(const unsigned int index)
{
#ifdef BT_AOS
	const auto hasStaticJob = m_BehaviorTree->dwarfManager->HasStaticJob(index);

	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	if (hasStaticJob)
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
	}
	else
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::FAIL;

	}
#endif

#ifdef BT_SOA
	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	if (m_BehaviorTree->dwarfManager->HasJob(index))
	{
		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
	}
	else
	{
		m_BehaviorTree->previousStatus[index] = Status::FAIL;

	}
#endif
}

void Node::AssignJobLeaf(const unsigned int index)
{
#ifdef BT_AOS
	const auto jobAssigned = m_BehaviorTree->dwarfManager->AssignJob(index);

	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	if (jobAssigned)
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
	}
	else
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::FAIL;
	}
#endif

#ifdef BT_SOA
	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	if (m_BehaviorTree->dwarfManager->AssignJob(index))
	{
		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
	}
	else
	{
		m_BehaviorTree->previousStatus[index] = Status::FAIL;
	}
#endif
}

void Node::IsDayTimeLeaf(const unsigned int index)
{
#ifdef BT_AOS
	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	if (m_BehaviorTree->dwarfManager->IsDayTime())
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
	}
	else
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::FAIL;
	}
#endif

#ifdef BT_SOA
	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	if (m_BehaviorTree->dwarfManager->IsDayTime())
	{
		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
	}
	else
	{
		m_BehaviorTree->previousStatus[index] = Status::FAIL;
	}
#endif
}

void Node::IsNightTimeLeaf(const unsigned int index)
{
#ifdef BT_AOS
	const auto isNightTime = m_BehaviorTree->dwarfManager->IsNightTime();

	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	if (isNightTime)
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
	}
	else
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::FAIL;
	}
#endif

#ifdef BT_SOA
	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	if (m_BehaviorTree->dwarfManager->IsNightTime())
	{
		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
	}
	else
	{
		m_BehaviorTree->previousStatus[index] = Status::FAIL;
	}
#endif
}

void Node::WaitDayTimeLeaf(const unsigned int index)
{
#ifdef BT_AOS
	if (m_BehaviorTree->dwarfManager->IsDayTime())
	{
		m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
	}
	else
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
	}
#endif

#ifdef BT_SOA
	if (m_BehaviorTree->dwarfManager->IsDayTime())
	{
		m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->currentNode[index] = m_ParentNode;
		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
	}
	else
	{
		m_BehaviorTree->previousStatus[index] = Status::RUNNING;
	}
#endif
}

void Node::WaitNightTimeLeaf(const unsigned int index)
{
#ifdef BT_AOS
	if (m_BehaviorTree->dwarfManager->IsNightTime())
	{
		m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
	}
	else
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
	}
#endif

#ifdef BT_SOA
	if (m_BehaviorTree->dwarfManager->IsNightTime())
	{
		m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->currentNode[index] = m_ParentNode;
		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
	}
	else
	{
		m_BehaviorTree->previousStatus[index] = Status::RUNNING;
	}
#endif
}

void Node::AskInventoryTaskLeaf(const unsigned int index)
{
#ifdef BT_AOS
	if (m_BehaviorTree->dwarfManager->AddInventoryTaskBT(index))
	{
		m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
	}
	else
	{
		m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::RUNNING;
	}
#endif

#ifdef BT_SOA
	if (m_BehaviorTree->dwarfManager->AddInventoryTaskBT(index))
	{
		m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
		m_BehaviorTree->currentNode[index] = m_ParentNode;
		m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
	}
	else
	{
		m_BehaviorTree->previousStatus[index] = Status::RUNNING;
	}
#endif
}

void Node::TakeResourcesLeaf(const unsigned int index)
{
#ifdef BT_AOS
	m_BehaviorTree->dwarfManager->TakeResources(index);

	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
#endif

#ifdef BT_SOA
	m_BehaviorTree->dwarfManager->TakeResources(index);

	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
#endif
}

void Node::PutResourcesLeaf(const unsigned int index)
{
#ifdef BT_AOS
	m_BehaviorTree->dwarfManager->PutResources(index);

	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
#endif

#ifdef BT_SOA
	m_BehaviorTree->dwarfManager->PutResources(index);

	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
#endif
}

void Node::FindPathToLeaf(const unsigned int index)
{
	switch (static_cast<FindPathToData*>(m_Datas.get())->m_Destination)
	{
	case NodeDestination::RANDOM:
		m_BehaviorTree->dwarfManager->AddFindRandomPathBT(index);
		break;
	case NodeDestination::DWELLING:
		m_BehaviorTree->dwarfManager->AddFindPathToDestinationBT(index, m_BehaviorTree->dwarfManager->GetDwellingAssociatedPosition(index));
		break;
	case NodeDestination::WORKING_PLACE:
		m_BehaviorTree->dwarfManager->AddFindPathToDestinationBT(index, m_BehaviorTree->dwarfManager->GetWorkingPlaceAssociatedPosition(index));
		break;
	case NodeDestination::INVENTORY_TASK_GIVER:
		m_BehaviorTree->dwarfManager->AddInventoryTaskPathToGiver(index);
		break;
	case NodeDestination::INVENTORY_TASK_RECEIVER:
		m_BehaviorTree->dwarfManager->AddInventoryTaskPathToReceiver(index);
		break;
	default: ;
	}

#ifdef BT_AOS
	m_BehaviorTree->dataBehaviorTree[index].doesFlowGoDown = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->dataBehaviorTree[index].currentNode = m_ParentNode;

	m_BehaviorTree->dataBehaviorTree[index].previousStatus = NodeStatus::SUCCESS;
#endif

#ifdef BT_SOA
	m_BehaviorTree->doesFlowGoDown[index] = m_BehaviorTree->flowGoUp;
	m_BehaviorTree->currentNode[index] = m_ParentNode;

	m_BehaviorTree->previousStatus[index] = Status::SUCCESS;
#endif
}
}
