#include "kdtree.h"
#include "iostream"

KDNode::KDNode()
    : leftChild(nullptr), rightChild(nullptr), axis(0), minCorner(), maxCorner(), particles()
{}

KDNode::~KDNode()
{
    delete leftChild;
    delete rightChild;
}

KDTree::KDTree()
    : root(nullptr)
{}

KDTree::~KDTree()
{
    delete root;
}

// Comparator functions you can use with std::sort to sort vec3s along the cardinal axes
bool xSort(glm::vec3* a, glm::vec3* b) { return a->x < b->x; }
bool ySort(glm::vec3* a, glm::vec3* b) { return a->y < b->y; }
bool zSort(glm::vec3* a, glm::vec3* b) { return a->z < b->z; }


void KDTree::build(const std::vector<glm::vec3*> &points)
{


    //TODO
    //I think this time the points will be many so let's use uint instead of int?
    //unsigned int does not work well in for loops, change back!

    int size=points.size();

    //Init
    for( int i=0;i<size;i++)
    {
        xsorted.push_back(i);
        ysorted.push_back(i);
        zsorted.push_back(i);
    }

    //Insert Sort
    //Checked from Wikipidia, which is the most efficient way I think?
    //This sorts the points from big to small in xyz axis

    int x_tmp;
    int y_tmp;
    int z_tmp;

    /*
    for (i = 1; i < n; i++)
    {
        key = arr[i];
        j = i-1;


        while (j >= 0 && arr[j] > key)
        {
            arr[j+1] = arr[j];
            j = j-1;
        }
        arr[j+1] = key;
    }*/


    for(int i=0; i<size; i++)
    {
        x_tmp=xsorted.at(i);
        int j=i-1;

        while(j>=0 && xSort( points.at( xsorted.at(j) ) , points.at(x_tmp) ) )
        {
            xsorted.at(j+1)=xsorted.at(j);
            j = j-1;
        }
        xsorted.at(j+1)=x_tmp;
    }


    for(int i=0; i<size; i++)
    {
        y_tmp=ysorted.at(i);
        int j=i-1;

        while(j>=0 && ySort( points.at( ysorted.at(j) ) , points.at(y_tmp) ) )
        {
            ysorted.at(j+1)=ysorted.at(j);
            j = j-1;
        }
        ysorted.at(j+1)=y_tmp;
    }



    //Sort Z
    for(int i=0; i<size; i++)
    {
        z_tmp=zsorted.at(i);
        int j=i-1;

        while(j>=0 && zSort( points.at( zsorted.at(j) ) , points.at(z_tmp) ) )
        {
            zsorted.at(j+1)=zsorted.at(j);
            j = j-1;
        }
        zsorted.at(j+1)=z_tmp;
    }


    /*
    qDebug() << "X axis sort: ";
    for(int i=0;i<size;i++)
    {
        qDebug() << ( glm::to_string( *points.at(xsorted.at(i)) ) ).c_str();
    }

    qDebug() << "Y axis sort: ";
    for(int i=0;i<size;i++)
    {
        qDebug() << ( glm::to_string( *points.at(ysorted.at(i)) ) ).c_str();
    }

    qDebug() << "Z axis sort: ";
    for(int i=0;i<size;i++)
    {
        qDebug() << ( glm::to_string( *points.at(zsorted.at(i)) ) ).c_str();
    }
    */

    //build root
    root=recursive(0,0,size-1,points);

    int min=size-1;
    int max=0;

    glm::vec3 xmin=*points.at(xsorted.at(min));
    glm::vec3 ymin=*points.at(ysorted.at(min));
    glm::vec3 zmin=*points.at(zsorted.at(min));
    minCorner=glm::vec3(xmin.x,ymin.y,zmin.z);

    glm::vec3 xmax=*points.at(xsorted.at(max));
    glm::vec3 ymax=*points.at(ysorted.at(max));
    glm::vec3 zmax=*points.at(zsorted.at(max));
    maxCorner=glm::vec3(xmax.x,ymax.y,zmax.z);

    qDebug() << "build complete!";

}

