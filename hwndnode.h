#ifndef HWNDNODE_H
#define HWNDNODE_H

#include <QList>
#include <Windows.h>

class HWNDNode
{
public:
    HWNDNode(HWND node = ::GetDesktopWindow());
    ~HWNDNode();

    void Sync();

public:
    HWND m_node;
    QList<HWNDNode*>* m_children;
};

#endif // HWNDNODE_H
