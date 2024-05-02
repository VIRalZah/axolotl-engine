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

#include "axCArray.h"
#include "cocoa/Object.h"

NS_AX_BEGIN

/** Allocates and initializes a new array with specified capacity */
axArray* axArrayNew(unsigned int capacity) 
{
	if (capacity == 0)
		capacity = 1;
	
	axArray *arr = (axArray*)malloc( sizeof(axArray) );
	arr->num = 0;
	arr->arr =  (Object**)calloc(capacity, sizeof(Object*));
	arr->max = capacity;
	
	return arr;
}

/** Frees array after removing all remaining objects. Silently ignores NULL arr. */
void axArrayFree(axArray*& arr)
{
    if( arr == NULL ) 
    {
        return;
    }
	axArrayRemoveAllObjects(arr);
	
	free(arr->arr);
	free(arr);

    arr = NULL;
}

void axArrayDoubleCapacity(axArray *arr)
{
	arr->max *= 2;
	Object** newArr = (Object**)realloc( arr->arr, arr->max * sizeof(Object*) );
	// will fail when there's not enough memory
    AXAssert(newArr != 0, "axArrayDoubleCapacity failed. Not enough memory");
	arr->arr = newArr;
}

void axArrayEnsureExtraCapacity(axArray *arr, unsigned int extra)
{
	while (arr->max < arr->num + extra)
    {
		axArrayDoubleCapacity(arr);
    }
}

void axArrayShrink(axArray *arr)
{
    unsigned int newSize = 0;
	
	//only resize when necessary
	if (arr->max > arr->num && !(arr->num==0 && arr->max==1))
	{
		if (arr->num!=0)
		{
			newSize=arr->num;
			arr->max=arr->num;
		}
		else
		{//minimum capacity of 1, with 0 elements the array would be free'd by realloc
			newSize=1;
			arr->max=1;
		}
		
		arr->arr = (Object**)realloc(arr->arr,newSize * sizeof(Object*) );
		AXAssert(arr->arr!=NULL,"could not reallocate the memory");
	}
}

/** Returns index of first occurrence of object, AX_INVALID_INDEX if object not found. */
unsigned int axArrayGetIndexOfObject(axArray *arr, Object* object)
{
    const unsigned int arrNum = arr->num;
    Object** ptr = arr->arr;
	for(unsigned int i = 0; i < arrNum; ++i, ++ptr)
    {
		if( *ptr == object ) return i;
    }
    
	return AX_INVALID_INDEX;
}

/** Returns a Boolean value that indicates whether object is present in array. */
bool axArrayContainsObject(axArray *arr, Object* object)
{
	return axArrayGetIndexOfObject(arr, object) != AX_INVALID_INDEX;
}

/** Appends an object. Behavior undefined if array doesn't have enough capacity. */
void axArrayAppendObject(axArray *arr, Object* object)
{
    AXAssert(object != NULL, "Invalid parameter!");
    object->retain();
	arr->arr[arr->num] = object;
	arr->num++;
}

/** Appends an object. Capacity of arr is increased if needed. */
void axArrayAppendObjectWithResize(axArray *arr, Object* object)
{
	axArrayEnsureExtraCapacity(arr, 1);
	axArrayAppendObject(arr, object);
}

/** Appends objects from plusArr to arr. Behavior undefined if arr doesn't have
 enough capacity. */
void axArrayAppendArray(axArray *arr, axArray *plusArr)
{
	for(unsigned int i = 0; i < plusArr->num; i++)
    {
		axArrayAppendObject(arr, plusArr->arr[i]);
    }
}

/** Appends objects from plusArr to arr. Capacity of arr is increased if needed. */
void axArrayAppendArrayWithResize(axArray *arr, axArray *plusArr)
{
	axArrayEnsureExtraCapacity(arr, plusArr->num);
	axArrayAppendArray(arr, plusArr);
}

/** Inserts an object at index */
void axArrayInsertObjectAtIndex(axArray *arr, Object* object, unsigned int index)
{
	AXAssert(index<=arr->num, "Invalid index. Out of bounds");
	AXAssert(object != NULL, "Invalid parameter!");

	axArrayEnsureExtraCapacity(arr, 1);
	
	unsigned int remaining = arr->num - index;
	if( remaining > 0)
    {
		memmove((void *)&arr->arr[index+1], (void *)&arr->arr[index], sizeof(Object*) * remaining );
    }

    object->retain();
	arr->arr[index] = object;
	arr->num++;
}

