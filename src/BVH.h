#ifndef BVH_H
#define BVH_H

#include <algorithm>
#include <iomanip>
#include <map>
#include "Model.h"
#include "AABB.h"
#include "Ray.h"

class BVH {
public:
    struct Node {
        Node(): size(0), left(nullptr), right(nullptr) {}
        ~Node() {
            if (left)
                delete left;
            if (right)
                delete right;
        }

        // model_index -> triangles_indices
        std::map<int, std::vector<int>> indices;
        int size;

        AABB aabb;

        Node* left;
        Node* right;
    };

    BVH(const std::vector<Model*>& models, int minSplit=100): root(new Node()),
                                                          models(models),
                                                          minSplit(minSplit),
                                                          numberOfNodes(1) {
        std::cout << "BVH.h" << std::endl;
        std::cout << "      Building BVH.. ";

        if (models.size() == 0)
            throw std::length_error("Length of models vector is 0.");
 
        for (std::size_t i = 0; i < models.size(); i++) {
            std::vector<int> currentIndices;
            for (std::size_t j = 0; j < models[i]->getIndices().size(); j++) {
                currentIndices.push_back(j);
            }
            root->indices[i] = currentIndices;
            root->size += currentIndices.size();
        }

        root->aabb = computeOverallAABB(models);
        recursiveBuild(root);
        std::cout << "Done" << std::endl;
        printInfos();
    }

    ~BVH() { delete root; }

    std::vector<Node*> recursiveIntersect(Node* node, const Ray& ray) const {
        std::vector<Node*> nodesIntersected;
        if (!ray.intersectAABB(node->aabb))
            return nodesIntersected;

        if (!node->left && !node->right) {
            nodesIntersected.push_back(node);
            return nodesIntersected;
        }

        std::vector<Node*> leftIntersected, rightIntersected;
        if (node->left)
            leftIntersected = recursiveIntersect(node->left, ray);
        if (node->right)
            rightIntersected = recursiveIntersect(node->right, ray);

        // Merge the vectors of intersected nodes
        nodesIntersected.reserve(leftIntersected.size() + rightIntersected.size());
        nodesIntersected.insert(nodesIntersected.end(), leftIntersected.begin(), leftIntersected.end());
        nodesIntersected.insert(nodesIntersected.end(), rightIntersected.begin(), rightIntersected.end());

        return nodesIntersected;
    }

    bool intersect(const Ray& ray, std::map<int, std::vector<int>>& indices) const {
        std::vector<Node*> nodesIntersected = recursiveIntersect(root, ray);
        if (nodesIntersected.size() == 0)
            return false;

        // Merge the indices of each intersected node
        for (Node* node: nodesIntersected)
            for (auto& item: node->indices)
                indices[item.first].insert(indices[item.first].end(), item.second.begin(), item.second.end());

        return true;
    }

    bool intersect(const Ray& ray, std::vector<Node*>& nodesIntersected) const {
        nodesIntersected = recursiveIntersect(root, ray);
        if (nodesIntersected.size() == 0)
            return false;

        return true;
    }

    void printInfos() const {
        std::cout << "      # of models:        " << models.size() << std::endl;
        std::cout << "      min. size to split: " << minSplit << std::endl;
        std::cout << "      number of nodes:    " << numberOfNodes << std::endl;
        // std::cout << "BVH Tree:" << std::endl;
        // printTreePostorder(root);
    }

    void printTreePostorder(Node* node, int indent=0) const {
        if(node->left)
            printTreePostorder(node->left, indent+10);

        if (indent)
            std::cout << std::setw(indent) << ' ';
        std::cout << "#triangles: " << node->size << std::endl;

        if(node->right)
            printTreePostorder(node->right, indent+10);
    }

private:
    AABB computeOverallAABB(const std::vector<Model*>& models) {
        AABB aabb;
        for (const auto& m: models)
            aabb.update(m->getAABB());

        return aabb;
    }

    void recursiveBuild(Node* node) {
        if (!node || node->size <= minSplit)
            return;

        Vec3<float> median = (node->aabb.getMinBound() + node->aabb.getMaxBound()) / 2.f;
        Vec3<float> midPoint = node->aabb.getMaxBound() - node->aabb.getMinBound();

        // Pick the axis with the largest range
        int axis = std::abs(midPoint[0]) > std::abs(midPoint[1]) ? 0 : 1;
        axis = std::abs(midPoint[axis]) > std::abs(midPoint[2]) ? axis : 2;

        // Split by the pivot
        for (int i = 0; i < 3; i++)
            if(split(node, median[(axis + i) % 3], (axis + i) % 3)) {
                numberOfNodes += 2;
                break;
            }

        recursiveBuild(node->left);
        recursiveBuild(node->right);
    }

    bool split(Node* node, float pivot, int axis) {
        Node* left = new Node();
        Node* right = new Node();

        for (std::size_t i = 0; i < models.size(); i++) {
            const Model& model = *models[i];
            const auto& triangleIndices = model.getIndices();

            for (int index: node->indices[i]) {
                const auto& triangle = triangleIndices[index];
                Vec3<float> v0 = model.getVertices()[triangle[0]];
                Vec3<float> v1 = model.getVertices()[triangle[1]];
                Vec3<float> v2 = model.getVertices()[triangle[2]];
                AABB aabb; aabb.update(v0); aabb.update(v1); aabb.update(v2);

                float centroid = (aabb.getMinBound() + aabb.getMaxBound())[axis] / 2.f;

                if (centroid < pivot) {
                    left->indices[i].push_back(index);
                    left->aabb.update(aabb);
                    left->size++;
                } else {
                    right->indices[i].push_back(index);
                    right->aabb.update(aabb);
                    right->size++;
                }
            }
        }

        if (left->size == 0 || right->size == 0) {
            delete left;
            delete right;
        } else {
            node->left = left;
            node->right = right;
        }

        return node->left && node->right;
    }

    Node* root;
    const std::vector<Model*>& models;
    int minSplit;
    int numberOfNodes;
};

#endif