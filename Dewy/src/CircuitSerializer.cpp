#include "CircuitSerializer.h"
#include "TopologicalOrder.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace
{
	class JsonReader
	{
	public:
		explicit JsonReader(std::string source) : m_source(std::move(source)) {}

		CircuitDocument ParseDocument()
		{
			CircuitDocument document;
			bool hasVersion = false;
			bool hasEntities = false;
			bool hasConnections = false;

			Expect('{');
			if (!Consume('}'))
			{
				do
				{
					const std::string key = ParseString();
					Expect(':');

					if (key == "version")
					{
						document.version = static_cast<int>(ParseUnsignedInteger());
						hasVersion = true;
					}
					else if (key == "entities")
					{
						document.entities = ParseEntities();
						hasEntities = true;
					}
					else if (key == "connections")
					{
						document.connections = ParseConnections();
						hasConnections = true;
					}
					else
					{
						SkipValue();
					}
				} while (Consume(','));

				Expect('}');
			}

			SkipWhitespace();
			if (m_position != m_source.size())
				Fail("unexpected content after the root object");
			if (!hasVersion || !hasEntities || !hasConnections)
				Fail("the root object must contain version, entities, and connections");

			Validate(document);
			return document;
		}

	private:
		std::string m_source;
		std::size_t m_position{};

		[[noreturn]] void Fail(const std::string& message) const
		{
			throw std::runtime_error(
				"Invalid circuit JSON at byte " + std::to_string(m_position) + ": " + message);
		}

		void SkipWhitespace()
		{
			while (m_position < m_source.size())
			{
				const char character = m_source[m_position];
				if (character != ' ' && character != '\n' && character != '\r' && character != '\t')
					break;
				++m_position;
			}
		}

		bool Consume(char expected)
		{
			SkipWhitespace();
			if (m_position >= m_source.size() || m_source[m_position] != expected)
				return false;
			++m_position;
			return true;
		}

		void Expect(char expected)
		{
			if (!Consume(expected))
				Fail(std::string("expected '") + expected + "'");
		}

		bool ConsumeLiteral(std::string_view literal)
		{
			SkipWhitespace();
			if (m_source.compare(m_position, literal.size(), literal) != 0)
				return false;
			m_position += literal.size();
			return true;
		}

		std::string ParseString()
		{
			SkipWhitespace();
			if (m_position >= m_source.size() || m_source[m_position] != '"')
				Fail("expected a string");
			++m_position;

			std::string result;
			while (m_position < m_source.size())
			{
				const char character = m_source[m_position++];
				if (character == '"')
					return result;
				if (static_cast<unsigned char>(character) < 0x20)
					Fail("control characters are not allowed in strings");
				if (character != '\\')
				{
					result.push_back(character);
					continue;
				}

				if (m_position >= m_source.size())
					Fail("unterminated escape sequence");
				switch (m_source[m_position++])
				{
				case '"': result.push_back('"'); break;
				case '\\': result.push_back('\\'); break;
				case '/': result.push_back('/'); break;
				case 'b': result.push_back('\b'); break;
				case 'f': result.push_back('\f'); break;
				case 'n': result.push_back('\n'); break;
				case 'r': result.push_back('\r'); break;
				case 't': result.push_back('\t'); break;
				default: Fail("unsupported string escape");
				}
			}

			Fail("unterminated string");
		}

		double ParseNumber()
		{
			SkipWhitespace();
			const std::size_t start = m_position;
			if (m_position < m_source.size() && m_source[m_position] == '-')
				++m_position;

			if (m_position >= m_source.size() || m_source[m_position] < '0' || m_source[m_position] > '9')
				Fail("expected a number");
			if (m_source[m_position] == '0')
				++m_position;
			else
				while (m_position < m_source.size() && m_source[m_position] >= '0' && m_source[m_position] <= '9')
					++m_position;

			if (m_position < m_source.size() && m_source[m_position] == '.')
			{
				++m_position;
				if (m_position >= m_source.size() || m_source[m_position] < '0' || m_source[m_position] > '9')
					Fail("expected digits after the decimal point");
				while (m_position < m_source.size() && m_source[m_position] >= '0' && m_source[m_position] <= '9')
					++m_position;
			}

			if (m_position < m_source.size() && (m_source[m_position] == 'e' || m_source[m_position] == 'E'))
			{
				++m_position;
				if (m_position < m_source.size() && (m_source[m_position] == '+' || m_source[m_position] == '-'))
					++m_position;
				if (m_position >= m_source.size() || m_source[m_position] < '0' || m_source[m_position] > '9')
					Fail("expected an exponent");
				while (m_position < m_source.size() && m_source[m_position] >= '0' && m_source[m_position] <= '9')
					++m_position;
			}

			const double value = std::stod(m_source.substr(start, m_position - start));
			if (!std::isfinite(value))
				Fail("numbers must be finite");
			return value;
		}

		std::size_t ParseUnsignedInteger()
		{
			const double value = ParseNumber();
			if (value < 0.0 || std::floor(value) != value ||
				value > static_cast<double>(std::numeric_limits<std::size_t>::max()))
				Fail("expected a non-negative integer");
			return static_cast<std::size_t>(value);
		}

		float ParseFloat()
		{
			const float value = static_cast<float>(ParseNumber());
			if (!std::isfinite(value))
				Fail("number is outside the supported coordinate range");
			return value;
		}

		bool ParseBoolean()
		{
			if (ConsumeLiteral("true"))
				return true;
			if (ConsumeLiteral("false"))
				return false;
			Fail("expected true or false");
		}

		std::vector<CircuitEntityRecord> ParseEntities()
		{
			std::vector<CircuitEntityRecord> entities;
			Expect('[');
			if (!Consume(']'))
			{
				do
				{
					entities.push_back(ParseEntity());
				} while (Consume(','));
				Expect(']');
			}
			return entities;
		}

		CircuitEntityRecord ParseEntity()
		{
			CircuitEntityRecord entity;
			bool hasId = false;
			bool hasType = false;
			bool hasX = false;
			bool hasY = false;

			Expect('{');
			if (!Consume('}'))
			{
				do
				{
					const std::string key = ParseString();
					Expect(':');
					if (key == "id") { entity.id = ParseUnsignedInteger(); hasId = true; }
					else if (key == "type") { entity.type = ParseString(); hasType = true; }
					else if (key == "x") { entity.x = ParseFloat(); hasX = true; }
					else if (key == "y") { entity.y = ParseFloat(); hasY = true; }
					else if (key == "state") { entity.state = ParseBoolean(); }
					else { SkipValue(); }
				} while (Consume(','));
				Expect('}');
			}

			if (!hasId || !hasType || !hasX || !hasY)
				Fail("each entity requires id, type, x, and y");
			return entity;
		}

		std::vector<CircuitConnectionRecord> ParseConnections()
		{
			std::vector<CircuitConnectionRecord> connections;
			Expect('[');
			if (!Consume(']'))
			{
				do
				{
					connections.push_back(ParseConnection());
				} while (Consume(','));
				Expect(']');
			}
			return connections;
		}

		CircuitConnectionRecord ParseConnection()
		{
			CircuitConnectionRecord connection;
			bool hasFromEntity = false;
			bool hasFromComponent = false;
			bool hasToEntity = false;
			bool hasToComponent = false;

			Expect('{');
			if (!Consume('}'))
			{
				do
				{
					const std::string key = ParseString();
					Expect(':');
					if (key == "from_entity") { connection.fromEntity = ParseUnsignedInteger(); hasFromEntity = true; }
					else if (key == "from_component") { connection.fromComponent = ParseUnsignedInteger(); hasFromComponent = true; }
					else if (key == "to_entity") { connection.toEntity = ParseUnsignedInteger(); hasToEntity = true; }
					else if (key == "to_component") { connection.toComponent = ParseUnsignedInteger(); hasToComponent = true; }
					else { SkipValue(); }
				} while (Consume(','));
				Expect('}');
			}

			if (!hasFromEntity || !hasFromComponent || !hasToEntity || !hasToComponent)
				Fail("each connection requires from_entity, from_component, to_entity, and to_component");
			return connection;
		}

		void SkipValue()
		{
			SkipWhitespace();
			if (m_position >= m_source.size())
				Fail("expected a value");

			if (m_source[m_position] == '"')
			{
				ParseString();
				return;
			}
			if (m_source[m_position] == '{')
			{
				Expect('{');
				if (!Consume('}'))
				{
					do { ParseString(); Expect(':'); SkipValue(); } while (Consume(','));
					Expect('}');
				}
				return;
			}
			if (m_source[m_position] == '[')
			{
				Expect('[');
				if (!Consume(']'))
				{
					do { SkipValue(); } while (Consume(','));
					Expect(']');
				}
				return;
			}
			if (ConsumeLiteral("true") || ConsumeLiteral("false") || ConsumeLiteral("null"))
				return;
			ParseNumber();
		}

		static std::size_t ComponentCount(const std::string& type)
		{
			if (type == "not") return 2;
			if (type == "or" || type == "and" || type == "xor") return 3;
			if (type == "bulb") return 1;
			if (type == "button") return 2;
			throw std::runtime_error("Unsupported circuit entity type: " + type);
		}

		static bool IsOutput(const std::string& type, std::size_t component)
		{
			if (type == "not") return component == 1;
			if (type == "or" || type == "and" || type == "xor") return component == 2;
			if (type == "button") return true;
			return false;
		}

		static void Validate(const CircuitDocument& document)
		{
			if (document.version != 1)
				throw std::runtime_error("Unsupported circuit file version: " + std::to_string(document.version));

			std::map<std::size_t, std::string> entityTypes;
			for (const CircuitEntityRecord& entity : document.entities)
			{
				ComponentCount(entity.type);
				if (!entityTypes.emplace(entity.id, entity.type).second)
					throw std::runtime_error("Duplicate entity id: " + std::to_string(entity.id));
			}

			std::set<std::pair<std::size_t, std::size_t>> usedOutputs;
			std::set<std::pair<std::size_t, std::size_t>> usedInputs;
			std::vector<std::pair<std::size_t, std::size_t>> graphEdges;
			for (const CircuitConnectionRecord& connection : document.connections)
			{
				const auto from = entityTypes.find(connection.fromEntity);
				const auto to = entityTypes.find(connection.toEntity);
				if (from == entityTypes.end() || to == entityTypes.end())
					throw std::runtime_error("A connection references an unknown entity id");
				if (connection.fromComponent >= ComponentCount(from->second) ||
					connection.toComponent >= ComponentCount(to->second))
					throw std::runtime_error("A connection references an unknown component index");
				if (!IsOutput(from->second, connection.fromComponent) ||
					IsOutput(to->second, connection.toComponent))
					throw std::runtime_error("Connections must run from an output component to an input component");
				if (!usedOutputs.emplace(connection.fromEntity, connection.fromComponent).second)
					throw std::runtime_error("An output component may only have one connection");
				if (!usedInputs.emplace(connection.toEntity, connection.toComponent).second)
					throw std::runtime_error("An input component may only have one connection");
				graphEdges.emplace_back(connection.fromEntity, connection.toEntity);
			}

			std::vector<std::size_t> graphNodes;
			graphNodes.reserve(document.entities.size());
			for (const CircuitEntityRecord& entity : document.entities)
				graphNodes.push_back(entity.id);

			TopologicalOrder<std::size_t> graphOrder;
			if (!graphOrder.Reset(graphNodes, graphEdges))
				throw std::runtime_error("Circuit connections contain a cycle");
		}
	};

	std::string EscapeJsonString(const std::string& value)
	{
		std::string escaped;
		for (const char character : value)
		{
			switch (character)
			{
			case '"': escaped += "\\\""; break;
			case '\\': escaped += "\\\\"; break;
			case '\n': escaped += "\\n"; break;
			case '\r': escaped += "\\r"; break;
			case '\t': escaped += "\\t"; break;
			default: escaped.push_back(character); break;
			}
		}
		return escaped;
	}
}

