#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

struct CircuitEntityRecord
{
	std::size_t id{};
	std::string type;
	float x{};
	float y{};
	bool state{};
};

struct CircuitConnectionRecord
{
	std::size_t fromEntity{};
	std::size_t fromComponent{};
	std::size_t toEntity{};
	std::size_t toComponent{};
};

struct CircuitDocument
{
	int version{ 1 };
	std::vector<CircuitEntityRecord> entities;
	std::vector<CircuitConnectionRecord> connections;
};

class CircuitSerializer
{
public:
	static void Save(const std::filesystem::path& path, const CircuitDocument& document);
	static CircuitDocument Load(const std::filesystem::path& path);
};
