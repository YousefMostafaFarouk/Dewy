#include "TopologicalOrder.h"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace
{
	void Require(bool condition, const char* message)
	{
		if (!condition)
			throw std::runtime_error(message);
	}
}

int main()
{
	try
	{
		TopologicalOrder<int> graph;
		for (int node = 0; node < 6; ++node)
			Require(graph.AddNode(node), "failed to add a new node");

		Require(!graph.AddNode(0), "duplicate node was accepted");
		Require(graph.Position(4) == 4, "new node was not appended");
		Require(!graph.TryAddEdge(2, 2), "self-loop was accepted");

		Require(graph.TryAddEdge(0, 2), "forward edge was rejected");
		Require(graph.TryAddEdge(1, 3), "second forward edge was rejected");
		Require(graph.Position(0) == 0 && graph.Position(2) == 2,
			"forward edge unnecessarily changed the order");

		Require(graph.TryAddEdge(4, 2), "safe backward edge was rejected");
		Require(graph.Position(4) < graph.Position(2),
			"safe backward edge did not reorder its endpoints");
		Require(graph.Position(0) < graph.Position(2),
			"reordering broke an existing edge");
		Require(graph.Position(5) == 5,
			"node after the affected slice was unnecessarily moved");

		TopologicalOrder<int> cycleGraph;
		Require(cycleGraph.Reset(
			{ 0, 1, 2, 3 },
			{ { 0, 1 }, { 1, 2 }, { 2, 3 } }),
			"valid graph reset failed");
		const std::vector<int> beforeRejectedEdge = cycleGraph.Order();
		Require(!cycleGraph.TryAddEdge(3, 1), "multi-node cycle was accepted");
		Require(cycleGraph.Order() == beforeRejectedEdge,
			"rejected edge changed the order");
		Require(!cycleGraph.ContainsEdge(3, 1),
			"rejected edge changed the graph");

		Require(!cycleGraph.Reset(
			{ 0, 1, 2 },
			{ { 0, 1 }, { 1, 2 }, { 2, 0 } }),
			"cyclic full graph was accepted");
		Require(cycleGraph.Order() == beforeRejectedEdge,
			"failed reset replaced the existing graph");

		Require(cycleGraph.RemoveEdge(1, 2), "existing edge was not removed");
		Require(cycleGraph.RemoveNode(1), "existing node was not removed");
		Require(!cycleGraph.ContainsNode(1), "removed node remains in the graph");

		std::cout << "Topological order tests passed\n";
		return 0;
	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << '\n';
		return 1;
	}
}
