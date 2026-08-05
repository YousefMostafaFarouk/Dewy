#include "CircuitSerializer.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace
{
	void Require(bool condition, const char* message)
	{
		if (!condition)
			throw std::runtime_error(message);
	}

	void WriteText(const std::filesystem::path& path, const std::string& text)
	{
		std::ofstream stream(path);
		stream << text;
	}

	void RequireRejected(const std::filesystem::path& path, const std::string& json, const char* message)
	{
		WriteText(path, json);
		bool rejected = false;
		try
		{
			CircuitSerializer::Load(path);
		}
		catch (const std::runtime_error&)
		{
			rejected = true;
		}
		Require(rejected, message);
	}
}

int main()
{
	const std::filesystem::path path =
		std::filesystem::temp_directory_path() / "dewy-circuit-serializer-test.json";

	try
	{
		CircuitDocument original;
		original.entities = {
			{ 10, "button", -2.0f, 1.25f, true },
			{ 20, "not", 0.0f, 1.25f, false },
			{ 30, "bulb", 2.0f, 1.25f, false }
		};
		original.connections = {
			{ 10, 1, 20, 0 },
			{ 20, 1, 30, 0 }
		};

		CircuitSerializer::Save(path, original);
		const CircuitDocument loaded = CircuitSerializer::Load(path);

		Require(loaded.version == 1, "version did not round-trip");
		Require(loaded.entities.size() == 3, "entities did not round-trip");
		Require(loaded.connections.size() == 2, "connections did not round-trip");
		Require(loaded.entities[0].type == "button", "entity type did not round-trip");
		Require(loaded.entities[0].state, "button state did not round-trip");
		Require(loaded.connections[1].toEntity == 30, "connection did not round-trip");

		RequireRejected(
			path,
			"{\"version\":1,\"entities\":[],\"connections\":[",
			"malformed JSON was accepted");

		RequireRejected(
			path,
			"{\"version\":1,\"entities\":["
			"{\"id\":0,\"type\":\"button\",\"x\":0,\"y\":0},"
			"{\"id\":0,\"type\":\"bulb\",\"x\":1,\"y\":0}],"
			"\"connections\":[]}",
			"duplicate entity IDs were accepted");

		RequireRejected(
			path,
			"{\"version\":1,\"entities\":["
			"{\"id\":0,\"type\":\"button\",\"x\":0,\"y\":0}],"
			"\"connections\":[{\"from_entity\":0,\"from_component\":1,"
			"\"to_entity\":99,\"to_component\":0}]}",
			"a connection to an unknown entity was accepted");

		RequireRejected(
			path,
			"{\"version\":1,\"entities\":["
			"{\"id\":0,\"type\":\"button\",\"x\":0,\"y\":0},"
			"{\"id\":1,\"type\":\"bulb\",\"x\":1,\"y\":0},"
			"{\"id\":2,\"type\":\"bulb\",\"x\":2,\"y\":0}],"
			"\"connections\":["
			"{\"from_entity\":0,\"from_component\":1,\"to_entity\":1,\"to_component\":0},"
			"{\"from_entity\":0,\"from_component\":1,\"to_entity\":2,\"to_component\":0}]}",
			"an output port connected twice was accepted");

		RequireRejected(
			path,
			"{\"version\":1,\"entities\":["
			"{\"id\":0,\"type\":\"not\",\"x\":0,\"y\":0},"
			"{\"id\":1,\"type\":\"not\",\"x\":1,\"y\":0}],"
			"\"connections\":["
			"{\"from_entity\":0,\"from_component\":1,\"to_entity\":1,\"to_component\":0},"
			"{\"from_entity\":1,\"from_component\":1,\"to_entity\":0,\"to_component\":0}]}",
			"cyclic circuit was accepted");

		std::filesystem::remove(path);
		std::cout << "Circuit serializer tests passed\n";
		return 0;
	}
	catch (const std::exception& error)
	{
		std::filesystem::remove(path);
		std::cerr << error.what() << '\n';
		return 1;
	}
}