void CircuitSerializer::Save(const std::filesystem::path& path, const CircuitDocument& document)
{
	if (path.empty())
		throw std::runtime_error("Choose a circuit file path before saving");
	if (path.has_parent_path())
		std::filesystem::create_directories(path.parent_path());

	std::ofstream stream(path);
	if (!stream)
		throw std::runtime_error("Could not open circuit file for writing: " + path.string());

	stream << std::setprecision(std::numeric_limits<float>::max_digits10);
	stream << "{\n  \"version\": " << document.version << ",\n  \"entities\": [";
	for (std::size_t index = 0; index < document.entities.size(); ++index)
	{
		const CircuitEntityRecord& entity = document.entities[index];
		stream << (index == 0 ? "\n" : ",\n")
			<< "    {\"id\": " << entity.id
			<< ", \"type\": \"" << EscapeJsonString(entity.type)
			<< "\", \"x\": " << entity.x
			<< ", \"y\": " << entity.y
			<< ", \"state\": " << (entity.state ? "true" : "false") << "}";
	}
	stream << (document.entities.empty() ? "]" : "\n  ]") << ",\n  \"connections\": [";
	for (std::size_t index = 0; index < document.connections.size(); ++index)
	{
		const CircuitConnectionRecord& connection = document.connections[index];
		stream << (index == 0 ? "\n" : ",\n")
			<< "    {\"from_entity\": " << connection.fromEntity
			<< ", \"from_component\": " << connection.fromComponent
			<< ", \"to_entity\": " << connection.toEntity
			<< ", \"to_component\": " << connection.toComponent << "}";
	}
	stream << (document.connections.empty() ? "]" : "\n  ]") << "\n}\n";

	if (!stream)
		throw std::runtime_error("Failed while writing circuit file: " + path.string());
}

CircuitDocument CircuitSerializer::Load(const std::filesystem::path& path)
{
	if (path.empty())
		throw std::runtime_error("Choose a circuit file path before loading");

	std::ifstream stream(path);
	if (!stream)
		throw std::runtime_error("Could not open circuit file: " + path.string());

	std::ostringstream contents;
	contents << stream.rdbuf();
	if (!stream.good() && !stream.eof())
		throw std::runtime_error("Failed while reading circuit file: " + path.string());

	return JsonReader(contents.str()).ParseDocument();
}