/** Swaps two objects */
void axArraySwapObjectsAtIndexes(axArray *arr, unsigned int index1, unsigned int index2)
{
	AXAssert(index1 < arr->num, "(1) Invalid index. Out of bounds");
	AXAssert(index2 < arr->num, "(2) Invalid index. Out of bounds");
	
	Object* object1 = arr->arr[index1];
	
	arr->arr[index1] = arr->arr[index2];
	arr->arr[index2] = object1;
}

/** Removes all objects from arr */
void axArrayRemoveAllObjects(axArray *arr)
{
	while( arr->num > 0 )
    {
		(arr->arr[--arr->num])->release();
    }
}

/** Removes object at specified index and pushes back all subsequent objects.
 Behavior undefined if index outside [0, num-1]. */
void axArrayRemoveObjectAtIndex(axArray *arr, unsigned int index, bool bReleaseObj/* = true*/)
{
    AXAssert(arr && arr->num > 0 && index < arr->num, "Invalid index. Out of bounds");
    if (bReleaseObj)
    {
        AX_SAFE_RELEASE(arr->arr[index]);
    }
    
	arr->num--;
	
	unsigned int remaining = arr->num - index;
	if(remaining>0)
    {
		memmove((void *)&arr->arr[index], (void *)&arr->arr[index+1], remaining * sizeof(Object*));
    }
}

/** Removes object at specified index and fills the gap with the last object,
 thereby avoiding the need to push back subsequent objects.
 Behavior undefined if index outside [0, num-1]. */
void axArrayFastRemoveObjectAtIndex(axArray *arr, unsigned int index)
{
	AX_SAFE_RELEASE(arr->arr[index]);
	unsigned int last = --arr->num;
	arr->arr[index] = arr->arr[last];
}

void axArrayFastRemoveObject(axArray *arr, Object* object)
{
	unsigned int index = axArrayGetIndexOfObject(arr, object);
	if (index != AX_INVALID_INDEX)
    {
		axArrayFastRemoveObjectAtIndex(arr, index);
    }
}

/** Searches for the first occurrence of object and removes it. If object is not
 found the function has no effect. */
void axArrayRemoveObject(axArray *arr, Object* object, bool bReleaseObj/* = true*/)
{
	unsigned int index = axArrayGetIndexOfObject(arr, object);
	if (index != AX_INVALID_INDEX)
    {
		axArrayRemoveObjectAtIndex(arr, index, bReleaseObj);
    }
}

/** Removes from arr all objects in minusArr. For each object in minusArr, the
 first matching instance in arr will be removed. */
void axArrayRemoveArray(axArray *arr, axArray *minusArr)
{
	for(unsigned int i = 0; i < minusArr->num; i++)
    {
		axArrayRemoveObject(arr, minusArr->arr[i]);
    }
}

/** Removes from arr all objects in minusArr. For each object in minusArr, all
 matching instances in arr will be removed. */
