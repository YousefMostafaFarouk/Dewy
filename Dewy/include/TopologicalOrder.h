#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Maintains a directed acyclic graph together with one valid topological order.
//
// Invariant: for every stored edge source -> destination,
// Position(source) < Position(destination).
//
// That invariant makes the common edge-insertion case cheap. A proposed edge
// that already points forward in the current order cannot create a cycle, so it
// is accepted in O(1) average time. A backward edge is checked by re-sorting
// only the order slice between its endpoints instead of searching the entire
// graph. Node is expected to be inexpensive to copy (an ID or pointer).
template<typename Node, typename Hash = std::hash<Node>, typename Equal = std::equal_to<Node>>
class TopologicalOrder
{
public:
	// A disconnected node can safely be appended: it has no ordering
	// requirements until its first edge is added.
	bool AddNode(const Node& node)
	{
		if (m_position.find(node) != m_position.end())
			return false;

		m_position.emplace(node, m_order.size());
		m_order.push_back(node);
		m_outgoing.emplace(node, NodeSet{});
		return true;
	}

	bool TryAddEdge(const Node& source, const Node& destination)
	{
		// Both endpoints must already be owned by the graph. A self-edge is a
		// cycle of length one and is therefore always rejected.
		const auto sourcePosition = m_position.find(source);
		const auto destinationPosition = m_position.find(destination);
		if (sourcePosition == m_position.end() || destinationPosition == m_position.end() ||
			m_equal(source, destination))
		{
			return false;
		}

		NodeSet& sourceEdges = m_outgoing.at(source);
		// Multiple physical wires can represent the same entity-level graph
		// edge. Treating an existing edge as success keeps insertion idempotent.
		if (sourceEdges.find(destination) != sourceEdges.end())
			return true;

		// If source is already before destination, adding the edge preserves the
		// invariant. A destination-to-source path cannot exist, because such a
		// path would require destination to already appear before source.
		if (sourcePosition->second < destinationPosition->second)
		{
			sourceEdges.insert(destination);
			return true;
		}

		// For a backward edge source -> destination, any existing path from
		// destination back to source can contain only nodes between their two
		// positions. Checking this closed slice is therefore sufficient.
		const std::size_t left = destinationPosition->second;
		const std::size_t right = sourcePosition->second;
		const std::vector<Node> reordered = SortSliceWithEdge(left, right, source, destination);

		// SortSliceWithEdge works on temporary containers. An empty result means
		// the proposed edge formed a cycle, so the live graph remains unchanged.
		if (reordered.empty())
			return false;

		// Commit only after the temporary slice was sorted successfully.
		sourceEdges.insert(destination);
		for (std::size_t offset = 0; offset < reordered.size(); ++offset)
		{
			m_order[left + offset] = reordered[offset];
			m_position[reordered[offset]] = left + offset;
		}
		return true;
	}

	bool RemoveEdge(const Node& source, const Node& destination)
	{
		const auto sourceEdges = m_outgoing.find(source);
		if (sourceEdges == m_outgoing.end())
			return false;
		return sourceEdges->second.erase(destination) != 0;
	}

	bool RemoveNode(const Node& node)
	{
		const auto foundPosition = m_position.find(node);
		if (foundPosition == m_position.end())
			return false;

		// Remove incoming edges as well as the node's own outgoing edge set.
		for (auto& outgoing : m_outgoing)
			outgoing.second.erase(node);
		m_outgoing.erase(node);

		// Removing a node cannot invalidate the relative order of the remaining
		// nodes, but their cached numeric positions must be compacted.
		const std::size_t removedPosition = foundPosition->second;
		m_position.erase(foundPosition);
		m_order.erase(m_order.begin() + static_cast<std::ptrdiff_t>(removedPosition));
		for (std::size_t index = removedPosition; index < m_order.size(); ++index)
			m_position[m_order[index]] = index;
		return true;
	}

	bool Reset(const std::vector<Node>& nodes, const std::vector<std::pair<Node, Node>>& edges)
	{
		// Build everything in local containers so invalid input or a cycle does
		// not partially replace the graph already held by this object.
		std::vector<Node> newOrder = nodes;
		PositionMap originalPosition;
		OutgoingMap newOutgoing;
		DegreeMap indegree;

		for (std::size_t index = 0; index < nodes.size(); ++index)
		{
			if (!originalPosition.emplace(nodes[index], index).second)
				return false;
			newOutgoing.emplace(nodes[index], NodeSet{});
			indegree.emplace(nodes[index], 0);
		}

		for (const auto& edge : edges)
		{
			if (m_equal(edge.first, edge.second) ||
				originalPosition.find(edge.first) == originalPosition.end() ||
				originalPosition.find(edge.second) == originalPosition.end())
			{
				return false;
			}

			if (newOutgoing.at(edge.first).insert(edge.second).second)
				++indegree.at(edge.second);
		}

		// A complete import has no trusted topological order, so it requires one
		// full Kahn pass. Kahn emits fewer than V nodes exactly when a cycle exists.
		newOrder = KahnSort(nodes, newOutgoing, indegree, originalPosition);
		if (newOrder.size() != nodes.size())
			return false;

		PositionMap newPosition;
		for (std::size_t index = 0; index < newOrder.size(); ++index)
			newPosition.emplace(newOrder[index], index);

		// Validation succeeded; atomically commit the newly built graph state.
		m_order = std::move(newOrder);
		m_position = std::move(newPosition);
		m_outgoing = std::move(newOutgoing);
		return true;
	}

