#include <iostream>
#include <string>

#include "..\include\microJson.h"

#include <vector>
#include <variant>
#include <memory>

int main()
{
	mjs::JsonRoot root;

	mjs::JsonValue boolVal((bool)true);
	root.pushValue("par1", boolVal);
	root.pushValue("par3", mjs::JsonValue(3.744));
	root.pushValue("par4", mjs::JsonValue(3.0f));
	root.pushValue("par5", mjs::JsonValue("my name"));

	mjs::JsonArray arr;
	arr.pushValue(true);
	arr.pushValue(42);
	arr.pushValue(3.14);
	arr.pushValue("example arr string");
	root.pushArray("part - array", arr);

	mjs::JsonObject objInObj;
	objInObj.pushValue("objInObj - name", "Peter");
	objInObj.pushValue("objInObj - family_name", "Bures");

	mjs::JsonObject obj;
	obj.pushValue("obj - par1",false);
	obj.pushValue("obj - par2", 123);
	obj.pushValue("obj - par3", 3.14);
	obj.pushArray("obj - parArr", arr.getValues());
	obj.pushObject("obj - objInObj", std::move(objInObj));
	root.moveObject("part - object", std::move(obj));

	auto str = root.serialize();

	std::cout << str;
}
