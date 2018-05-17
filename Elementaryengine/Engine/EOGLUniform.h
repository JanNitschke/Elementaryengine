#pragma once
#include <Shader.h>
#include <glm\glm.hpp>
#include <functional>
#include <utility>

using namespace std;
using namespace glm;

template <class T>
class EOGLUniform {
public:
	Shader * _shader;
	T _value;
	T* _valuePt;
	string _name;
	int locationCache;
	typedef T(*ValueFn)();

	//using _valueFunction = std::function<T()>;
	ValueFn _valueFunction = NULL;
	//EOGLUniform();

	//EOGLUniform(Shader * shader, const std::string & name, T value);

	//~EOGLUniform();

	//void SetShader(Shader * shader);

	//void SetValue(T value);

	//void SetValuePointer(T * valuePointer);

	//void SetName(const std::string & name);

	//T GetValue();

	//T * GetValuePointer();

	EOGLUniform()
	{
	}

	~EOGLUniform()
	{
	}

	void SetShader(Shader * shader)
	{
		_shader = shader;
	}

	void Set()
	{
		//OGLSetUniform(_shader, valPt, na, locationCache);
	}


	void SetValue(T value)
	{
		_value = value;
		_valuePt = &_value;
	}

	void SetValuePointer(T * valuePointer)
	{
		_valuePt = valuePointer;
	}

	void SetName(const std::string & name)
	{
		_name = name;
	}

	void Update(T value) {
		SetValue(value);
		Set();
	}
	void Update(T* valuePt) {
		SetValuePointer(valuePt);
		Set();
	}
	void Update() {
		if(_valueFunction != NULL)
			SetValue(_valueFunction());
		mat4 test = Game::Instance().Projection;
		Set();
	}
	T GetValue()
	{
		return _value;
	}

	T * GetValuePointer()
	{
		return _valuePt;
	}

	EOGLUniform(Shader * shader, const std::string & name, T value)
	{
		SetShader(shader);
		SetName(name);
		SetValue(value);
	}
	void SetValueFunction(ValueFn valueFunction) {
		_valueFunction = valueFunction;
	}
};


template<>
void EOGLUniform<int>::Set()
{
	if (locationCache < 0) {
		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
	}
	_shader->setInt(locationCache, _value);
}

template<>
void EOGLUniform<float>::Set()
{
	if (locationCache < 0) {
		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
	}
	_shader->setFloat(locationCache, _value);
}
template<>
void EOGLUniform<vec2>::Set()
{
	if (locationCache < 0) {
		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
	}
	_shader->set2Float(locationCache, _value);
}
template<>
void EOGLUniform<vec3>::Set()
{
	if (locationCache < 0) {
		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
	}
	_shader->set3Float(locationCache, _value);
}
template<>
void EOGLUniform<mat4>::Set()
{
	if (locationCache < 0) {
		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
	}
	_shader->setMat4f(locationCache, _value);
}
template<>
void EOGLUniform<bool>::Set()
{
	if (locationCache < 0) {
		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
	}
	_shader->setBool(locationCache, _value);
}
//#include "EOGLUniform.cpp"
