#include "microJson.h"

#include <fstream>
#include <istream>

namespace mjs
{
	JsonValue::JsonValue(bool b)
	{
		value = b;
		type = JsonType::Boolean;
	}

	JsonValue::JsonValue(int i)
	{
		value = static_cast<int64_t>(i);
		type = JsonType::Number_int;
	}

	JsonValue::JsonValue(int64_t i)
	{
		value = i;
		type = JsonType::Number_int;
	}

	JsonValue::JsonValue(double d)
	{
		value = d;
		type = JsonType::Number_double;
	}

	JsonValue::JsonValue(std::string s)
	{
		value = std::move(s);
		type = JsonType::String;
	}

	JsonValue::JsonValue(const char* s)
	{
		value = s;
		type = JsonType::String;
	}

	JsonValue::JsonValue(const mJsonArray& arr)
	{
		value = arr;
		type = JsonType::Array;
	}

	JsonValue::JsonValue(const mJsonObject& obj)
	{
		value = obj;
		type = JsonType::Object;
	}

	JsonValue::JsonValue(mJsonObject&& obj)
	{
		value = std::move(obj);
		type = JsonType::Object;
	}

	bool JsonValue::asBoolean() const noexcept
	{
		if (isBoolean())
			return std::get<bool>(value);
		else
		{
			_ASSERT(false);
			return false;
		}
	}

	int64_t JsonValue::asInt() const noexcept
	{
		if (isInt())
			return std::get<int64_t>(value);
		else
		{
			_ASSERT(false);
			return 0;
		}
	}

	double JsonValue::asDouble() const noexcept
	{
		if (isDouble())
			return std::get<double>(value);
		else
		{
			_ASSERT(false);
			return 0.0;
		}
	}

	const std::string& JsonValue::asString() const noexcept
	{
		if (isString())
			return std::get<std::string>(value);
		else
		{
			_ASSERT(false);
			static const std::string emptyString;
			return emptyString;
		}
	}

	std::string JsonValue::copyString() const noexcept
	{
		if (isString())
			return std::get<std::string>(value);
		else
		{
			_ASSERT(false);
			return std::string();
		}
	}

	void JsonValue::setValue(int64_t newValue)
	{
		value = newValue;
		type = JsonType::Number_int;
	}

	void JsonValue::setValue(double newValue)
	{
		value = newValue;
		type = JsonType::Number_double;
	}

	void JsonValue::setValue(const std::string& newValue)
	{
		value = newValue;
		type = JsonType::String;
	}

	void JsonValue::setValue(const char* newValue)
	{
		value = std::string(newValue);
		type = JsonType::String;
	}

	void JsonValue::setValue(bool newValue)
	{
		value = newValue;
		type = JsonType::Boolean;
	}

	void JsonValue::setValue(std::nullptr_t nullValue)
	{
		value = nullptr;
		type = JsonType::Null;
	}

	[[nodiscard]] std::string JsonValue::serialize() const
	{
		std::string result;
		switch (type)
		{
		case JsonType::Null:
			result += "null";
			break;
		case JsonType::Boolean:
			result += asBoolean() ? "true" : "false";
			break;
		case JsonType::Number_int:
			result += std::to_string(asInt());
			break;
		case JsonType::Number_double:
			result += std::to_string(asDouble());
			break;
		case JsonType::String:
			result += "\"" + asString() + "\"";
			break;
		case JsonType::Array:
		{
			const auto& array = std::get<mJsonArray>(value);
			_ASSERT(!array.empty());
			if (array.empty())
				break;

			result += "["; 
			bool first = true;
			for (const auto& item : array)
			{
				if (!first)
					result += ",";
				first = false;
				result += item.serialize();
			}
			result += "]";
			break;
		}
		case JsonType::Object:
		{
			const auto& object = std::get<mJsonObject>(value);
			result += "{";
			bool first = true;
			for (const auto& [key, val] : object)
			{
				if (!first)
					result += ",";
				first = false;
				result += "\"" + key + "\":" + val.serialize();
			}
			result += "}";
			break;
		}
		default:
			_ASSERT(false);
			result += "";
		}
		return result;
	}

	void JsonObject::pushValue(std::string key, bool value)
	{
		pushValue(key, JsonValue(value));
	}

	void JsonObject::pushValue(std::string key, int value)
	{
		pushValue(key, JsonValue(value));
	}

	void JsonObject::pushValue(std::string key, int64_t value)
	{
		pushValue(key, JsonValue(value));
	}

