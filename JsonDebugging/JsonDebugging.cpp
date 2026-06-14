#include <iostream>
#include <string>

#include "..\include\microJson.h"

#include <vector>
#include <variant>
#include <memory>

int main()
{
	mjs::JsonObject root;

	mjs::JsonValue boolVal((bool)true);
	root.pushValue("par1", boolVal);
	root.pushValue("par2", false);
	root.pushValue("par3", mjs::JsonValue(3.744));
	root.pushValue("par4", mjs::JsonValue(3.0f));
	root.pushValue("par5", mjs::JsonValue("my name"));

	mjs::JsonArray arr;
	arr.pushValue(true);
	arr.pushValue(42);
	arr.pushValue(3.14);
	arr.pushValue("example arr string");
	root.pushArray("part - array", arr.getValues());

	mjs::JsonObject objInObj;
	objInObj.pushValue("objInObj - name", "Peter");
	objInObj.pushValue("objInObj - family_name", "Bures");

	mjs::JsonObject obj;
	obj.pushValue("obj - par1",false);
	obj.pushValue("obj - par2", 123);
	obj.pushValue("obj - par3", 3.14);
	obj.moveArray("obj - parArr", arr.move());
	obj.moveObject("obj - objInObj", objInObj.move());
	root.pushObject("part - object", obj.getObject());

	mjs::JsonSerializer::serialize("D:\\VS_projects\\MicroJson\\dLib", "output1.json", root);

	auto str = root.serialize();

	mjs::JsonParser parser(str);
	auto newObj_opt = parser.parse();

	if (newObj_opt.has_value())
	{
		auto& newObj = newObj_opt.value();
		mjs::JsonSerializer::serialize("D:\\VS_projects\\MicroJson\\dLib", "output2.json", newObj);
	}
	else
	{
		_ASSERT(false);
	}

	std::cout << "serialisation completed";
}
