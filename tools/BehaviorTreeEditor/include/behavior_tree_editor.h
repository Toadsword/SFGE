/*

MIT License

Copyright (c) 2018 SAE Institute Switzerland AG

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
#ifndef SFGE_TOOLS_BEHAVIOR_TREE_EDITOR_H
#define SFGE_TOOLS_BEHAVIOR_TREE_EDITOR_H

#include <engine/system.h>

#include <file_selector.h>
#include <extensions/AI/behavior_tree_nodes_core.h>
#include <utility/json_utility.h>

namespace sfge::tools
{

	using ext::behavior_tree::Node;
	using ext::behavior_tree::NodeGroup;

class BehaviorTreeEditor : public System
{
public:
    using System::System;
    /**
     * \brief Called at scene init (a good place to link to other Systems
     */
    void Init() override;
    /**
     * \brief Called every graphic frame (dt depends on the use of VSync or not, controllable in the Configuration) 
     */
    void Update(float dt) override;
    /**
     * \brief Called every graphic frame after Update
     */
    void Draw() override;

private:

	void LoadBehaviourTree(std::string& path);
	void NewBehaviorTreeFile(const std::string&fileName);

	void DisplayNode(const Node::ptr& node);
	bool DisplayDeleteButton(const Node::ptr& node);
	void DisplayAddButton(const Node::ptr& node);
	void DisplayDeleteMenu();
	void DisplayAddMenu();
	void DisplayAddMenuComposite();
	void DisplayAddMenuDecorator();
	void DisplayAddMenuLeaf();
	void DisplayNewMenu();
	void DisplaySaveMenu() const;
	void DisplayNodeInfo() const;


	FileBrowserModal m_FileBrowser;

    Node::ptr m_RootNode = nullptr;
	std::string m_CurrentFilePath = "";

	int m_IndexButton = 0;

	Node::ptr m_NodeToAddChild = nullptr;

	//Node selection
	int m_SelectedNodeToAdd = -1;
	int m_ClickedNode = -1;
	int m_IndexClickableNode = 0;
	static int m_SelectionMask;
	Node::ptr m_SelectedNode = nullptr;
};
}
#endif