	void JsonObject::pushValue(std::string key, double value)
	{
		pushValue(key, JsonValue(value));
	}

	void JsonObject::pushValue(std::string key, std::string value)
	{
		pushValue(key, JsonValue(std::move(value)));
	}

	void JsonArray::pushValue(JsonValue value)
	{
		m_values.push_back(std::move(value));
	}

	void JsonArray::pushValue(bool value)
	{
		m_values.emplace_back(JsonValue(value));
	}

	void JsonArray::pushValue(int value)
	{
		m_values.emplace_back(JsonValue(value));
	}

	void JsonArray::pushValue(int64_t value)
	{
		m_values.emplace_back(JsonValue(value));
	}

	void JsonArray::pushValue(double value)
	{
		m_values.emplace_back(JsonValue(value));
	}

	void JsonArray::pushValue(const std::string& value)
	{
		m_values.emplace_back(JsonValue(value));
	}

	mJsonArray JsonArray::move()
	{
		return std::move(m_values);
	}

	void JsonObject::pushValue(std::string key, const JsonValue& value)
	{
		auto it = m_object.find(key);
		if (it != m_object.end())
		{
			_ASSERT(false); //duplicate key, overwriting value
			it->second = value;
		}
		else
			m_object.emplace(std::move(key), value);
	}

	void JsonObject::moveValue(std::string key, JsonValue&& value)
	{
		auto it = m_object.find(key);
		if (it != m_object.end())
		{
			_ASSERT(false); //duplicate key, overwriting value
			it->second = std::move(value);
		}
		else
			m_object.emplace(std::move(key), std::move(value));
	}

	void JsonObject::pushObject(std::string key, const mJsonObject& obj)
	{
		auto it = m_object.find(key);
		if (it != m_object.end())
		{
			_ASSERT(false); //duplicate key, overwriting value
			it->second = obj;
		}
		else
			m_object.emplace(std::move(key), obj);
	}

	void JsonObject::moveObject(std::string key, mJsonObject&& obj)
	{
		auto it = m_object.find(key);
		if (it != m_object.end())
		{
			_ASSERT(false); //duplicate key, overwriting value
			it->second = std::move(obj);
		}
		else
			m_object.emplace(std::move(key), std::move(obj));
	}

	void JsonObject::pushArray(std::string key, const mJsonArray& arr)
	{
		auto it = m_object.find(key);
		if (it != m_object.end())
		{
			_ASSERT(false); //duplicate key, overwriting value
			it->second = arr;
		}
		else
			m_object.emplace(std::move(key), arr);
	}

	void JsonObject::moveArray(std::string key, mJsonArray&& arr)
	{
		auto it = m_object.find(key);
		if (it != m_object.end())
		{
			_ASSERT(false); //duplicate key, overwriting value
			it->second = std::move(arr);
		}
		else
			m_object.emplace(std::move(key), std::move(arr));
	}

	[[nodiscard]] std::string JsonObject::serialize() const
	{
		std::string result = "{";

		for (const auto& [key, value] : m_object)
		{
			if (!result.empty() && result.back() != '{')
				result += ",";

			result += "\"" + key + "\":";
			result += value.serialize();
		}
		return result += "}";
	}

	mJsonObject JsonObject::move()
	{
		return std::move(m_object);
	}

	[[nodiscard]] bool JsonSerializer::serialize(const std::string& directoryPath, const std::string& fileName, const JsonObject& root) noexcept
	{
		namespace fs = std::filesystem;
		fs::path dirPath(directoryPath);
		if (!fs::exists(dirPath) || !fs::is_directory(dirPath) || fileName.empty())
			return false;

		fs::path filePath = dirPath / fileName;
		std::ofstream file(filePath);

		if (!file.is_open())
			return false;

		file << root.serialize();
		file.close();
		return true;
	}

	[[nodiscard]] std::optional<JsonObject> JsonParser::parse() noexcept
	{
		skipWhitespace();

		if (peek() != '{')
			return std::nullopt;

		JsonObject obj;
		if (!parseObject(obj))
			return std::nullopt;

		skipWhitespace();

		return obj;
	}

	char JsonParser::peek() const noexcept
	{
		return m_pos < m_text.size()
			? m_text[m_pos]
			: '\0';
	}

	char JsonParser::get() noexcept
	{
		return m_pos < m_text.size()
			? m_text[m_pos++]
			: '\0';
	}