//dir  = direction
//max-min = how many particles in this segment
//node = the KD node we will build

KDNode* KDTree::recursive( int dir,
                           int min,
                           int max,
                           const std::vector<glm::vec3*> &points)
{
    int size=max-min;
    KDNode* node=new KDNode();

    if(size>0)
    {
        int left,right;

        if(size%2 == 0)
        {
            //odd num of elements
            left=(size/2);
            right=size-left;

            left+=min;
            right+=min;

            if(size>1)
            {
                node->particles.push_back(points.at(left));
                //qDebug() << "branch pushed " << left;
            }


            right++;
            if(right>max)
            {
                right=max;
            }

            //qDebug() << "dir = " <<dir ;
            //qDebug() << "left child is " <<min << " to " <<left;
            //qDebug() << "right child is " <<right << " to " <<max;

            //Spawn left child
            node->leftChild=recursive(dir+1,min,left,points);
            //Spawn right child
            node->rightChild=recursive(dir+1,right,max,points);
        }
        else
        {
            //even num of elements
            left=(size/2);
            right=size-left;

            left+=min;
            right+=min;

            if(size>1)
            {
                node->particles.push_back(points.at(left));
                //qDebug() << "branch pushed " << left;
            }

            left--;
            if(left<min)
            {
                left=min;
            }

            //qDebug() << "dir = " <<dir ;
            //qDebug() << "left child is " <<min << " to " <<left;
            //qDebug() << "right child is " <<right << " to " <<max;

            //Spawn left child
            node->leftChild=recursive(dir+1,min,left,points);
            //Spawn right child
            node->rightChild=recursive(dir+1,right,max,points);
        }
    }
    else
    {
        node->particles.push_back(points.at(min));
        node->leftChild=NULL;
        node->rightChild=NULL;
        //qDebug() << "leaf pushed" << min;

    }

    //Min-Max Corners

    node->axis=dir%3;

    glm::vec3 xmin=*points.at(xsorted.at(min));
    glm::vec3 ymin=*points.at(ysorted.at(min));
    glm::vec3 zmin=*points.at(zsorted.at(min));
    node->minCorner=glm::vec3(xmin.x,ymin.y,zmin.z);

    glm::vec3 xmax=*points.at(xsorted.at(max));
    glm::vec3 ymax=*points.at(ysorted.at(max));
    glm::vec3 zmax=*points.at(zsorted.at(max));
    node->maxCorner=glm::vec3(xmax.x,ymax.y,zmax.z);

    //test
    glm::vec3 tmp=node->minCorner;
    node->minCorner=node->maxCorner;
    node->maxCorner=tmp;

    return node;

}


std::vector<glm::vec3> KDTree::particlesInSphere(glm::vec3 c, float r)
{
    //init
    std::vector<glm::vec3> buffer;
    buffer.clear();

    KDNode* p;

    p=root;

    if(p->particles.size()>0)
    {
        glm::vec3 pos=*(p->particles.at(0));

        float length=glm::dot(pos-c,pos-c);

        if(length<r*r)
        {
            qDebug()<< "Find ! Len= " << length;
            buffer.push_back(pos);
        }
        if(p->leftChild!=NULL)
        {
            scan(buffer,p->leftChild,c,r);
        }
        if(p->rightChild!=NULL)
        {
            scan(buffer,p->rightChild,c,r);
        }
    }

    qDebug() << "Find Finished!";

    return buffer;
}

void KDTree::scan(std::vector<glm::vec3> &buffer,KDNode* p, glm::vec3 c, float r)
{
    if(p->particles.size()>0)
    {
        glm::vec3 pos=*(p->particles.at(0));

        float length=glm::dot(pos-c,pos-c);

        if(length<r*r)
        {
            qDebug()<< "Find ! Len= " << length;
            buffer.push_back(pos);
        }

        if(p->leftChild!=NULL)
        {
            scan(buffer,p->leftChild,c,r);
        }
        if(p->rightChild!=NULL)
        {
            scan(buffer,p->rightChild,c,r);
        }
    }
}

void KDTree::clear()
{
    delete root;
    root = nullptr;
}
