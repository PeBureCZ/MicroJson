#include "microJson.h"

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

	void JsonObject::pushArray(std::string key, const mJsonArray& arr)
	{
		pushValue(key, JsonValue(arr));
	}

	void JsonObject::moveArray(std::string key, mJsonArray&& arr)
	{
		pushValue(key, JsonValue(std::move(arr)));
	}

	void JsonObject::pushObject(std::string key, const JsonObject& obj)
	{
		pushValue(key, JsonValue(obj.getObject()));
	}

	void JsonObject::pushValue(std::string key, JsonValue value)
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

	void JsonRoot::pushValue(std::string key, JsonValue value)
	{
		moveValue(std::move(key), std::move(value));
	}

	void JsonRoot::moveValue(std::string key, JsonValue&& value)
	{
		root.emplace_back(BaseJsonItem(std::move(key), std::move(value)));
	}

	void JsonRoot::pushObject(std::string key, const JsonObject& obj)
	{
		root.emplace_back(BaseJsonItem(std::move(key), obj));
	}

	void JsonRoot::moveObject(std::string key, JsonObject&& obj)
	{
		root.emplace_back(BaseJsonItem(std::move(key), std::move(obj)));
	}

	void JsonRoot::pushArray(std::string key, const JsonArray& arr)
	{
		root.emplace_back(BaseJsonItem(std::move(key), arr));
	}

	void JsonRoot::moveArray(std::string key, JsonArray&& arr)
	{
		root.emplace_back(BaseJsonItem(std::move(key), std::move(arr)));
	}

	[[nodiscard]] std::string JsonRoot::serialize() const
	{
		std::string result = "{";

		for (const auto& basicItem : root)
		{
			if (!result.empty() && result.back() != '{')
				result += ",";

			result += "\"" + basicItem.getKey() + "\":";

			if (std::holds_alternative<JsonValue>(basicItem.getValue()))
			{
				const auto& value = std::get<JsonValue>(basicItem.getValue());
				result += value.serialize();
			}
			else if (std::holds_alternative<JsonObject>(basicItem.getValue()))
			{
				const auto& obj = std::get<JsonObject>(basicItem.getValue());

				result += "{";
				bool isFirst = true;
				for (const auto& [key, val] : obj.getObject())
				{
					if (!isFirst)
						result += ",";
					else
						isFirst = false;
					result += "\"" + key + "\":" + val.serialize();
				}
				result += "}";
			}
			else if (std::holds_alternative<JsonArray>(basicItem.getValue()))
			{
				const auto& arr = std::get<JsonArray>(basicItem.getValue());

				result += "[";
				bool isFirst = true;
				for (const auto& val : arr.getValues())
				{
					if (!isFirst)
						result += ",";
					else
						isFirst = false;
					result += val.serialize();
				}
				result += "]";
			}
		}
		return result += "}";
	}

	[[nodiscard]] std::optional<JsonRoot> JsonDeserializer::deserialize(const std::string& jsonString) noexcept
	{
		_ASSERT(false); // Not implemented yet
		return std::nullopt;
	}

	[[nodiscard]] std::optional<JsonRoot> JsonDeserializer::deserialize(const std::filesystem::path& jsonFilePath) noexcept
	{
		_ASSERT(false); // Not implemented yet
		return std::nullopt;
	}
}

