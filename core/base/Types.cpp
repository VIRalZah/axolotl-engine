#include "base/Types.h"
#include "support/StringUtils.h"

NS_AX_BEGIN

// Vec2

Vec2::Vec2()
{
    setVec2(0.0f, 0.0f);
}

Vec2::Vec2(float x, float y) : x(x), y(y)
{
}

Vec2::Vec2(const Vec2& other) : x(other.x), y(other.y)
{
}

Vec2::Vec2(const Size& size) : x(size.width), y(size.height)
{
}

Vec2& Vec2::operator= (const Vec2& other)
{
    setVec2(other.x, other.y);
    return *this;
}

Vec2& Vec2::operator= (const Size& size)
{
    setVec2(size.width, size.height);
    return *this;
}

Vec2 Vec2::operator+(const Vec2& right) const
{
    return Vec2(this->x + right.x, this->y + right.y);
}

Vec2 Vec2::operator-(const Vec2& right) const
{
    return Vec2(this->x - right.x, this->y - right.y);
}

Vec2 Vec2::operator-() const
{
    return Vec2(-x, -y);
}

Vec2 Vec2::operator*(float a) const
{
    return Vec2(this->x * a, this->y * a);
}

Vec2 Vec2::operator/(float a) const
{
    AXAssert(a, "Point division by 0.");
    return Vec2(this->x / a, this->y / a);
}

void Vec2::setVec2(float x, float y)
{
    this->x = x;
    this->y = y;
}

bool Vec2::equals(const Vec2& target) const
{
    return (fabs(this->x - target.x) < FLT_EPSILON)
        && (fabs(this->y - target.y) < FLT_EPSILON);
}

bool Vec2::fuzzyEquals(const Vec2& b, float var) const
{
    if (x - var <= b.x && b.x <= x + var)
        if (y - var <= b.y && b.y <= y + var)
            return true;
    return false;
}

float Vec2::getAngle(const Vec2& other) const
{
    Vec2 a2 = normalize();
    Vec2 b2 = other.normalize();
    float angle = atan2f(a2.cross(b2), a2.dot(b2));
    if (fabs(angle) < FLT_EPSILON) return 0.f;
    return angle;
}

Vec2 Vec2::rotateByAngle(const Vec2& pivot, float angle) const
{
    return pivot + (*this - pivot).rotate(Vec2::forAngle(angle));
}

const Vec2 Vec2::ZERO = Vec2();

// Size

Size::Size(void) : width(0), height(0)
{
}

Size::Size(float width, float height) : width(width), height(height)
{
}

Size::Size(const Size& other) : width(other.width), height(other.height)
{
}

Size::Size(const Vec2& point) : width(point.x), height(point.y)
{
}

Size& Size::operator= (const Size& other)
{
    setSize(other.width, other.height);
    return *this;
}

Size& Size::operator= (const Vec2& point)
{
    setSize(point.x, point.y);
    return *this;
}

Size Size::operator+(const Size& right) const
{
    return Size(this->width + right.width, this->height + right.height);
}

Size Size::operator-(const Size& right) const
{
    return Size(this->width - right.width, this->height - right.height);
}

Size Size::operator-() const
{
    return Size(-width, -height);
}

Size Size::operator*(float a) const
{
    return Size(this->width * a, this->height * a);
}

Size Size::operator/(float a) const
{
    AXAssert(a, "Size division by 0.");
    return Size(this->width / a, this->height / a);
}

void Size::setSize(float width, float height)
{
    this->width = width;
    this->height = height;
}

bool Size::equals(const Size& target) const
{
    return (fabs(this->width - target.width) < FLT_EPSILON)
        && (fabs(this->height - target.height) < FLT_EPSILON);
}

const Size Size::ZERO = Size();

// Rect

Rect::Rect(void)
{
    setRect(0.0f, 0.0f, 0.0f, 0.0f);
}

Rect::Rect(float x, float y, float width, float height)
{
    setRect(x, y, width, height);
}

Rect::Rect(const Rect& other)
{
    setRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
}

Rect& Rect::operator= (const Rect& other)
{
    setRect(other.origin.x, other.origin.y, other.size.width, other.size.height);
    return *this;
}

void Rect::setRect(float x, float y, float width, float height)
{
    // CGRect can support width<0 or height<0
    // AXAssert(width >= 0.0f && height >= 0.0f, "width and height of Rect must not less than 0.");

    origin.x = x;
    origin.y = y;

    size.width = width;
    size.height = height;
}

bool Rect::equals(const Rect& rect) const
{
    return (origin.equals(rect.origin) &&
        size.equals(rect.size));
}

float Rect::getMaxX() const
{
    return (float)(origin.x + size.width);
}

float Rect::getMidX() const
{
    return (float)(origin.x + size.width / 2.0);
}

float Rect::getMinX() const
{
    return origin.x;
}

float Rect::getMaxY() const
{
    return origin.y + size.height;
}

float Rect::getMidY() const
{
    return (float)(origin.y + size.height / 2.0);
}

float Rect::getMinY() const
{
    return origin.y;
}

bool Rect::containsPoint(const Vec2& point) const
{
    if (point.x >= getMinX() && point.x <= getMaxX()
        && point.y >= getMinY() && point.y <= getMaxY())
    {
        return true;
    }

    return false;
}

bool Rect::intersectsRect(const Rect& rect) const
{
    return !(getMaxX() < rect.getMinX() ||
        rect.getMaxX() < getMinX() ||
        getMaxY() < rect.getMinY() ||
        rect.getMaxY() < getMinY());
}

const Rect Rect::ZERO = Rect();

// Version

Version::Version(int major, int minor, int revision)
{
	setVersion(major, minor, revision);
}

Version::Version(const Version& other)
{
	setVersion(other.major, other.minor, other.revision);
}

Version& Version::operator=(const Version& other)
{
	setVersion(other.major, other.minor, other.revision);
	return *this;
}

void Version::setVersion(int major, int minor, int revision)
{
	this->major = major;
	this->minor = minor;
	this->revision = revision;
}

std::string Version::toString()
{
	return StringUtils::format("%d.%d.%d", major, minor, revision);
}

NS_AX_END