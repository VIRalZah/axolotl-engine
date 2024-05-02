/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2007      Scott Lembcke

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

/**
 @file
 based on Chipmunk cpArray.
 axArray is a faster alternative to NSMutableArray, it does pretty much the
 same thing (stores NSObjects and retains/releases them appropriately). It's
 faster because:
 - it uses a plain C interface so it doesn't incur Objective-c messaging overhead
 - it assumes you know what you're doing, so it doesn't spend time on safety checks
 (index out of bounds, required capacity etc.)
 - comparisons are done using pointer equality instead of isEqual

 There are 2 kind of functions:
 - axArray functions that manipulates objective-c objects (retain and release are performed)
 - axCArray functions that manipulates values like if they were standard C structures (no retain/release is performed)
 */

#ifndef AX_ARRAY_H
#define AX_ARRAY_H

#include "ccMacros.h"
#include "cocoa/Object.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>

NS_AX_BEGIN

#define AX_INVALID_INDEX 0xffffffff

// Easy integration
#define AXARRAY_DATA_FOREACH(__array__, __object__)															\
__object__=__array__->arr[0]; for(unsigned int i=0, num=__array__->num; i<num; i++, __object__=__array__->arr[i])	\


typedef struct _axArray {
	unsigned int num, max;
	Object** arr;
} axArray;

/** Allocates and initializes a new array with specified capacity */
axArray* axArrayNew(unsigned int capacity);

/** Frees array after removing all remaining objects. Silently ignores nil arr. */
void axArrayFree(axArray*& arr);

/** Doubles array capacity */
void axArrayDoubleCapacity(axArray *arr);

/** Increases array capacity such that max >= num + extra. */
void axArrayEnsureExtraCapacity(axArray *arr, unsigned int extra);

/** shrinks the array so the memory footprint corresponds with the number of items */
void axArrayShrink(axArray *arr);

/** Returns index of first occurrence of object, NSNotFound if object not found. */
unsigned int axArrayGetIndexOfObject(axArray *arr, Object* object);

/** Returns a Boolean value that indicates whether object is present in array. */
bool axArrayContainsObject(axArray *arr, Object* object);

/** Appends an object. Behavior undefined if array doesn't have enough capacity. */
void axArrayAppendObject(axArray *arr, Object* object);

/** Appends an object. Capacity of arr is increased if needed. */
void axArrayAppendObjectWithResize(axArray *arr, Object* object);

/** Appends objects from plusArr to arr. 
 Behavior undefined if arr doesn't have enough capacity. */
void axArrayAppendArray(axArray *arr, axArray *plusArr);

/** Appends objects from plusArr to arr. Capacity of arr is increased if needed. */
void axArrayAppendArrayWithResize(axArray *arr, axArray *plusArr);

/** Inserts an object at index */
void axArrayInsertObjectAtIndex(axArray *arr, Object* object, unsigned int index);

/** Swaps two objects */
void axArraySwapObjectsAtIndexes(axArray *arr, unsigned int index1, unsigned int index2);

/** Removes all objects from arr */
void axArrayRemoveAllObjects(axArray *arr);

/** Removes object at specified index and pushes back all subsequent objects.
 Behavior undefined if index outside [0, num-1]. */
void axArrayRemoveObjectAtIndex(axArray *arr, unsigned int index, bool bReleaseObj = true);

/** Removes object at specified index and fills the gap with the last object,
 thereby avoiding the need to push back subsequent objects.
 Behavior undefined if index outside [0, num-1]. */
void axArrayFastRemoveObjectAtIndex(axArray *arr, unsigned int index);

void axArrayFastRemoveObject(axArray *arr, Object* object);

/** Searches for the first occurrence of object and removes it. If object is not
 found the function has no effect. */
void axArrayRemoveObject(axArray *arr, Object* object, bool bReleaseObj = true);

/** Removes from arr all objects in minusArr. For each object in minusArr, the
 first matching instance in arr will be removed. */
void axArrayRemoveArray(axArray *arr, axArray *minusArr);

/** Removes from arr all objects in minusArr. For each object in minusArr, all
 matching instances in arr will be removed. */
void axArrayFullRemoveArray(axArray *arr, axArray *minusArr);

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
// #pragma mark -
// #pragma mark axCArray for Values (c structures)
#endif

typedef struct _axCArray {
    unsigned int num, max;
    void** arr;
} axCArray;

/** Allocates and initializes a new C array with specified capacity */
axCArray* axCArrayNew(unsigned int capacity);

/** Frees C array after removing all remaining values. Silently ignores nil arr. */
void axCArrayFree(axCArray *arr);

/** Doubles C array capacity */
void axCArrayDoubleCapacity(axCArray *arr);

/** Increases array capacity such that max >= num + extra. */
void axCArrayEnsureExtraCapacity(axCArray *arr, unsigned int extra);

/** Returns index of first occurrence of value, NSNotFound if value not found. */
unsigned int axCArrayGetIndexOfValue(axCArray *arr, void* value);

/** Returns a Boolean value that indicates whether value is present in the C array. */
bool axCArrayContainsValue(axCArray *arr, void* value);

/** Inserts a value at a certain position. Behavior undefined if array doesn't have enough capacity */
void axCArrayInsertValueAtIndex( axCArray *arr, void* value, unsigned int index);

/** Appends an value. Behavior undefined if array doesn't have enough capacity. */
void axCArrayAppendValue(axCArray *arr, void* value);

/** Appends an value. Capacity of arr is increased if needed. */
void axCArrayAppendValueWithResize(axCArray *arr, void* value);

/** Appends values from plusArr to arr. Behavior undefined if arr doesn't have
 enough capacity. */
void axCArrayAppendArray(axCArray *arr, axCArray *plusArr);

/** Appends values from plusArr to arr. Capacity of arr is increased if needed. */
void axCArrayAppendArrayWithResize(axCArray *arr, axCArray *plusArr);

/** Removes all values from arr */
void axCArrayRemoveAllValues(axCArray *arr);

/** Removes value at specified index and pushes back all subsequent values.
 Behavior undefined if index outside [0, num-1].
 @since v0.99.4
 */
void axCArrayRemoveValueAtIndex(axCArray *arr, unsigned int index);

/** Removes value at specified index and fills the gap with the last value,
 thereby avoiding the need to push back subsequent values.
 Behavior undefined if index outside [0, num-1].
 @since v0.99.4
 */
void axCArrayFastRemoveValueAtIndex(axCArray *arr, unsigned int index);

/** Searches for the first occurrence of value and removes it. If value is not found the function has no effect.
 @since v0.99.4
 */
void axCArrayRemoveValue(axCArray *arr, void* value);

/** Removes from arr all values in minusArr. For each Value in minusArr, the first matching instance in arr will be removed.
 @since v0.99.4
 */
void axCArrayRemoveArray(axCArray *arr, axCArray *minusArr);

/** Removes from arr all values in minusArr. For each value in minusArr, all matching instances in arr will be removed.
 @since v0.99.4
 */
void axCArrayFullRemoveArray(axCArray *arr, axCArray *minusArr);

NS_AX_END
	
#endif // AX_ARRAY_H
