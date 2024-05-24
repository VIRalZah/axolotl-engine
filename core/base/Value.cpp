#include "Value.h"
#include "support/StringUtils.h"

NS_AX_BEGIN

Value::Value()
	: _type(Value::Type::UNKNOWN)
{
}

Value::Value(const Value& other)
{
	_type = other._type;
	_string = other._string;
}

Value::Value(const int& v)
	: _type(Value::Type::INTEGER)
{
	_string = StringUtils::format("%d", v);
}

Value::Value(const unsigned int& v)
	: _type(Value::Type::UNSIGNED)
{
	_string = StringUtils::format("%u", v);
}

Value::Value(const float& v)
	: _type(Value::Type::FLOAT)
{
	_string = StringUtils::format("%f", v);
}

Value::Value(const double& v)
	: _type(Value::Type::DOUBLE)
{
	_string = StringUtils::format("%lf", v);
}

Value::Value(const bool& v)
	: _type(Value::Type::BOOL)
{
	_string = StringUtils::boolToString(v);
}

Value::Value(const char* v)
	: _type(Value::Type::STRING)
{
	_string = v;
}

Value::Value(const std::string& v)
	: _type(Value::Type::STRING)
{
	_string = v;
}

Value::~Value()
{
	reset();
}

Value& Value::operator=(const Value& other)
{
	_type = other._type;
	_string = other._string;
	return *this;
}

Value& Value::operator=(const int& v)
{
	_type = Value::Type::INTEGER;
	_string = StringUtils::format("%d", v);
	return *this;
}

Value& Value::operator=(const unsigned int& v)
{
	_type = Value::Type::UNSIGNED;
	_string = StringUtils::format("%u", v);
	return *this;
}

Value& Value::operator=(const float& v)
{
	_type = Value::Type::FLOAT;
	_string = StringUtils::format("%f", v);
	return *this;
}

Value& Value::operator=(const double& v)
{
	_type = Value::Type::DOUBLE;
	_string = StringUtils::format("%lf", v);
	return *this;
}

Value& Value::operator=(const bool& v)
{
	_type = Value::Type::BOOL;
	_string = StringUtils::boolToString(v);
	return *this;
}

Value& Value::operator=(const char* v)
{
	_type = Value::Type::STRING;
	_string = v;
	return *this;
}

Value& Value::operator=(const std::string& v)
{
	_type = Value::Type::STRING;
	_string = v;
	return *this;
}

void Value::reset()
{
	_type = Value::Type::UNKNOWN;
	_string.clear();
}

int Value::intValue() const
{
	int ret = 0;
	switch (_type)
	{
	case Value::Type::INTEGER:
	case Value::Type::UNSIGNED:
	case Value::Type::STRING:
		ret = atoi(_string.c_str());
		break;
	case Value::Type::FLOAT:
	case Value::Type::DOUBLE:
		ret = (int)atof(_string.c_str());
		break;
	case Value::Type::BOOL:
		ret = (int)StringUtils::stringToBool(_string);
		break;
	}
	return ret;
}

unsigned int Value::uintValue() const
{
	return (unsigned int)intValue();
}

float Value::floatValue() const
{
	float ret = 0.0f;
	switch (_type)
	{
	case Value::Type::INTEGER:
	case Value::Type::UNSIGNED:
	case Value::Type::STRING:
		ret = (float)atoi(_string.c_str());
		break;
	case Value::Type::FLOAT:
	case Value::Type::DOUBLE:
		ret = (float)atof(_string.c_str());
		break;
	case Value::Type::BOOL:
		ret = (float)StringUtils::stringToBool(_string);
		break;
	}
	return ret;
}

double Value::doubleValue() const
{
	float ret = 0.0;
	switch (_type)
	{
	case Value::Type::INTEGER:
	case Value::Type::UNSIGNED:
	case Value::Type::STRING:
		ret = (double)atoi(_string.c_str());
		break;
	case Value::Type::FLOAT:
	case Value::Type::DOUBLE:
		ret = atof(_string.c_str());
		break;
	case Value::Type::BOOL:
		ret = (double)StringUtils::stringToBool(_string);
		break;
	}
	return ret;
}

bool Value::boolValue() const
{
	bool ret = false;
	switch (_type)
	{
	case Value::Type::INTEGER:
	case Value::Type::UNSIGNED:
	case Value::Type::STRING:
		ret = atoi(_string.c_str()) == 1;
		break;
	case Value::Type::FLOAT:
	case Value::Type::DOUBLE:
		ret = atof(_string.c_str()) == 1.0;
		break;
	case Value::Type::BOOL:
		ret = StringUtils::stringToBool(_string);
		break;
	}
	return ret;
}

std::string Value::stringValue() const
{
	return _string;
}

void Value::acceptVisitor(DataVisitor& visitor)
{
	visitor.visit(this);
}

NS_AX_END