	void Clear()
	{
		m_order.clear();
		m_position.clear();
		m_outgoing.clear();
	}

	bool ContainsNode(const Node& node) const
	{
		return m_position.find(node) != m_position.end();
	}

	bool ContainsEdge(const Node& source, const Node& destination) const
	{
		const auto sourceEdges = m_outgoing.find(source);
		return sourceEdges != m_outgoing.end() &&
			sourceEdges->second.find(destination) != sourceEdges->second.end();
	}

	std::size_t Position(const Node& node) const
	{
		return m_position.at(node);
	}

	const std::vector<Node>& Order() const
	{
		return m_order;
	}

private:
	using NodeSet = std::unordered_set<Node, Hash, Equal>;
	using PositionMap = std::unordered_map<Node, std::size_t, Hash, Equal>;
	using OutgoingMap = std::unordered_map<Node, NodeSet, Hash, Equal>;
	using DegreeMap = std::unordered_map<Node, std::size_t, Hash, Equal>;

	struct QueueEntry
	{
		std::size_t oldPosition;
		Node node;
	};

	struct QueueEntryGreater
	{
		bool operator()(const QueueEntry& left, const QueueEntry& right) const
		{
			return left.oldPosition > right.oldPosition;
		}
	};

	std::vector<Node> SortSliceWithEdge(
		std::size_t left,
		std::size_t right,
		const Node& proposedSource,
		const Node& proposedDestination) const
	{
		// Construct the induced subgraph for order[left..right]. Edges to nodes
		// outside the slice cannot affect the relative order inside it: in the
		// current valid order, outside predecessors are before the slice and
		// outside successors are after it.
		std::vector<Node> slice(
			m_order.begin() + static_cast<std::ptrdiff_t>(left),
			m_order.begin() + static_cast<std::ptrdiff_t>(right + 1));
		NodeSet inside(slice.begin(), slice.end());
		OutgoingMap localOutgoing;
		DegreeMap indegree;

		for (const Node& node : slice)
		{
			localOutgoing.emplace(node, NodeSet{});
			indegree.emplace(node, 0);
		}

		for (const Node& source : slice)
		{
			for (const Node& destination : m_outgoing.at(source))
			{
				if (inside.find(destination) != inside.end() &&
					localOutgoing.at(source).insert(destination).second)
				{
					++indegree.at(destination);
				}
			}
		}

		// Test the proposed edge in the temporary subgraph only. The caller adds
		// it to m_outgoing after this slice has proved acyclic.
		if (localOutgoing.at(proposedSource).insert(proposedDestination).second)
			++indegree.at(proposedDestination);

		const std::vector<Node> result = KahnSort(slice, localOutgoing, indegree, m_position);
		// This slice always contains at least the two distinct endpoints, so an
		// empty vector can be used as an unambiguous cycle/failure result.
		return result.size() == slice.size() ? result : std::vector<Node>{};
	}

	static std::vector<Node> KahnSort(
		const std::vector<Node>& nodes,
		const OutgoingMap& outgoing,
		DegreeMap indegree,
		const PositionMap& oldPosition)
	{
		// Kahn's algorithm repeatedly emits a zero-indegree node. The priority
		// queue uses the previous position as a tie-breaker, making the result
		// stable: unrelated nodes move as little as possible.
		std::priority_queue<QueueEntry, std::vector<QueueEntry>, QueueEntryGreater> ready;
		for (const Node& node : nodes)
		{
			if (indegree.at(node) == 0)
				ready.push({ oldPosition.at(node), node });
		}

		std::vector<Node> result;
		result.reserve(nodes.size());
		while (!ready.empty())
		{
			const Node current = ready.top().node;
			ready.pop();
			result.push_back(current);

			// Removing current conceptually removes all of its outgoing edges.
			// A destination becomes ready when its last predecessor is removed.
			for (const Node& destination : outgoing.at(current))
			{
				std::size_t& destinationIndegree = indegree.at(destination);
				--destinationIndegree;
				if (destinationIndegree == 0)
					ready.push({ oldPosition.at(destination), destination });
			}
		}
		// In a cyclic graph, no node in the remaining cycle ever reaches zero
		// indegree, so result will contain fewer nodes than the input.
		return result;
	}

	std::vector<Node> m_order;
	PositionMap m_position;
	OutgoingMap m_outgoing;
	Equal m_equal;
};