void axArrayFullRemoveArray(axArray *arr, axArray *minusArr)
{
	unsigned int back = 0;
	unsigned int i = 0;
	
	for( i = 0; i < arr->num; i++) 
    {
		if( axArrayContainsObject(minusArr, arr->arr[i]) ) 
        {
			AX_SAFE_RELEASE(arr->arr[i]);
			back++;
		} 
        else
        {
			arr->arr[i - back] = arr->arr[i];
        }
	}
	
	arr->num -= back;
}

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS) || (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
// #pragma mark -
// #pragma mark axCArray for Values (c structures)
#endif

/** Allocates and initializes a new C array with specified capacity */
axCArray* axCArrayNew(unsigned int capacity)
{
	if (capacity == 0)
    {
		capacity = 1;
    }

	axCArray *arr = (axCArray*)malloc( sizeof(axCArray) );
	arr->num = 0;
	arr->arr = (void**)malloc( capacity * sizeof(void*) );
	arr->max = capacity;
	
	return arr;
}

/** Frees C array after removing all remaining values. Silently ignores NULL arr. */
void axCArrayFree(axCArray *arr)
{
    if( arr == NULL ) 
    {
        return;
    }
	axCArrayRemoveAllValues(arr);
	
	free(arr->arr);
	free(arr);
}

/** Doubles C array capacity */
void axCArrayDoubleCapacity(axCArray *arr)
{
    axArrayDoubleCapacity((axArray*)arr);
}

/** Increases array capacity such that max >= num + extra. */
void axCArrayEnsureExtraCapacity(axCArray *arr, unsigned int extra)
{
    axArrayEnsureExtraCapacity((axArray*)arr,extra);
}

/** Returns index of first occurrence of value, AX_INVALID_INDEX if value not found. */
unsigned int axCArrayGetIndexOfValue(axCArray *arr, void* value)
{
	unsigned int i;
	
	for( i = 0; i < arr->num; i++)
    {
		if( arr->arr[i] == value ) return i;
    }
	return AX_INVALID_INDEX;
}

/** Returns a Boolean value that indicates whether value is present in the C array. */
bool axCArrayContainsValue(axCArray *arr, void* value)
{
	return axCArrayGetIndexOfValue(arr, value) != AX_INVALID_INDEX;
}

/** Inserts a value at a certain position. Behavior undefined if array doesn't have enough capacity */
void axCArrayInsertValueAtIndex( axCArray *arr, void* value, unsigned int index)
{
	AXAssert( index < arr->max, "axCArrayInsertValueAtIndex: invalid index");
	
	unsigned int remaining = arr->num - index;
    // make sure it has enough capacity
    if (arr->num + 1 == arr->max)
    {
        axCArrayDoubleCapacity(arr);
    }
	// last Value doesn't need to be moved
	if( remaining > 0) {
		// tex coordinates
		memmove((void *)&arr->arr[index+1], (void *)&arr->arr[index], sizeof(void*) * remaining );
	}
	
	arr->num++;
	arr->arr[index] = value;
}

/** Appends an value. Behavior undefined if array doesn't have enough capacity. */
void axCArrayAppendValue(axCArray *arr, void* value)
{
	arr->arr[arr->num] = value;
	arr->num++;
    // double the capacity for the next append action
    // if the num >= max
    if (arr->num >= arr->max)
    {
        axCArrayDoubleCapacity(arr);
    }
}

/** Appends an value. Capacity of arr is increased if needed. */
void axCArrayAppendValueWithResize(axCArray *arr, void* value)
{
	axCArrayEnsureExtraCapacity(arr, 1);
	axCArrayAppendValue(arr, value);
}


/** Appends values from plusArr to arr. Behavior undefined if arr doesn't have
 enough capacity. */
void axCArrayAppendArray(axCArray *arr, axCArray *plusArr)
{
	unsigned int i;
	
	for( i = 0; i < plusArr->num; i++)
    {
		axCArrayAppendValue(arr, plusArr->arr[i]);
    }
}

/** Appends values from plusArr to arr. Capacity of arr is increased if needed. */
void axCArrayAppendArrayWithResize(axCArray *arr, axCArray *plusArr)
{
	axCArrayEnsureExtraCapacity(arr, plusArr->num);
	axCArrayAppendArray(arr, plusArr);
}

/** Removes all values from arr */
void axCArrayRemoveAllValues(axCArray *arr)
{
	arr->num = 0;
}

/** Removes value at specified index and pushes back all subsequent values.
 Behavior undefined if index outside [0, num-1].
 @since v0.99.4
 */
void axCArrayRemoveValueAtIndex(axCArray *arr, unsigned int index)
{
	unsigned int last;
	
	for( last = --arr->num; index < last; index++)
    {
		arr->arr[index] = arr->arr[index + 1];
    }
}

/** Removes value at specified index and fills the gap with the last value,
 thereby avoiding the need to push back subsequent values.
 Behavior undefined if index outside [0, num-1].
 @since v0.99.4
 */
void axCArrayFastRemoveValueAtIndex(axCArray *arr, unsigned int index)
{
	unsigned int last = --arr->num;
	arr->arr[index] = arr->arr[last];
}

/** Searches for the first occurrence of value and removes it. If value is not found the function has no effect.
 @since v0.99.4
 */
void axCArrayRemoveValue(axCArray *arr, void* value)
{
	unsigned int index = axCArrayGetIndexOfValue(arr, value);
	if (index != AX_INVALID_INDEX)
    {
		axCArrayRemoveValueAtIndex(arr, index);
    }
}

/** Removes from arr all values in minusArr. For each Value in minusArr, the first matching instance in arr will be removed.
 @since v0.99.4
 */
void axCArrayRemoveArray(axCArray *arr, axCArray *minusArr)
{
	for(unsigned int i = 0; i < minusArr->num; i++)
    {
		axCArrayRemoveValue(arr, minusArr->arr[i]);
    }
}

/** Removes from arr all values in minusArr. For each value in minusArr, all matching instances in arr will be removed.
 @since v0.99.4
 */
void axCArrayFullRemoveArray(axCArray *arr, axCArray *minusArr)
{
	unsigned int back = 0;
	
	for(unsigned int i = 0; i < arr->num; i++) 
    {
		if( axCArrayContainsValue(minusArr, arr->arr[i]) ) 
        {
			back++;
		} 
        else
        {
			arr->arr[i - back] = arr->arr[i];
        }
	}
	
	arr->num -= back;
}

NS_AX_END
