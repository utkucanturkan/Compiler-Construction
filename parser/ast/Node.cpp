#include "ast/Node.h"

Node::Node(NodeType nodeType, FilePos pos) :nodeType_(nodeType), pos_(pos) {

}

const NodeType Node::getNodeType() const {
	return nodeType_;
}

const FilePos Node::getFilePos() const {
	return pos_;
}