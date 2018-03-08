#include "EScriptRuntime.h"
#include <comdef.h>
#include <glm\glm.hpp>
jsc::context EScriptRuntime::ctx;
jsc::runtime EScriptRuntime::runtime;

using namespace glm;
EScriptRuntime::EScriptRuntime()
{
	try
	{
		// Create a runtime
		check(runtime.create(JsRuntimeAttributeNone));

		// Create a context
		check(ctx.create(runtime));

		// Make this a current context for this scope

		ExposeNative();

		loadScript(L"main.js");
	}
	catch (const jsc::exception &e)
	{
		std::wcout << L"Exception code: "s << e.code() << L"\r\n"s;
	}
	return;
}


EScriptRuntime::~EScriptRuntime()
{
}

void EScriptRuntime::loadScript(wstring path)
{
	jsc::scoped_context s(ctx);

	FILE *file;
	if (_wfopen_s(&file, path.c_str(), L"rb"))
	{
		fwprintf(stderr, L"chakrahost: unable to open file: %s.\n", path.c_str());
		return;
	}

	unsigned int current = ftell(file);
	fseek(file, 0, SEEK_END);
	unsigned int end = ftell(file);
	fseek(file, current, SEEK_SET);
	unsigned int lengthBytes = end - current;
	char *rawBytes = (char *)calloc(lengthBytes + 1, sizeof(char));

	if (rawBytes == nullptr)
	{
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return;
	}

	fread((void *)rawBytes, sizeof(char), lengthBytes, file);

	wchar_t *contents = (wchar_t *)calloc(lengthBytes + 1, sizeof(wchar_t));
	if (contents == nullptr)
	{
		free(rawBytes);
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return;
	}

	if (MultiByteToWideChar(CP_UTF8, 0, rawBytes, lengthBytes + 1, contents, lengthBytes + 1) == 0)
	{
		free(rawBytes);
		free(contents);
		fwprintf(stderr, L"chakrahost: fatal error.\n");
		return;
	}

	wstring sc = contents;
	free(rawBytes);
	free(contents);
	script = sc;

	jsc::RunScript(script.c_str(), 0, L"");

	JsValueRef vref;
	JsGetAndClearException(&vref);

	JsValueType jsType;
	JsGetValueType(vref, &jsType);
	return;
}

void EScriptRuntime::ExposeNative()
{
	jsc::scoped_context sc(ctx);

	jsc::value::global()[L"console"] =
		jsc::value::object().method<1>(L"log", [](const std::wstring &message)
	{
		std::wcout << message << L"\r\n";
	});

	auto obj = jsc::value::object()
		.method<1>(L"print", [](const std::wstring &message)
	{
		std::wcout << message << L"\r\n";
	});

	jsc::value::global()[L"vec3"] = 
		jsc::value::function<3>([] (const float x, const float y, const float z) {
			vec3* vec = new vec3(x,y,z);

	});
	//jsc::value::global()[L"log"] =
	//	jsc::value::function<1>([](const std::wstring &sval)
	//{
	//	std::wcout << "log: " << sval;

	//	// Return value to JavaScript
	//	return true;
	//});


}

void EScriptRuntime::Test()
{
	using namespace std::string_literals;

	jsc::runtime runtime;
	jsc::context ctx;
	try
	{
		// Create a runtime
		check(runtime.create(JsRuntimeAttributeNone));

		// Create a context
		check(ctx.create(runtime));

		// Make this a current context for this scope
		jsc::scoped_context sc(ctx);

		// JavaScript code
		auto script_body = LR"==(
function sum(arg1, arg2) {
	return arg1 + arg2;
}

function testExternalFunction() {
	external_function("string value", true, { a: 20, b: [ "a1", null, undefined ] });
}

function testExternalObject(obj) {
	obj.print("a: " + obj.a);
	obj.print("b: " + obj.b);
	obj.print("c: " + obj.c);

	// re-assign property (will cause property put accessor to be called)
	obj.c = 2;
	obj.print("c: " + obj.c);
}
)=="s;

		// Execute script
		auto result = jsc::RunScript(script_body.c_str(), 0, L"");

		// 1. Run JavaScript function sum to add two integer values
		std::wcout << jsc::value::global()[L"sum"](nullptr, 2, 3).as_int() << L"\r\n"s;	// prints 5

																						// 2. Add an external function to a script
		jsc::value::global()[L"external_function"] =
			jsc::value::function<3>([](const std::wstring &sval, bool bval, jsc::value object)
		{
			std::wcout <<
				L"String argument: "s << sval <<
				L"\r\nBoolean argument: "s << bval <<
				L"\r\nInteger argument in object: "s << static_cast<int>(object[L"a"]) <<
				L"\r\nLength of JavaScript array: "s << object[L"b"][L"length"].as<int>() << L"\r\n"s;

			// Return value to JavaScript
			return true;
		});

		// 3. Run JavaScript function. Alternative call syntax
		jsc::value::global().call(L"testExternalFunction");	// will make a lambda above called

															// 4. Create a JavaScript object
		int c = 42;
		auto obj = jsc::value::object()
			.field(L"a", 10)	// constant property value
			.property(L"b", [] { return L"Read-only property"; })
			.property(L"c", [&] { return c; }, [&](int new_c) { c = new_c; })
			.method<1>(L"print", [](const std::wstring &message)
		{
			std::wcout << message << L"\r\n";
		});

		// 5. Run JavaScript function
		jsc::value::global().call(L"testExternalObject", obj);
	}
	catch (const jsc::exception &e)
	{
		std::wcout << L"Exception code: "s << e.code() << L"\r\n"s;
	}

}
