#include "core/graph/Node.h"

namespace nav {

Node::Node() : id_(INVALID_ID), position_() {}

Node::Node(Id id, const Point2D& position) : id_(id), position_(position) {}

} // namespace nav
