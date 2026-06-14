#pragma once

#include <utility>
#include <unordered_map>
#include <string>
#include <variant>
#include <optional>
#include <vector>
#include <memory>
#include <filesystem>


namespace mjs
{

	enum class JsonType : int
	{
		Undefined = -1, //error type only, not a valid JSON type
		Null,
		Boolean,
		Number_double,
		Number_int,
		String,
		Array,
		Object
	};

	class JsonValue;
	class JsonArray;

	using mJsonObject = std::unordered_map<std::string, JsonValue>;
	using mJsonArray = std::vector<JsonValue>;

	using mJsonValue = std::variant
		<
			std::nullptr_t,
			bool,
			int64_t,
			double,
			std::string,
			mJsonArray,
			mJsonObject
		>;

	class JsonValue
	{
	public:
		JsonValue() = default;
		JsonValue(const JsonValue& other) = default;
		JsonValue(JsonValue&& other) = default;

		explicit JsonValue(bool b);
		explicit JsonValue(int i);
		explicit JsonValue(int64_t i);
		explicit JsonValue(double d);
		explicit JsonValue(std::string s);
		explicit JsonValue(const char* s);
		JsonValue(const mJsonArray& arr);
		JsonValue(const mJsonObject& obj);
		JsonValue(mJsonObject&& obj);

		JsonValue& operator=(const JsonValue& other) = default;
		JsonValue& operator=(JsonValue&& other) = default;

		bool operator==(const JsonValue& other) const
		{
			return type == other.type && value == other.value;
		}

		[[nodiscard]] bool isNull() const { return type == JsonType::Null; }
		[[nodiscard]] bool isBoolean() const { return type == JsonType::Boolean; }
		[[nodiscard]] bool isNumber() const { return type == JsonType::Number_double || type == JsonType::Number_int; }
		[[nodiscard]] bool isInt() const { return type == JsonType::Number_int; }
		[[nodiscard]] bool isDouble() const { return type == JsonType::Number_double; }
		[[nodiscard]] bool isString() const { return type == JsonType::String; }
		[[nodiscard]] bool isObject() const { return type == JsonType::Object; }
		[[nodiscard]] bool isArray() const { return type == JsonType::Array; }

		[[nodiscard]] JsonType getType() const { return type; }

		//[[nodiscard]] ValueType getValue() const { return value; }
		[[nodiscard]] bool asBoolean() const noexcept;
		[[nodiscard]] int64_t asInt() const noexcept;
		[[nodiscard]] double asDouble() const noexcept;
		[[nodiscard]] const std::string& asString() const noexcept;
		[[nodiscard]] std::string copyString() const noexcept;

		void setValue(int64_t newValue);
		void setValue(double newValue);
		void setValue(const std::string& newValue);
		void setValue(const char* newValue);
		void setValue(bool newValue);
		void setValue(std::nullptr_t nullValue);

		[[nodiscard]] std::string serialize() const;

	private:
		mJsonValue value;
		JsonType type = JsonType::Null;
	};

	class JsonObject
	{
	public:
		JsonObject() = default;

		const mJsonObject& getObject() const { return m_object; };

		void pushValue(std::string key, const JsonValue& value);
		void pushValue(std::string key, bool value);
		void pushValue(std::string key, int value);
		void pushValue(std::string key, int64_t value);
		void pushValue(std::string key, double value);
		void pushValue(std::string key, std::string value);
		void moveValue(std::string key, JsonValue&& value);

		void pushArray(std::string key, const mJsonArray& arr);
		void moveArray(std::string key, mJsonArray&& arr);

		void pushObject(std::string key, const mJsonObject& obj);
		void moveObject(std::string key, mJsonObject&& obj);

		[[nodiscard]] std::string serialize() const;

		mJsonObject move();

	private:
		mJsonObject m_object;
	};

	class JsonArray
	{
	public:
		JsonArray() = default;
		const mJsonArray& getValues() const { return m_values; };

		void pushValue(JsonValue value);
		void pushValue(bool value);
		void pushValue(int value);
		void pushValue(int64_t value);
		void pushValue(double value);
		void pushValue(const std::string& value);

		mJsonArray move();

	private:
		std::vector<JsonValue> m_values;
	};

	using mJsonBaseItem = std::variant<JsonValue, JsonArray, JsonObject>;
	using mJsonItem = std::pair<std::string, mJsonBaseItem>; //key-value pair

	class BaseJsonItem
	{
	public:
		BaseJsonItem() = default;
		BaseJsonItem(std::string key, mJsonBaseItem value) : item(std::move(key), std::move(value)) {}
		void setKey(std::string newKey) { item.first = std::move(newKey); }
		const std::string& getKey() const { return item.first; };
		[[nodiscard]] bool hasKey() const { return !item.first.empty(); };

		const mJsonBaseItem& getValue() const { return item.second; }

	private:
		mJsonItem item;
	};

	class JsonSerializer
	{
	public:
		static [[nodiscard]] bool serialize(const std::string& directoryPath, const std::string& fileName, const JsonObject& root) noexcept;
	};

	class JsonParser
	{
	public:
		explicit JsonParser(std::string_view text)
			: m_text(text)
		{
		}

		[[nodiscard]] std::optional<JsonObject> parse() noexcept;

	private:

		//return next char without consuming it, return '\0' if end of text
		char peek() const noexcept;

		//consume and return next char, return '\0' if end of text
		char get() noexcept;

		//if next char matches c, consume it and return true, otherwise return false without consuming
		bool match(char c) noexcept;

		//shift to next non-whitespace character
		void skipWhitespace() noexcept;

		[[nodiscard]] bool parseObject(JsonObject& obj) noexcept;
		[[nodiscard]] bool parseValue(JsonValue& value) noexcept;
		[[nodiscard]] bool parseArray(JsonValue& value) noexcept;
		[[nodiscard]] bool parseString(std::string& out) noexcept;
		[[nodiscard]] bool parseBool(JsonValue& value) noexcept;
		[[nodiscard]] bool parseNull(JsonValue& value) noexcept;
		[[nodiscard]] bool parseNumber(JsonValue& value) noexcept;


	private:
		std::string_view m_text;
		size_t m_pos = 0;
	};
}