	bool JsonParser::match(char c) noexcept
	{
		if (peek() == c)
		{
			++m_pos;
			return true;
		}
		return false;
	}

	void JsonParser::skipWhitespace() noexcept
	{
		while (std::isspace(static_cast<unsigned char>(peek())))
			++m_pos;
	}

	[[nodiscard]] bool JsonParser::parseObject(JsonObject& obj) noexcept
	{
		if (!match('{'))
			return false;

		skipWhitespace();

		if (match('}'))
			return true; // empty object

		while (true)
		{
			skipWhitespace();

			std::string key;
			if (!parseString(key))
				return false;

			skipWhitespace();

			if (!match(':'))
				return false;

			skipWhitespace();

			JsonValue value;
			if (!parseValue(value))
				return false;

			obj.moveValue(std::move(key), std::move(value));

			skipWhitespace();

			if (match('}'))
				break;

			if (!match(','))
				return false;
		}

		return true;
	}

	[[nodiscard]] bool JsonParser::parseValue(JsonValue& value) noexcept
	{
		skipWhitespace();

		if (peek() == '"')
		{
			std::string s;
			if (!parseString(s))
				return false;

			value = JsonValue(std::move(s));
			return true;
		}
		else if (std::isdigit(peek()) || peek() == '-')
		{
			return parseNumber(value);
		}
		else if (peek() == '{')
		{
			JsonObject obj;
			if (!parseObject(obj))
				return false;

			value = JsonValue(obj.move());
			return true;
		}
		else if (peek() == '[')
		{
			return parseArray(value);
		}
		else if (peek() == 't' || peek() == 'f')
		{
			return parseBool(value);
		}
		else if (peek() == 'n')
		{
			return parseNull(value);
		}
		else
		{
			_ASSERT(false); //unhandled or error value
		}

		return false;
	}

	[[nodiscard]] bool JsonParser::parseArray(JsonValue& value) noexcept
	{
		match('[');

		mJsonArray arr;

		skipWhitespace();

		if (match(']'))
		{
			value = JsonValue(arr);
			return true;
		}

		while (true)
		{
			JsonValue elem;
			if (!parseValue(elem))
				return false;

			arr.push_back(std::move(elem));

			skipWhitespace();

			if (match(']'))
				break;

			if (!match(','))
				return false;
		}

		value = JsonValue(arr);
		return true;
	}

	[[nodiscard]] bool JsonParser::parseString(std::string& out) noexcept
	{
		if (!match('"'))
			return false;

		out.clear();

		while (peek() != '"' && peek() != '\0')
		{
			char c = get();

			// minimal escape support
			if (c == '\\')
			{
				char next = get();
				switch (next)
				{
				case '"': out.push_back('"'); break;
				case '\\': out.push_back('\\'); break;
				case '/': out.push_back('/'); break;
				case 'b': out.push_back('\b'); break;
				case 'f': out.push_back('\f'); break;
				case 'n': out.push_back('\n'); break;
				case 'r': out.push_back('\r'); break;
				case 't': out.push_back('\t'); break;
				default: return false;
				}
			}
			else
				out.push_back(c);
		}

		return match('"');
	}

	[[nodiscard]] bool JsonParser::parseBool(JsonValue& value) noexcept
	{
		if (m_text.substr(m_pos, 4) == "true")
		{
			m_pos += 4;
			value = JsonValue(true);
			return true;
		}

		if (m_text.substr(m_pos, 5) == "false")
		{
			m_pos += 5;
			value = JsonValue(false);
			return true;
		}

		return false;
	}

	[[nodiscard]] bool JsonParser::parseNull(JsonValue& value) noexcept
	{
		if (m_text.substr(m_pos, 4) == "null")
		{
			m_pos += 4;
			value = JsonValue(nullptr);
			return true;
		}

		return false;
	}


	[[nodiscard]] bool JsonParser::parseNumber(JsonValue& value) noexcept
	{
		size_t start = m_pos;

		if (peek() == '-') get();

		while (std::isdigit(peek()))
			get();

		bool isDouble = false;

		if (peek() == '.')
		{
			isDouble = true;
			get();

			while (std::isdigit(peek()))
				get();
		}

		std::string numStr(m_text.substr(start, m_pos - start));

		try
		{
			if (isDouble)
				value = JsonValue(std::stod(numStr));
			else
				value = JsonValue((int64_t)std::stoll(numStr));
		}
		catch (const std::exception&)
		{
			_ASSERT(false); //invalid number format
			return false;
		}
		return true;
	}
}

