#pragma once
#include <ignore_me/la.h>

class KDNode
{
public:
    KDNode();
    ~KDNode();

    KDNode* leftChild;
    KDNode* rightChild;

    //0 means X-axis, 1 means Y-axis, and 2 means Z-axis
    unsigned int axis; // Which axis split this node represents

    glm::vec3 minCorner, maxCorner; // The world-space bounds of this node
    std::vector<glm::vec3*> particles; // A collection of pointers to the particles contained in this node.
};

class KDTree
{
public:
    KDTree();
    ~KDTree();
    void build(const std::vector<glm::vec3*> &points);
    void clear();

    //recursor used in building
    KDNode* recursive(int dir,
                   int min,
                   int max,
                   const std::vector<glm::vec3 *> &points);

    //used to store sorted ID in xyz axis
    std::vector<int> xsorted;
    std::vector<int> ysorted;
    std::vector<int> zsorted;

    //store KD tree
    std::vector<KDNode> tree;

    std::vector<glm::vec3> particlesInSphere(glm::vec3 c, float r); // Returns all the points contained within a sphere with center c and radius r
    //Iterate Function for KD-Scearch!
    void scan(std::vector<glm::vec3> &buffer,KDNode* p, glm::vec3 c, float r);

    KDNode* root;
    glm::vec3 minCorner, maxCorner; // For visualization purposes
};
