#include "hwndnode.h"

HWNDNode::HWNDNode(HWND node)
{
    m_node = node;
    m_children = new QList<HWNDNode*>;
}

HWNDNode::~HWNDNode(){
    if(m_children !=nullptr){
        delete m_children;
        m_children = nullptr;
    }
}

void HWNDNode::Sync(){
    m_children->clear();
    HWND child=GetWindow(m_node,GW_CHILD);        //得到屏幕上第一个子窗口
    while(child!=nullptr)                    //循环得到所有的子窗口
    {
        HWNDNode* childnode=new HWNDNode(child);
        childnode->Sync();
        m_children->append(childnode);
        child=GetNextWindow(child,GW_HWNDNEXT);
    }
}
