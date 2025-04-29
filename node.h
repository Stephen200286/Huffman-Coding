#include <cstdint>
#include <memory>

struct Node {
	Node(uint8_t v, uint64_t f);
	Node(uint8_t v, uint64_t f, std::shared_ptr<Node> l, std::shared_ptr<Node> r);
	
	uint8_t val = 0x00;
	uint64_t freq = 0;
	std::shared_ptr<Node> left = nullptr;
	std::shared_ptr<Node> right = nullptr;

	friend bool operator<(Node const& lhs, Node const& rhs);
};

