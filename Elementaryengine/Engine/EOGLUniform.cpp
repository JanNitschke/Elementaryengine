#include "EOGLUniform.h"

//template <class T>
//EOGLUniform<T>::EOGLUniform()
//{
//}
//
//template<class T>
//EOGLUniform<T>::EOGLUniform(Shader * shader, const std::string & name, T value)
//{
//	SetShader(shader);
//	SetName(name);
//	SetValue(value);
//}
//
//template <class T>
//EOGLUniform<T>::~EOGLUniform()
//{
//}
//
//template<class T>
//void EOGLUniform<T>::SetShader(Shader * shader)
//{
//	_shader = shader;
//}
//
//template<class T>
//void EOGLUniform<T>::Set()
//{
//	//OGLSetUniform(_shader, valPt, na, locationCache);
//}
//
//
//template<class T>
//void EOGLUniform<T>::SetValue(T value)
//{
//	_value = value;
//	_valuePt = &_value;
//}
//
//template<class T>
//void EOGLUniform<T>::SetValuePointer(T * valuePointer)
//{
//	_valuePt = valuePointer;
//}
//
//template<class T>
//void EOGLUniform<T>::SetName(const std::string & name)
//{
//	_name = name;
//}
//
//template<class T>
//T EOGLUniform<T>::GetValue()
//{
//	return _value;
//}
//
//template<class T>
//T * EOGLUniform<T>::GetValuePointer()
//{
//	return _valuePt;
//}
//
//template<>
//void EOGLUniform<int>::Set()
//{
//	if (locationCache < 0) {
//		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
//	}
//	_shader->setInt(locationCache, *_valuePt);
//}
//template<>
//void EOGLUniform<float>::Set()
//{
//	if (locationCache < 0) {
//		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
//	}
//	_shader->setFloat(locationCache, *_valuePt);
//}
//template<>
//void EOGLUniform<vec2>::Set()
//{
//	if (locationCache < 0) {
//		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
//	}
//	_shader->set2Float(locationCache, *_valuePt);
//}
//template<>
//void EOGLUniform<vec3>::Set()
//{
//	if (locationCache < 0) {
//		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
//	}
//	_shader->set3Float(locationCache, *_valuePt);
//}
//template<>
//void EOGLUniform<mat4>::Set()
//{
//	if (locationCache < 0) {
//		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
//	}
//	_shader->setMat4f(locationCache, *_valuePt);
//}
//template<>
//void EOGLUniform<bool>::Set()
//{
//	if (locationCache < 0) {
//		locationCache = glGetUniformLocation(_shader->ID, _name.c_str());
//	}
//	_shader->setBool(locationCache, *_valuePt);
//}