#ifndef __AXINTEGER_H__
#define __AXINTEGER_H__

#include "Object.h"

NS_AX_BEGIN

/**
 * @addtogroup data_structures
 * @{
 * @js NA
 */

class AX_DLL Integer : public Object
{
public:
    Integer(int v)
        : m_nValue(v) {}
    int getValue() const {return m_nValue;}

    static Integer* create(int v)
    {
        Integer* pRet = new Integer(v);
        pRet->autorelease();
        return pRet;
    }

    /* override functions 
     * @lua NA
     */
    virtual void acceptVisitor(DataVisitor &visitor) { visitor.visit(this); }

private:
    int m_nValue;
};

// end of data_structure group
/// @}

NS_AX_END

#endif /* __AXINTEGER_H__ */
