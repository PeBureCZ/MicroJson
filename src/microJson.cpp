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

	[[nodiscard]] const std::optional<mJsonObject>& JsonValue::asObject() const noexcept
	{
		if (isObject())
			return std::get<mJsonObject>(value);
		else
		{
			_ASSERT(false);
			static const std::optional<mJsonObject> emptyObject;
			return emptyObject;
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

	[[nodiscard]] std::string JsonObject::serialize(const bool makeLinesBetweenObjects) const
	{
		std::string result = "{";

		bool isFirst = true;

		if (makeLinesBetweenObjects)
			result += "\n";

		for (const auto& [key, value] : m_object)
		{
			if (!isFirst)
			{
				result += ",";
				if (makeLinesBetweenObjects)
					result += "\n";
			}
			else
				isFirst = false;

			result += "\"" + key + "\":";
			result += value.serialize();
		}

		if (makeLinesBetweenObjects)
			result += "\n";

		return result += "}";
	}

	mJsonObject JsonObject::move()
	{
		return std::move(m_object);
	}

	[[nodiscard]] bool JsonSerializer::serialize(const std::string& directoryPath, const std::string& fileName, const JsonObject& root, const bool makeLinesBetweenObjects) noexcept
	{
		namespace fs = std::filesystem;
		fs::path dirPath(directoryPath);
		if (!fs::exists(dirPath) || !fs::is_directory(dirPath) || fileName.empty())
			return false;

		fs::path filePath = dirPath / fileName;
		std::ofstream file(filePath);

		if (!file.is_open())
			return false;

		file << root.serialize(makeLinesBetweenObjects);
		file.close();
		return true;
	}

	[[nodiscard]] std::optional<JsonObject> JsonParser::parse(std::string_view text) noexcept
	{
		size_t charPos = 0;
		skipWhitespace(text, charPos);

		if (peek(text, charPos) != '{')
			return std::nullopt;

		JsonObject obj;
		if (!parseObject(text, obj, charPos))
			return std::nullopt;

		skipWhitespace(text, charPos);

		return obj;
	}

	char JsonParser::peek(std::string_view text, size_t& charPos) noexcept
	{
		return charPos < text.size()
			? text[charPos]
			: '\0';
	}

	char JsonParser::get(std::string_view text, size_t& charPos) noexcept
	{
		return charPos < text.size()
			? text[charPos++]
			: '\0';
	}

	bool JsonParser::match(std::string_view text, char c, size_t& charPos) noexcept
	{
		if (peek(text, charPos) == c)
		{
			++charPos;
			return true;
		}
		return false;
	}

	void JsonParser::skipWhitespace(std::string_view text, size_t& charPos) noexcept
	{
		while (std::isspace(static_cast<unsigned char>(peek(text, charPos))))
			++charPos;
	}

	[[nodiscard]] bool JsonParser::parseObject(std::string_view text, JsonObject& obj, size_t& charPos) noexcept
	{
		if (!match(text, '{', charPos))
			return false;

		skipWhitespace(text, charPos);

		if (match(text, '}', charPos))
			return true; // empty object

		while (true)
		{
			skipWhitespace(text, charPos);

			std::string key;
			if (!parseString(text, key, charPos))
				return false;

			skipWhitespace(text, charPos);

			if (!match(text, ':', charPos))
				return false;

			skipWhitespace(text, charPos);

			JsonValue value;
			if (!parseValue(text, value, charPos))
				return false;

			obj.moveValue(std::move(key), std::move(value));

			skipWhitespace(text, charPos);

			if (match(text, '}', charPos))
				break;

			if (!match(text, ',', charPos))
				return false;
		}

		return true;
	}

	[[nodiscard]] bool JsonParser::parseValue(std::string_view text, JsonValue& value, size_t& charPos) noexcept
	{
		skipWhitespace(text, charPos);

		if (peek(text, charPos) == '"')
		{
			std::string s;
			if (!parseString(text, s, charPos))
				return false;

			value = JsonValue(std::move(s));
			return true;
		}
		else if (std::isdigit(peek(text, charPos)) || peek(text, charPos) == '-')
		{
			return parseNumber(text, value, charPos);
		}
		else if (peek(text, charPos) == '{')
		{
			JsonObject obj;
			if (!parseObject(text, obj, charPos))
				return false;

			value = JsonValue(obj.move());
			return true;
		}
		else if (peek(text, charPos) == '[')
		{
			return parseArray(text, value, charPos);
		}
		else if (peek(text, charPos) == 't' || peek(text, charPos) == 'f')
		{
			return parseBool(text, value, charPos);
		}
		else if (peek(text, charPos) == 'n')
		{
			return parseNull(text, value, charPos);
		}
		else
		{
			_ASSERT(false); //unhandled or error value
		}

		return false;
	}

	[[nodiscard]] bool JsonParser::parseArray(std::string_view text, JsonValue& value, size_t& charPos) noexcept
	{
		match(text, '[', charPos);

		mJsonArray arr;

		skipWhitespace(text, charPos);

		if (match(text, ']', charPos))
		{
			value = JsonValue(arr);
			return true;
		}

		while (true)
		{
			JsonValue elem;
			if (!parseValue(text, elem, charPos))
				return false;

			arr.push_back(std::move(elem));

			skipWhitespace(text, charPos);

			if (match(text, ']', charPos))
				break;

			if (!match(text, ',', charPos))
				return false;
		}

		value = JsonValue(arr);
		return true;
	}

	[[nodiscard]] bool JsonParser::parseString(std::string_view text, std::string& out, size_t& charPos) noexcept
	{
		if (!match(text, '"', charPos))
			return false;

		out.clear();

		while (peek(text, charPos) != '"' && peek(text, charPos) != '\0')
		{
			char c = get(text, charPos);
			// minimal escape support
			if (c == '\\')
			{
				char next = get(text, charPos);
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

		return match(text, '"', charPos);
	}

	[[nodiscard]] bool JsonParser::parseBool(std::string_view text, JsonValue& value, size_t& charPos) noexcept
	{
		if (text.substr(charPos, 4) == "true")
		{
			charPos += 4;
			value = JsonValue(true);
			return true;
		}

		if (text.substr(charPos, 5) == "false")
		{
			charPos += 5;
			value = JsonValue(false);
			return true;
		}

		return false;
	}

	[[nodiscard]] bool JsonParser::parseNull(std::string_view text, JsonValue& value, size_t& charPos) noexcept
	{
		if (text.substr(charPos, 4) == "null")
		{
			charPos += 4;
			value = JsonValue(nullptr);
			return true;
		}

		return false;
	}


	[[nodiscard]] bool JsonParser::parseNumber(std::string_view text, JsonValue& value, size_t& charPos) noexcept
	{
		size_t start = charPos;

		if (peek(text, charPos) == '-') get(text, charPos);

		while (std::isdigit(peek(text, charPos)))
			get(text, charPos);

		bool isDouble = false;

		if (peek(text, charPos) == '.')
		{
			isDouble = true;
			get(text, charPos);

			while (std::isdigit(peek(text, charPos)))
				get(text, charPos);
		}

		std::string numStr(text.substr(start, charPos - start));

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

