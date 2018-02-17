#pragma once

#include "Material.hpp"
#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include <glm/glm.hpp>

#include <list>
#include <string>
#include <iostream>

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();

    virtual void render(glm::mat4 view, ShaderProgram* shader, BatchInfoMap* batchInfoMap, bool do_picking);

    void renderChildren(glm::mat4 view, ShaderProgram* shader, BatchInfoMap* batchInfoMap, bool do_picking);
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);
    void applyTranslate(const glm::vec3& amount, glm::mat4 parentTrans);
    void applyRot(char axis, float angle, glm::mat4 parentTrans);
    void applyRot2(float angleX, float angleY, glm::mat4 parentTrans);
    void applyBallRot(glm::mat4 rotMat, glm::mat4 parentTrans);
    void adjustTransl(const glm::vec3 amount);
    void saveTrans();
    void revertSelectionClr();

    // Picking
    SceneNode* getByID(unsigned int id);

	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

	bool isSelected;
    
    // Transformation Elements
    glm::mat4 trans;
    glm::mat4 invtrans;
    glm::mat4 oldTrans;

    glm::mat4 localTrans;
    glm::mat4 oldLocalTrans;

    glm::mat4 totalTrans;

    glm::mat4 originalTrans;

    glm::mat4 transl;
    glm::mat4 invTransl;
    glm::mat4 oldTransl;
    glm::mat4 oldInvTransl;
    glm::mat4 origTransl;
    glm::mat4 origInvTransl;
    
    SceneNode* parent;
    glm::mat4 curParentTrans;

    std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
	unsigned int m_nodeId;

    // Menu Elements
    void reset();


private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
